#include "cbase.h"
#include "freespace.h"
#include "vr/motionsensor.h"
#include "vr/sensor_fusion.h"

unsigned MotionSensor_Thread(void* params);

#define PI 3.141592654f
#define RADIANS_TO_DEGREES(rad) ((float) rad * (float) (180.0 / PI))

QAngle MotionSensor::getOrientation()
{
	QAngle angle;
	angle[PITCH] = _threadState->angle[PITCH];
	angle[ROLL] = _threadState->angle[ROLL];
	angle[YAW] = _threadState->angle[YAW];
	return angle;
}

bool MotionSensor::initialized()
{
	return _initialized;
}

bool MotionSensor::hasOrientation()
{
	return true;
}

MotionSensor::MotionSensor(int deviceNumber) 
{
	Msg("Motion Sensor %i initializing", deviceNumber);
			
	_threadState = new InputThreadState();
	_threadState->quit = false;
	_threadState->isDone = false;
	_threadState->angle = QAngle(0,0,0);

	struct freespace_message message;
	int numIds; 
	int rc;
		
	// Initialize the freespace library
	rc = freespace_init();
	if (rc != FREESPACE_SUCCESS) {
		printf("Initialization error. rc=%d\n", rc);
		return;
	}

	/** --- START EXAMPLE INITIALIZATION OF DEVICE -- **/
	rc = freespace_getDeviceList(&_threadState->deviceId, deviceNumber, &numIds);
	if (numIds == 0) {
		printf("MotionSensor: Didn't find any devices.\n");
		return;
	}

	rc = freespace_openDevice(_threadState->deviceId);
	if (rc != FREESPACE_SUCCESS) {
		printf("MotionSensor: Error opening device: %d\n", rc);
		return;
	}

	rc = freespace_flush(_threadState->deviceId);
	if (rc != FREESPACE_SUCCESS) {
		printf("MotionSensor: Error flushing device: %d\n", rc);
		return;
	}

	memset(&message, 0, sizeof(message));
	message.messageType = FREESPACE_MESSAGE_DATAMODECONTROLV2REQUEST;
	message.dataModeControlV2Request.packetSelect = 2;
	message.dataModeControlV2Request.modeAndStatus |= 0 << 1;

	rc = freespace_sendMessage(_threadState->deviceId, &message);
	if (rc != FREESPACE_SUCCESS) {
		printf("freespaceInputThread: Could not send message: %d.\n", rc);
		return;
	}

	_threadState->handle = CreateSimpleThread(MotionSensor_Thread, _threadState);
	_initialized = true;

	Msg("Motion Sensor %i initialized successfully", deviceNumber);
}


MotionSensor::~MotionSensor() 
{
	_threadState->quit = true;	
	int rc;
	struct freespace_message message;
	
	// wait for thread to shut down
	while (!_threadState->isDone) { }

	printf("\n\nfreespaceInputThread: Cleaning up...\n");
    memset(&message, 0, sizeof(message));
    if (freespace_isNewDevice(_threadState->deviceId)) {
        message.messageType = FREESPACE_MESSAGE_DATAMODECONTROLV2REQUEST;
        message.dataModeControlV2Request.packetSelect = 0;
		message.dataModeControlV2Request.modeAndStatus = 1 << 1;
    } else {
        message.messageType = FREESPACE_MESSAGE_DATAMODEREQUEST;
		message.dataModeRequest.enableUserPosition = 0;
		message.dataModeRequest.inhibitPowerManager = 0;
	}
	try 
	{
		rc = freespace_sendMessage(_threadState->deviceId, &message);
		if (rc != FREESPACE_SUCCESS) {
			printf("freespaceInputThread: Could not send message: %d.\n", rc);
		}

		freespace_closeDevice(_threadState->deviceId);
    
		freespace_exit();
	} 
	catch(...)
	{
		Msg("An exception occurred when shutting down freespace library");
	}
}

unsigned MotionSensor_Thread(void* params)
{
	InputThreadState* state = (InputThreadState*)params;
	int rc;
	struct freespace_message message;
	static uint16_t lastseq = 0;
	int i = 0;
	while (!state->quit)
	{
		
		rc = freespace_readMessage(state->deviceId, &message, 10 /* 10 ms timeout */);
		if (rc == FREESPACE_ERROR_TIMEOUT ||
			rc == FREESPACE_ERROR_INTERRUPTED) {
				continue;
		}

		if (message.messageType == FREESPACE_MESSAGE_BODYFRAME && 
			message.bodyFrame.sequenceNumber != lastseq) {

			lastseq = message.bodyFrame.sequenceNumber;

			MahonyAHRSupdateIMU(message.bodyFrame.angularVelX / 1000.0f, 
				message.bodyFrame.angularVelY / 1000.0f,
				message.bodyFrame.angularVelZ / 1000.0f,
				message.bodyFrame.linearAccelX ,
				message.bodyFrame.linearAccelY ,
				message.bodyFrame.linearAccelZ );

			// convert quaternion to euler angles
			float m11 = (2.0f * q0 * q0) + (2.0f * q1 * q1) - 1.0f;
			float m12 = (2.0f * q1 * q2) + (2.0f * q0 * q3);
			float m13 = (2.0f * q1 * q3) - (2.0f * q0 * q2);
			float m23 = (2.0f * q2 * q3) + (2.0f * q0 * q1);
			float m33 = (2.0f * q0 * q0) + (2.0f * q3 * q3) - 1.0f;

			float roll = RADIANS_TO_DEGREES(atan2f(m23, m33)) + 180;
			float pitch = RADIANS_TO_DEGREES(asinf(-m13));
			float yaw = RADIANS_TO_DEGREES(atan2f(m12, m11));

			state->angle[ROLL]  = roll;
			state->angle[PITCH] = pitch;
			state->angle[YAW]   = yaw;
		}
	}

	state->isDone = true;

	return 0;
}
