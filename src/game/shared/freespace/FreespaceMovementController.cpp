#include "cbase.h"
#include "freespace/FreespaceMovementController.h"

#define PI 3.141592654f
#define RADIANS_TO_DEGREES(rad) ((float) rad * (float) (180.0 / PI))
#define RTD(rad) ((float) rad * (float) (180.0 / PI))
#define DEGREES_TO_RADIANS(deg) ((float) deg * (float) (PI / 180.0))
#define DTR(deg) ((float) deg * (float) (PI / 180.0))

const float MAX_ANGLE_ACCEL = 3;
const float MIN_ANGULAR_CHANGE = (PI / 180.f);

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

static float smoothAngle(float& cur, float& prev, float &lastChange, float max, float accelRate) {
	//currently not taking into account change in direction
	if (abs(lastChange) >= max) {
		//Msg("LastChange %f already beyond max %f ... adjusting\n", RTD(lastChange), RTD(max));
		max *= (abs(lastChange)/max) + accelRate;	
	}

	if (cur < prev) {
		return max(cur, prev - max);	
	} 
	else if (cur > prev) {
		return min(cur, prev + max);	
	}
	return cur;
}

FreespaceMovementController::FreespaceMovementController() {
	_intialized = false;
	PitchAxis = 0;
	RollAxis = 1;
	YawAxis = 2;
	RollEnabled = true;
	
	struct freespace_message message;
	FreespaceDeviceId device;
	int numIds;
	int rc;

	memset(&cachedUserFrame, 0, sizeof(cachedUserFrame));
	
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

void FreespaceMovementController::setOrientationAxis(int pitch, int roll, int yaw) {
	if(pitch >= 0) PitchAxis = pitch;
	if(roll >= 0) RollAxis = roll;
	if(yaw >= 0) YawAxis = yaw;
}

void FreespaceMovementController::setRollEnabled(bool enabled) {
	RollEnabled = enabled;
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

	TrackedAngles[CurSample][RollAxis] = atan2f(m23,m33);
	TrackedAngles[CurSample][PitchAxis] = asinf(-m13);
	TrackedAngles[CurSample][YawAxis] = atan2f(m12,m11);

	if (NumSamples < SMOOTHING_WINDOW_SIZE) {
		NumSamples++;
		CurAngle[RollAxis] = TrackedAngles[CurSample][RollAxis];
		CurAngle[PitchAxis] = TrackedAngles[CurSample][PitchAxis];
		CurAngle[YawAxis] = TrackedAngles[CurSample][YawAxis];
		return 0;
	}

	int i = CurSample+1;
	if(i >= SMOOTHING_WINDOW_SIZE)
		i=0;

	QAngle sum(TrackedAngles[CurSample]);
	//Msg("Tracked pitch:%f roll:%f yaw:%f\n", sum[PitchAxis], sum[RollAxis], sum[YawAxis]);
	while (i != CurSample) {
		sum[RollAxis] += TrackedAngles[i][RollAxis];
		sum[PitchAxis] += TrackedAngles[i][PitchAxis];

		//Handle potential discontinuity from pos to neg on yaw averages
		if(fabs(TrackedAngles[CurSample][YawAxis] - TrackedAngles[i][YawAxis]) > PI) {
			if (TrackedAngles[CurSample][YawAxis] > 0)
				sum[YawAxis] += TrackedAngles[i][YawAxis] + (2*PI);
			else
				sum[YawAxis] += TrackedAngles[i][YawAxis] + (-2*PI);
		}
		else {
			sum[YawAxis]   += TrackedAngles[i][YawAxis];
		}

		i++;
		if (i >= SMOOTHING_WINDOW_SIZE) {
			i = 0;
		}
	}

	QAngle next(
		sum[RollAxis] / SMOOTHING_WINDOW_SIZE,
		sum[PitchAxis] / SMOOTHING_WINDOW_SIZE,
		sum[YawAxis] / SMOOTHING_WINDOW_SIZE
	);

    // correct YawAxis angles for earlier discontinuity fix
    if (next[YawAxis] > PI)
        next[YawAxis] -= (2 * PI);
    else if (next[YawAxis] < -PI)
        next[YawAxis] += (2 * PI);

/*
	SMOOTHING IS SUCH A TRADEOFF BETWEEN SMOOTH AND LAGGY

	next[RollAxis] =  smoothAngle(next[RollAxis], CurAngle[RollAxis], LastChange[RollAxis], DTR(1.5f), .5f);
	next[PitchAxis] = smoothAngle(next[PitchAxis], CurAngle[PitchAxis], LastChange[PitchAxis], DTR(1.f), .75f);


	if (abs(lastChange) < PI/180*.5 && abs(curChange) > DTR(2.f)) {
		Msg("Roll from low change %f to %f\n", RTD(lastChange), RTD(curChange));
		
		int sign = 1;
		if (curChange < 0) sign=-1;
		curChange = DTR(.75) * sign;
		newRoll = TrackedAngles[prev][RollAxis] + curChange;
		Msg("\tRoll adjusted from %f to %f = %f + %f\n\n", RTD(TrackedAngles[CurSample][RollAxis]), RTD(newRoll), RTD(TrackedAngles[prev][RollAxis]), RTD(curChange));
	}
*/

	//track last change used in smoothing
	LastChange[PitchAxis] = next[PitchAxis] - CurAngle[PitchAxis];
	LastChange[RollAxis] = next[RollAxis] - CurAngle[RollAxis];
	LastChange[YawAxis] = next[YawAxis] - CurAngle[YawAxis];

	CurAngle[PitchAxis] = next[PitchAxis];
	CurAngle[RollAxis] = next[RollAxis];
	CurAngle[YawAxis] = next[YawAxis];

	if (RollEnabled) {
		roll = RTD(next[RollAxis]) * -1;
	} else {
		roll = false;
	}
	pitch = RTD(next[PitchAxis]) * -1;  //inverting pitch
	yaw = RTD(next[YawAxis]) * -1;

	Msg("angle: %f %f %f", pitch, roll, yaw);

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
extern void UTIL_getHeadOrientation(float &pitch, float& yaw, float& roll)
{
	if (freespace == NULL) return;
	freespace->getOrientation(pitch, yaw, roll);	
}