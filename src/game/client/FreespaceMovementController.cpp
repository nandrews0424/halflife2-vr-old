#include "cbase.h"
#include "FreespaceMovementController.h"

#define RADIANS_TO_DEGREES(rad) ((float) rad * (float) (180.0 / M_PI))
#define DEGREES_TO_RADIANS(deg) ((float) deg * (float) (M_PI / 180.0))

struct freespace_UserFrame cachedUserFrame;
static  bool test = false;
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

static void getEulerAnglesFromUserFrame(const struct freespace_UserFrame* user,
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
	memset(&_previousAngles, 0, sizeof(_previousAngles));

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
		Msg("Can't read orientation, not initialized");
		return -1;
	}
	 _userFrame = cachedUserFrame;

	getEulerAnglesFromUserFrame(&_userFrame, &_eulerAngles);
	roll = 0;
	pitch = 0;
	yaw = 0;

	roll = RADIANS_TO_DEGREES(_eulerAngles.x);
	pitch = RADIANS_TO_DEGREES(_eulerAngles.y) * -1;
	yaw = RADIANS_TO_DEGREES(_eulerAngles.z) * -1;

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