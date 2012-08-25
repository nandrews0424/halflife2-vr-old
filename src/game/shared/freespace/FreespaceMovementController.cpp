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
struct TrackerData devices[MAX_TRACKERS];
static int activeDevices = 0;

static void receiveMessageCallback(FreespaceDeviceId id,
                            struct freespace_message* message,
                            void* cookie,
                            int result) {
    if (result == FREESPACE_SUCCESS && message != NULL && message->messageType == FREESPACE_MESSAGE_USERFRAME) {
		userFrame = message->userFrame;
    }
}

static void initDevice(FreespaceDeviceId id) {
    struct freespace_message message;
    int rc;
    int idx;

    freespace_setReceiveMessageCallback(id, receiveMessageCallback, NULL);

    rc = freespace_openDevice(id);
    if (rc != 0) {
        printf("Error opening device.\n");
        return;
    }

    rc = freespace_flush(id);
    if (rc != 0) {
        printf("Error flushing device.\n");
        return;
    }

    // Add the device to our list
    for (idx = 0; idx < MAX_TRACKERS; idx++) {
        if (devices[idx].id < 0) {
            rc = 1;
            devices[idx].id = id;
            devices[idx].CurSample = 0;
            devices[idx].CurAngle.Init();
            devices[idx].LastChange.Init();
            Msg("Added device %d to index %d\n", id, idx);
            break;
        }
    }
    if (rc == 0) {
        printf("Could not add device.\n");
    }

    memset(&message, 0, sizeof(message));
    if (freespace_isNewDevice(id)) {
        message.messageType = FREESPACE_MESSAGE_DATAMODECONTROLV2REQUEST;
        message.dataModeControlV2Request.packetSelect = 3;
        message.dataModeControlV2Request.modeAndStatus |= 0 << 1;
    } else {
        message.messageType = FREESPACE_MESSAGE_DATAMODEREQUEST;
        message.dataModeRequest.enableBodyMotion = 1;
        message.dataModeRequest.inhibitPowerManager = 1;
    }

    rc = freespace_sendMessage(id, &message);
    if (rc != FREESPACE_SUCCESS) {
        printf("Could not send message: %d.\n", rc);
    }
	activeDevices++;
	Msg("Device %d initialized", id, idx);
       
}



static void cleanupDevice(FreespaceDeviceId id) {
    struct freespace_message message;
    int rc;
    int idx;

    // Remove the device from our list.
    for (idx = 0; idx < MAX_TRACKERS; idx++) {
        if (devices[idx].id == id) {
            devices[idx].id = -1;
			devices[idx].PitchAxis = 0;
			devices[idx].RollAxis = 1;
			devices[idx].YawAxis = 2;
            break;
        }
    }
    
    Msg("%d> Sending message to enable mouse motion data.\n", id);
    memset(&message, 0, sizeof(message));
    if (freespace_isNewDevice(id)) {
        message.messageType = FREESPACE_MESSAGE_DATAMODECONTROLV2REQUEST;
        message.dataModeControlV2Request.packetSelect = 1;
    } else {
        message.messageType = FREESPACE_MESSAGE_DATAMODEREQUEST;
        message.dataModeRequest.enableMouseMovement = 1;
    }
    rc = freespace_sendMessage(id, &message);
    if (rc != FREESPACE_SUCCESS) {
       Msg("Could not send message: %d.\n", rc);
    } else {
        freespace_flush(id);
        //Sleep(1);
        freespace_flush(id);
	}
    Msg("%d> Cleaning up...\n", id);
    freespace_closeDevice(id);
	activeDevices--;
}


static void hotplugCallback(enum freespace_hotplugEvent event, FreespaceDeviceId id, void* cookie) {
	if (event == FREESPACE_HOTPLUG_REMOVAL) {
        Msg("Closing removed device %d\n", id);
        cleanupDevice(id);
    } else if (event == FREESPACE_HOTPLUG_INSERTION) {
        Msg("Opening newly inserted device %d\n", id);
        initDevice(id);
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
	_initialized = false;
	memset(&userFrame, 0, sizeof(userFrame));
	int rc;

	for (int idx = 0; idx < MAX_TRACKERS; idx++) {
		devices[idx].id = -1;
    }
	
	rc = freespace_init();
	if (rc != FREESPACE_SUCCESS) {
		Msg("Freespace initialization error.  rc=%d", rc);
		return;
	}

	freespace_setDeviceHotplugCallback(hotplugCallback, NULL);
	_initialized = true;
	Msg("Freespace initialization complete");
}
 
FreespaceMovementController::~FreespaceMovementController(){
	Msg("Shutting down Freespace devices");
	
	for (int idx=0; idx < MAX_TRACKERS; idx++) {
		if (devices[idx].id < 0) {
			continue;
		}
		cleanupDevice(devices[idx].id);
	}

    freespace_exit();
}

void FreespaceMovementController::setOrientationAxis(int pitch, int roll, int yaw) {
	int idx=0;
	if(pitch >= 0) devices[idx].PitchAxis = pitch;
	if(roll >= 0) devices[idx].RollAxis = roll;
	if(yaw >= 0) devices[idx].YawAxis = yaw;
}

void FreespaceMovementController::setRollEnabled(bool enabled) {
	int idx=0;
	devices[idx].RollEnabled = enabled;
}

int FreespaceMovementController::getOrientation(float &pitch, float &yaw, float &roll){
	int idx = 0;
	if (idx > activeDevices-1) {
		return -1;
	}

	TrackerData* t = &devices[idx];
	//TODO: USER FRAME WILL HAVE TO BE PER INPUT DEVICE
	
	//idx axis for this tracker
	int pitchAxis = t->PitchAxis;
	int rollAxis = t->RollAxis;
	int yawAxis = t->YawAxis;

	/* add locks around userframe here */

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
    // rotating the world around the object-> We take the conjugate to
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
    t->CurSample++;

	Msg("sample %d, window size %d\n", t->CurSample, SMOOTHING_WINDOW_SIZE);

    if (t->CurSample >= SMOOTHING_WINDOW_SIZE) {
        t->CurSample = 0;
		Msg("resetting sample num");
	}

	//TODO: values aren't persisting properly on tracker objects.....
	Msg("Current sample %d\n", t->CurSample);


	t->TrackedAngles[t->CurSample][rollAxis] = atan2f(m23,m33);
	t->TrackedAngles[t->CurSample][pitchAxis] = asinf(-m13);
	t->TrackedAngles[t->CurSample][yawAxis] = atan2f(m12,m11);

	if (t->NumSamples < SMOOTHING_WINDOW_SIZE) {
		t->NumSamples++;
		t->CurAngle[rollAxis] = t->TrackedAngles[t->CurSample][rollAxis];
		t->CurAngle[pitchAxis] = t->TrackedAngles[t->CurSample][pitchAxis];
		t->CurAngle[yawAxis] = t->TrackedAngles[t->CurSample][yawAxis];
		Msg("Tracked Sample %d/%d pitch:%f roll:%f yaw:%f\n", t->CurSample, t->NumSamples, t->CurAngle[pitchAxis], t->CurAngle[rollAxis], t->CurAngle[yawAxis]);
		return 0;
	}

	int i = t->CurSample+1;
	if(i >= SMOOTHING_WINDOW_SIZE)
		i=0;

	QAngle sum(t->TrackedAngles[t->CurSample]);
	Msg("Tracked Sample %d pitch:%f roll:%f yaw:%f\n", t->CurSample, sum[pitchAxis], sum[rollAxis], sum[yawAxis]);
	while (i != t->CurSample) {
		sum[rollAxis] += t->TrackedAngles[i][rollAxis];
		sum[pitchAxis] += t->TrackedAngles[i][pitchAxis];
	
		//Handle potential discontinuity from pos to neg on yaw averages
		if(fabs(t->TrackedAngles[t->CurSample][yawAxis] - t->TrackedAngles[i][yawAxis]) > PI) {
			if (t->TrackedAngles[t->CurSample][yawAxis] > 0)
				sum[yawAxis] += t->TrackedAngles[i][yawAxis] + (2*PI);
			else
				sum[yawAxis] += t->TrackedAngles[i][yawAxis] + (-2*PI);
		}
		else {
			sum[yawAxis]   += t->TrackedAngles[i][yawAxis];
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

	Msg("Resample %d  pitch:%f roll:%f yaw:%f\n", t->CurSample, next[pitchAxis], next[rollAxis], next[yawAxis]);

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
	t->LastChange[pitchAxis] = next[pitchAxis] - t->CurAngle[pitchAxis];
	t->LastChange[rollAxis] = next[rollAxis] - t->CurAngle[rollAxis];
	t->LastChange[yawAxis] = next[yawAxis] - t->CurAngle[yawAxis];

	t->CurAngle[pitchAxis] = next[pitchAxis];
	t->CurAngle[rollAxis] = next[rollAxis];
	t->CurAngle[yawAxis] = next[yawAxis];

	if (t->RollEnabled) {
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
	return _initialized  && activeDevices > 0; 
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