#include "cbase.h"
#include "freespace/FreespaceMovementController.h"

#define PI 3.141592654f
#define RADIANS_TO_DEGREES(rad) ((float) rad * (float) (180.0 / PI))
#define RTD(rad) ((float) rad * (float) (180.0 / PI))
#define DEGREES_TO_RADIANS(deg) ((float) deg * (float) (PI / 180.0))
#define DTR(deg) ((float) deg * (float) (PI / 180.0))

const float MAX_ANGLE_ACCEL = 3;
const float MIN_ANGULAR_CHANGE = (PI / 180.f);

FreespaceMovementController* freespace;
freespace_UserFrame userFrame;


static void receiveMessageCallback(FreespaceDeviceId id,
                            struct freespace_message* message,
                            void* cookie,
                            int result) {
    if (result == FREESPACE_SUCCESS && message != NULL && message->messageType == FREESPACE_MESSAGE_USERFRAME) {
		userFrame = message->userFrame;
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
	
	memset(&userFrame, 0, sizeof(userFrame));
	_initialized = false;
	struct freespace_message message;
	FreespaceDeviceId* devices;
	int numDevices;
	int rc;

	rc = freespace_init();
	if (rc != FREESPACE_SUCCESS) {
		Msg("Freespace initialization error.  rc=%d", rc);
		return;
	}
	
	rc = freespace_getDeviceList(devices, MAX_TRACKERS, &numDevices);
	if (numDevices == 0) {
		Msg("Freespace no devices found");
		return;
	}

	for (int i=0; i < numDevices; i++) {

		FreespaceDeviceId device = devices[i];

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

		TrackerData t;
		t.deviceId = device;
		trackers.AddToHead(t);
		_numTrackers++;
	}
   
	_initialized = true;
	Msg("Freespace initialization complete %d", _numTrackers);
}
 
FreespaceMovementController::~FreespaceMovementController(){
	Msg("Shutting down Freespace api");
	
	struct freespace_message message;
	memset(&message, 0, sizeof(message));
    int rc;

	for (int i=0; i < trackers.Size(); i++) {
		printf("\n\nfreespaceInputThread: Cleaning up...\n");
		
		if (freespace_isNewDevice(trackers[i].deviceId)) {
			message.messageType = FREESPACE_MESSAGE_DATAMODECONTROLV2REQUEST;
			message.dataModeControlV2Request.packetSelect = 0;
			message.dataModeControlV2Request.modeAndStatus = 1 << 1;
		} else {
			message.messageType = FREESPACE_MESSAGE_DATAMODEREQUEST;
			message.dataModeRequest.enableUserPosition = 0;
			message.dataModeRequest.inhibitPowerManager = 0;
		}

		rc = freespace_sendMessage(trackers[i].deviceId, &message);
		if (rc != FREESPACE_SUCCESS) {
			printf("freespaceInputThread: Could not send message: %d.\n", rc);
		}

		freespace_closeDevice(trackers[i].deviceId);
	}

    freespace_exit();
}

void FreespaceMovementController::setOrientationAxis(int pitch, int roll, int yaw) {
	int idx=0;
	if(pitch >= 0) trackers[idx].PitchAxis = pitch;
	if(roll >= 0) trackers[idx].RollAxis = roll;
	if(yaw >= 0) trackers[idx].YawAxis = yaw;
}

void FreespaceMovementController::setRollEnabled(bool enabled) {
	int idx=0;
	trackers[idx].RollEnabled = enabled;
}

int FreespaceMovementController::getOrientation(float &pitch, float &yaw, float &roll){
	int idx = 0;
	if (idx >= trackers.Size()){
		return -1;
	}

	TrackerData t = trackers[idx];
	//TODO: USER FRAME WILL HAVE TO BE PER INPUT DEVICE
	
	//idx axis for this tracker
	int pitchAxis = t.PitchAxis;
	int rollAxis = t.RollAxis;
	int yawAxis = t.YawAxis;

	 // Get the quaternion vector
    float w = userFrame.angularPosA;
    float x = userFrame.angularPosB;
    float y = userFrame.angularPosC;
    float z = userFrame.angularPosD;

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
    t.CurSample++;
    if (t.CurSample >= SMOOTHING_WINDOW_SIZE)
        t.CurSample = 0;

	t.TrackedAngles[t.CurSample][rollAxis] = atan2f(m23,m33);
	t.TrackedAngles[t.CurSample][pitchAxis] = asinf(-m13);
	t.TrackedAngles[t.CurSample][yawAxis] = atan2f(m12,m11);

	if (t.NumSamples < SMOOTHING_WINDOW_SIZE) {
		t.NumSamples++;
		t.CurAngle[rollAxis] = t.TrackedAngles[t.CurSample][rollAxis];
		t.CurAngle[pitchAxis] = t.TrackedAngles[t.CurSample][pitchAxis];
		t.CurAngle[yawAxis] = t.TrackedAngles[t.CurSample][yawAxis];
		return 0;
	}

	int i = t.CurSample+1;
	if(i >= SMOOTHING_WINDOW_SIZE)
		i=0;

	QAngle sum(t.TrackedAngles[t.CurSample]);
	//Msg("Tracked pitch:%f roll:%f yaw:%f\n", sum[PitchAxis], sum[RollAxis], sum[YawAxis]);
	while (i != t.CurSample) {
		sum[rollAxis] += t.TrackedAngles[i][rollAxis];
		sum[pitchAxis] += t.TrackedAngles[i][pitchAxis];

		//Handle potential discontinuity from pos to neg on yaw averages
		if(fabs(t.TrackedAngles[t.CurSample][yawAxis] - t.TrackedAngles[i][yawAxis]) > PI) {
			if (t.TrackedAngles[t.CurSample][yawAxis] > 0)
				sum[yawAxis] += t.TrackedAngles[i][yawAxis] + (2*PI);
			else
				sum[yawAxis] += t.TrackedAngles[i][yawAxis] + (-2*PI);
		}
		else {
			sum[yawAxis]   += t.TrackedAngles[i][yawAxis];
		}

		i++;
		if (i >= SMOOTHING_WINDOW_SIZE) {
			i = 0;
		}
	}

	QAngle next(
		sum[rollAxis] / SMOOTHING_WINDOW_SIZE,
		sum[pitchAxis] / SMOOTHING_WINDOW_SIZE,
		sum[yawAxis] / SMOOTHING_WINDOW_SIZE
	);

    // correct YawAxis angles for earlier discontinuity fix
    if (next[yawAxis] > PI)
        next[yawAxis] -= (2 * PI);
    else if (next[yawAxis] < -PI)
        next[yawAxis] += (2 * PI);

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
	t.LastChange[pitchAxis] = next[pitchAxis] - t.CurAngle[pitchAxis];
	t.LastChange[rollAxis] = next[rollAxis] - t.CurAngle[rollAxis];
	t.LastChange[yawAxis] = next[yawAxis] - t.CurAngle[yawAxis];

	t.CurAngle[pitchAxis] = next[pitchAxis];
	t.CurAngle[rollAxis] = next[rollAxis];
	t.CurAngle[yawAxis] = next[yawAxis];

	if (t.RollEnabled) {
		roll = RTD(next[rollAxis]) * -1;
	} else {
		roll = false;
	}
	pitch = RTD(next[pitchAxis]) * -1;  //inverting pitch
	yaw = RTD(next[yawAxis]) * -1;
	return 0;
}
  
int FreespaceMovementController::getPosition(float &x, float &y, float &z){
	return 0;
}
 
void FreespaceMovementController::update() {
	freespace_perform();
}
 
bool FreespaceMovementController::hasOrientationTracking() {
	return _initialized  && _numTrackers > 0; 
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

extern void UTIL_getWeaponOrientation(float &pitch, float& yaw, float& roll)
{
	if (freespace == NULL) return;
	freespace->getOrientation(pitch, yaw, roll);	
}