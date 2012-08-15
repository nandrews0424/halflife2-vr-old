#include "cbase.h"
#include "FreespaceMovementController.h"

#define RADIANS_TO_DEGREES(rad) ((float) rad * (float) (180.0 / M_PI))
#define DEGREES_TO_RADIANS(deg) ((float) deg * (float) (M_PI / 180.0))

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

static void getAnglesFromUserFrame(const struct freespace_UserFrame* user,
                                        struct Vec3f* eulerAngles) {
    struct FS_Quaternion q;
    q_quatFromUserFrame(&q, user);

    // The Freespace quaternion gives the rotation in terms of
    // rotating the world around the object. We take the conjugate to
    // get the rotation in the object's reference frame.
    q_conjugate(&q, &q);

    // Convert quaternion to Euler angles
    q_toEulerAngles(eulerAngles, &q);
}


//todo: move to init so it can be call from a console command later to reinitialize
FreespaceMovementController::FreespaceMovementController() {
	
	struct freespace_message message;
	FreespaceDeviceId device;
	int numIds;
	int rc;

	memset(&cachedUserFrame, 0, sizeof(cachedUserFrame));
	memset(&_userFrame, 0, sizeof(_userFrame));
	
	rc = freespace_init();
	if (rc != FREESPACE_SUCCESS) {
		Msg("Freespace initialization error.  rc=%d", rc);
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
        message.dataModeControlV2Request.packetSelect = 3;
        message.dataModeControlV2Request.modeAndStatus |= 0 << 1;
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
        message.dataModeControlV2Request.packetSelect = 1;
    } else {
        message.messageType = FREESPACE_MESSAGE_DATAMODEREQUEST;
        message.dataModeRequest.enableMouseMovement = 1;
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
	 _userFrame = cachedUserFrame;

	getAnglesFromUserFrame(&_userFrame, &_angle);
	
	QAngle angle(RADIANS_TO_DEGREES(_angle.x), RADIANS_TO_DEGREES(_angle.y), RADIANS_TO_DEGREES(_angle.z));
	_recentAngles.AddToTail(angle);
	
	
	// todo: make convar
	if (_recentAngles.Size() > 5) {
		_recentAngles.Remove(0);
	}

	// Should you sum of unit vectors of each angle to allow averaging on yaw where there exits the wrap around point 180 to -180
	QAngle avg(0,0,0);
	int size = _recentAngles.Size();
	for (int i = 0; i < size; i++)
	{
		avg.x += _recentAngles[i].x;
		avg.y += _recentAngles[i].y;
		avg.z += _recentAngles[i].z;
	}
	
	avg.x /= size;
	avg.y /= size;
	avg.z /= size;

	//Msg("roll: (%2f,%2f) pitch: (%2f,%2f) yaw: (%2f,%2f)", angle.x, avg.x, angle.y, avg.y, angle.z, avg.z);

	roll = avg.x;
	pitch = avg.y * -1;
	yaw = angle.z * -1;  // yaw smoothing is causing odd behavior so ignoring for now
	
	return 0;
}
  
int FreespaceMovementController::getPosition(float &x, float &y, float &z){
	return 0;
}
 
void FreespaceMovementController::update() {
	freespace_perform();
}
 
bool FreespaceMovementController::hasOrientationTracking() {
	return true; 
}
  
bool FreespaceMovementController::hasPositionTracking() {
	return false;
}

//static helper methods for external libs
static void Util_getOrientation(float &pitch, float& yaw, float& roll)
{
	if (freespace == null) return;
	freespace->getOrientation(pitch, yaw, roll);	
}