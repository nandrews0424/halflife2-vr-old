#include "cbase.h"
#include <windows.h>
#include "freespace.h"
#include "vr/motionsensor.h"
#include "vr/sensor_fusion.h"
#include <windows.h>
#include <sys/timeb.h>

#define PI 3.141592654f
#define RADIANS_TO_DEGREES(rad) ((float) rad * (float) (180.0 / PI))

static void hotplug_Callback(enum freespace_hotplugEvent evnt, FreespaceDeviceId id, void* params) {
	MotionSensor* sensor = (MotionSensor*) params;

	if (evnt == FREESPACE_HOTPLUG_REMOVAL) { 
        Msg("Closing removed freespace device %d\n", id);
        sensor->_removeDevice(id);
    } else if (evnt == FREESPACE_HOTPLUG_INSERTION) {
        Msg("Opening newly inserted freespace device %d\n", id);
        sensor->_initDevice(id);
    }
}

unsigned MotionSensor_Thread(void* params)
{
	InputThreadState* state = (InputThreadState*)params;
	int rc;
	FreespaceDeviceId id;
	struct freespace_message message;
	static uint16_t lastseq[MAX_SENSORS];
	Quaternion q;
	
	for (int i = 0; i<MAX_SENSORS; i++){
		lastseq[i] = 0;
		state->sampleCount[i] = 0;
		state->errorCount[i] = 0;
		state->lastReturnCode[i] = 0;
		state->pitch[i] = 0;
		state->roll[i] = 0;
		state->yaw[i] = 0;
	}

	int i = 0;
	while (!state->quit)
	{
		i = ++i % MAX_SENSORS;
		id = state->deviceIds[i];
				
		if (id < 0)
			continue;

		rc = freespace_readMessage(id, &message, 5);
		state->lastReturnCode[i] = rc;
		if (rc == FREESPACE_ERROR_TIMEOUT || rc == FREESPACE_ERROR_INTERRUPTED) {
			state->errorCount[i]++;
			continue;
		}

		if (message.messageType == FREESPACE_MESSAGE_BODYFRAME && message.bodyFrame.sequenceNumber != lastseq[i]) {

			lastseq[i] = message.bodyFrame.sequenceNumber;

			state->sensorFusion[i].MahonyAHRSupdateIMU(
				message.bodyFrame.angularVelX / 1000.0f, 
				message.bodyFrame.angularVelY / 1000.0f,
				message.bodyFrame.angularVelZ / 1000.0f,
				message.bodyFrame.linearAccelX ,
				message.bodyFrame.linearAccelY ,
				message.bodyFrame.linearAccelZ );

			q = state->sensorFusion[i].Read();

			// convert quaternion to euler angles
			float m11 = (2.0f * q[0] * q[0]) + (2.0f * q[1] * q[1]) - 1.0f;
			float m12 = (2.0f * q[1] * q[2]) + (2.0f * q[0] * q[3]);
			float m13 = (2.0f * q[1] * q[3]) - (2.0f * q[0] * q[2]);
			float m23 = (2.0f * q[2] * q[3]) + (2.0f * q[0] * q[1]);
			float m33 = (2.0f * q[0] * q[0]) + (2.0f * q[3] * q[3]) - 1.0f;

			float roll = RADIANS_TO_DEGREES(atan2f(m23, m33)) + 180;
			float pitch = RADIANS_TO_DEGREES(asinf(-m13));
			float yaw = RADIANS_TO_DEGREES(atan2f(m12, m11));

			
			state->deviceAngles[i][ROLL]  = roll;
			state->deviceAngles[i][PITCH] = pitch;
			state->deviceAngles[i][YAW]   = yaw;

			state->pitch[i] = pitch;
			state->roll[i] = roll;
			state->yaw[i] = yaw;

			state->sampleCount[i]++;
		}
	}

	return 0;
}

MotionSensor* _freespaceSensor;

MotionSensor::MotionSensor() 
{
	Msg("Initializing freespace drivers\n");

	struct freespace_message message;
	int deviceCount; 
	int rc;
	_deviceCount = 0;
	_threadState.quit = false;
	
	for (int i=0; i<MAX_SENSORS; i++) {
		_threadState.deviceAngles[i].Init();
		_threadState.deviceIds[i] = -1;
	}
				
	if ( freespace_init() != FREESPACE_SUCCESS ) {
		printf("Freespace initialization error. rc=%d\n", rc);
		return;
	}
	
	freespace_setDeviceHotplugCallback(hotplug_Callback, this);
	freespace_perform();

	_threadState.handle = CreateSimpleThread(MotionSensor_Thread, &_threadState);
	_freespaceSensor = this;
	_initialized = true;

	Msg("Freespace initialized successfully\n");
}


MotionSensor::~MotionSensor() 
{
	Msg("Shutting downfreespace devices\n");
	_threadState.quit = true;	
	int rc;
	int i = 0;
	struct freespace_message message;
	
	Msg("Waiting on input thread to join\n");
	if (ThreadJoin(_threadState.handle, 1000)) {
		Msg("Freespace input thread shut down successfully...\n");
	} else {
		Msg("Freespace input thread join timed out, releasing thread handle...\n");
		ReleaseThreadHandle(_threadState.handle);
	}
    Msg("Shutting down Freespace devices\n");

	for (int idx=0; idx < MAX_SENSORS; idx++) {
		if (_threadState.deviceIds[idx] >= 0) {
			_removeDevice(_threadState.deviceIds[idx]);
		}
	}

	freespace_exit();
	
	Msg("Freespace interface shutdown\n");
}

void MotionSensor::_initDevice(FreespaceDeviceId id) 
{
	if (_deviceCount >= MAX_SENSORS) {
		Msg("Too many devices, can't add new freespace device %i\n", id);
		return;
    }

	Msg("Opening freespace device %i\n", id);
    int rc = freespace_openDevice(id);
    if (rc != 0) {
        Msg("Error opening device.\n");
        return;
    }

    rc = freespace_flush(id);
    if (rc != 0) {
        Msg("Error flushing device.\n");
        return;
    }
		
    struct freespace_message message;
    memset(&message, 0, sizeof(message));
	message.messageType = FREESPACE_MESSAGE_DATAMODECONTROLV2REQUEST;
	message.dataModeControlV2Request.packetSelect = 2;
	message.dataModeControlV2Request.modeAndStatus |= 0 << 1;
    
    if (freespace_sendMessage(id, &message) != FREESPACE_SUCCESS) {
        Msg("Freespace unable to send message: returned %d.\n", rc);
		return;
    }

	//todo: freespace_setReceiveMessageCallback(id, receiveMessageCallback, NULL);

	_threadState.deviceAngles[_deviceCount].Init();
	_threadState.deviceIds[_deviceCount] = id;
    _deviceCount++;
	
	Msg("Freespace sensor %i initialized (id: %i)", _deviceCount, id);
}

void MotionSensor::_removeDevice(FreespaceDeviceId id) {
    struct freespace_message message;
    int rc;
    int i;

    // Remove the device from our list.
    for (int i = 0; i < MAX_SENSORS; i++) {
        if (_threadState.deviceIds[i] == id) {
            _threadState.deviceIds[i] = -1;
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
    
    if (freespace_sendMessage(id, &message) != FREESPACE_SUCCESS) {
       Msg("Could not send message: %d.\n", rc);
    } else {
        freespace_flush(id);
	}

    Msg("%d> Cleaning up freespace device...\n", id);
    freespace_closeDevice(id);
	_deviceCount--;
}

void MotionSensor::getOrientation(int deviceIndex, QAngle& angle)
{
	//Msg("Device Orientation %i (%i samples, %i errors, %i returned)\n", deviceIndex, _threadState.sampleCount[deviceIndex], _threadState.errorCount[deviceIndex], _threadState.lastReturnCode[deviceIndex]);
	
	if (_threadState.deviceIds[deviceIndex] == -1) {
		angle.Init();
		return;
	}

	angle[PITCH] = _threadState.pitch[deviceIndex];
	angle[ROLL] = _threadState.roll[deviceIndex];
	angle[YAW] = _threadState.yaw[deviceIndex];
}

bool MotionSensor::initialized()
{
	return _initialized;
}

bool MotionSensor::hasOrientation()
{
	return true;
}
