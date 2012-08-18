#include "cbase.h"
#include "freespace/FreespaceMovementController.h"

#define PI 3.141592654f
#define RADIANS_TO_DEGREES(rad) ((float) rad * (float) (180.0 / PI))
#define DEGREES_TO_RADIANS(deg) ((float) deg * (float) (PI / 180.0))
#define SMOOTHING_WINDOW_SIZE 5

const float MAX_ANGLE_ACCEL = 3;
const float MIN_ANGULAR_CHANGE = (PI / 180.f);

//QANGLE INDICES
const int PITCH_I = 0;
const int ROLL_I = 1;
const int YAW_I = 2;


QAngle TrackedAngles[SMOOTHING_WINDOW_SIZE];
int CurSample = -1;
int NumSamples = 0;

struct freespace_UserFrame cachedUserFrame;
FreespaceMovementController* freespace;

static void receiveMessageCallback(FreespaceDeviceId id,
                            struct freespace_message* message,
                            void* cookie,
                            int result) {
    if (result == FREESPACE_SUCCESS && message != NULL && message->messageType == FREESPACE_MESSAGE_USERFRAME) {
   		cachedUserFrame = message->userFrame;
    } else if (result == FREESPACE_SUCCESS && message != NULL && message->messageType == FREESPACE_MESSAGE_BODYFRAME) {
		Msg("A Bodyframe callback was recieved with linear acceleration Callback received: %d", message->messageType);
	}
}

//todo: move to init so it can be call from a console command later to reinitialize
FreespaceMovementController::FreespaceMovementController() {
	_intialized = false;
	struct freespace_message message;
	FreespaceDeviceId device;
	int numIds;
	int rc;

	memset(&cachedUserFrame, 0, sizeof(cachedUserFrame));
	memset(&_userFrame, 0, sizeof(_userFrame));
	
	rc = freespace_init();
	if (rc != FREESPACE_SUCCESS) {
		Msg("Freespace initialization error.  rc=%d", rc);
		return;
	}
	
	rc = freespace_getDeviceList(&device, 1, &numIds);
	if (numIds == 0) {
		Msg("Freespace no devices found");
		return;
	}

	rc = freespace_openDevice(device);
	if (rc != FREESPACE_SUCCESS) {
		Msg("Freespace: Error opening device: %d", rc);
		return;
	}

	freespace_setReceiveMessageCallback(device, receiveMessageCallback, NULL);

	rc = freespace_flush(device);
    if (rc != FREESPACE_SUCCESS) {
        Msg("freespaceInputThread: Error flushing device: %d\n", rc);
        return;
    }
	
	//allocate space for structs
   
	 if (freespace_isNewDevice(device)) {
        message.messageType = FREESPACE_MESSAGE_DATAMODECONTROLV2REQUEST;
        message.dataModeControlV2Request.packetSelect = 3;  // User Frame (orientation)
        message.dataModeControlV2Request.modeAndStatus = 0;
    } else {
        message.messageType = FREESPACE_MESSAGE_DATAMODEREQUEST;
        message.dataModeRequest.enableBodyMotion = 1;
        message.dataModeRequest.inhibitPowerManager = 1;
    }
    
	rc = freespace_sendMessage(device, &message);
    if (rc != FREESPACE_SUCCESS) {
        Msg("freespace: Could not send message: %d.\n", rc);
    }
   
    _device = device;
	_intialized = true;
	Msg("Freespace initialization complete");
}
 
FreespaceMovementController::~FreespaceMovementController(){

	struct freespace_message message;
    int rc;

    printf("\n\nfreespaceInputThread: Cleaning up...\n");
    memset(&message, 0, sizeof(message));
    if (freespace_isNewDevice(_device)) {
        message.messageType = FREESPACE_MESSAGE_DATAMODECONTROLV2REQUEST;
        message.dataModeControlV2Request.packetSelect = 0;
		message.dataModeControlV2Request.modeAndStatus = 1 << 1;
    } else {
        message.messageType = FREESPACE_MESSAGE_DATAMODEREQUEST;
		message.dataModeRequest.enableUserPosition = 0;
		message.dataModeRequest.inhibitPowerManager = 0;
	}

    rc = freespace_sendMessage(_device, &message);
    if (rc != FREESPACE_SUCCESS) {
        printf("freespaceInputThread: Could not send message: %d.\n", rc);
    }

    freespace_closeDevice(_device);
    
    freespace_exit();
	
}
 
 int FreespaceMovementController::getOrientation(float &pitch, float &yaw, float &roll){
	
	if (!_intialized) {
		pitch = 0;
		roll = 0;
		yaw = 0;
		return 0;
	}
	
	 // Get the quaternion vector
    float w = cachedUserFrame.angularPosA;
    float x = cachedUserFrame.angularPosB;
    float y = cachedUserFrame.angularPosC;
    float z = cachedUserFrame.angularPosD;

    // normalize the vector
    float len = sqrtf((w*w) + (x*x) + (y*y) + (z*z));
    w /= len;
    x /= len;
    y /= len;
    z /= len;

    // The Freespace quaternion gives the rotation in terms of
    // rotating the world around the object. We take the conjugate to
    // get the rotation in the object's reference frame.
    w = w;
    x = -x;
    y = -y;
    z = -z;

    // Convert to angles in radians
    float m11 = (2.0f * w * w) + (2.0f * x * x) - 1.0f;
    float m12 = (2.0f * x * y) + (2.0f * w * z);
    float m13 = (2.0f * x * z) - (2.0f * w * y);
    float m23 = (2.0f * y * z) + (2.0f * w * x);
    float m33 = (2.0f * w * w) + (2.0f * z * z) - 1.0f;

	// Find the index into the rotating sample window
    CurSample++;
    if (CurSample >= SMOOTHING_WINDOW_SIZE)
        CurSample = 0;

	TrackedAngles[CurSample][ROLL_I] = atan2f(m23,m33);
	TrackedAngles[CurSample][PITCH_I] = asinf(-m13);
	TrackedAngles[CurSample][YAW_I] = atan2f(m12,m11);

	if (NumSamples < SMOOTHING_WINDOW_SIZE) {
		NumSamples++;
		return 0;
	}

	int i = CurSample+1;
	if(i >= SMOOTHING_WINDOW_SIZE)
		i=0;

	QAngle sum(TrackedAngles[CurSample]);
	//Msg("Tracked pitch:%f roll:%f yaw:%f\n", sum[PITCH_I], sum[ROLL_I], sum[YAW_I]);
	while (i != CurSample) {
		sum[ROLL_I] += TrackedAngles[i][ROLL_I];
		sum[PITCH_I] += TrackedAngles[i][PITCH_I];

		//Handle potential discontinuity from pos to neg on yaw averages
		if(fabs(TrackedAngles[CurSample][YAW_I] - TrackedAngles[i][YAW_I]) > PI) {
			if (TrackedAngles[CurSample][YAW_I] > 0)
				sum[YAW_I] += TrackedAngles[i][YAW_I] + (2*PI);
			else
				sum[YAW_I] += TrackedAngles[i][YAW_I] + (-2*PI);
		}
		else {
			sum[YAW_I]   += TrackedAngles[i][YAW_I];
		}

		i++;
		if (i >= SMOOTHING_WINDOW_SIZE) {
			i = 0;
		}
	}

	QAngle next(
		sum[ROLL_I] / SMOOTHING_WINDOW_SIZE,
		sum[PITCH_I] / SMOOTHING_WINDOW_SIZE,
		sum[YAW_I] / SMOOTHING_WINDOW_SIZE
	);

    // correct YAW_I angles for earlier discontinuity fix
    if (next[YAW_I] > PI)
        next[YAW_I] -= (2 * PI);
    else if (next[YAW_I] < -PI)
        next[YAW_I] += (2 * PI);

	//TODO: limit large changes in acceleration
	
	//Msg("Tracked(%d) pitch:%f roll:%f yaw:%f\n", TrackedAngles[CurSample][PITCH_I], TrackedAngles[CurSample][ROLL_I], TrackedAngles[CurSample][YAW_I], CurSample);
	//Msg("Next Avg    pitch:%f roll:%f yaw:%f\n", next[PITCH_I], next[ROLL_I], next[YAW_I]);
	
	roll = RADIANS_TO_DEGREES(next[ROLL_I]) * -1;
	pitch = RADIANS_TO_DEGREES(next[PITCH_I]) * -1;  //inverting pitch
	yaw = RADIANS_TO_DEGREES(next[YAW_I]) * -1;
	
	return 0;
}
  
int FreespaceMovementController::getPosition(float &x, float &y, float &z){
	return 0;
}
 
void FreespaceMovementController::update() {
	if (_intialized) {
		freespace_perform();
	}
}
 
bool FreespaceMovementController::hasOrientationTracking() {
	return _intialized; 
}
  
bool FreespaceMovementController::hasPositionTracking() {
	return false;
}

//static helper methods for external libs
static void UTIL_getHeadOrientation(float &pitch, float& yaw, float& roll)
{
	if (freespace == NULL) return;
	freespace->getOrientation(pitch, yaw, roll);	
}