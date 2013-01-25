#include "cbase.h"
#include "freespace.h"
#include "vr/motionsensor.h"
#include "vr/sensor_fusion.h"

unsigned MotionSensor_Thread(void* params);
static void hotplug_Callback(enum freespace_hotplugEvent event, FreespaceDeviceId id, void* cookie);
static void init_Device(FreespaceDeviceId id);
static void cleanup_Device(FreespaceDeviceId id);

#define PI 3.141592654f
#define RADIANS_TO_DEGREES(rad) ((float) rad * (float) (180.0 / PI))

MotionSensor* _freespaceSensor;

QAngle MotionSensor::getOrientation(int deviceIndex)
{
	QAngle angle;

	if (_threadState.deviceIds[deviceIndex] == -1) {
		Msg("Unable to retrieve orientation of freespace device %i, not properly initialized\n", deviceIndex);
		angle.Init();
		return angle;
	}
	
	VectorCopy(_threadState.deviceAngles[deviceIndex], angle);
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

MotionSensor::MotionSensor() 
{
	Msg("Initializing freespace drivers");

	struct freespace_message message;
	int numIds; 
	int rc;
	_deviceCount = 0;

	_threadState.quit = false;
	_threadState.isDone = false;

	for (int i=0; i<MAX_SENSORS; i++) {
		_threadState.deviceAngles[i].Init();
		_threadState.deviceIds[i] = -1;
	}
			
	if ( freespace_init() != FREESPACE_SUCCESS) {
		printf("Freespace initialization error. rc=%d\n", rc);
		return;
	}

	freespace_setDeviceHotplugCallback(hotplug_Callback, NULL);

	_threadState.handle = CreateSimpleThread(MotionSensor_Thread, &_threadState);
	_freespaceSensor = this;
	_initialized = true;

	Msg("Freespace initialized successfully");
}


MotionSensor::~MotionSensor() 
{
	_threadState.quit = true;	
	int rc;
	int i = 0;
	struct freespace_message message;
	
	// wait for thread to shut down
	while (!_threadState.isDone) { 
		i++; 
	}

	printf("\n\nfreespaceInputThread: Shut down successfully...\n");
    
    Msg("Shutting down Freespace devices");

	for (int idx=0; idx < MAX_SENSORS; idx++) {
		if (_threadState.deviceIds[idx] >= 0) {
			_removeDevice(_threadState.deviceIds[idx]);
		}
	}

	freespace_exit();
}

void MotionSensor::_initDevice(FreespaceDeviceId id) 
{
	if (_deviceCount >= MAX_SENSORS) {
		Msg("Too many devices, can't add new freespace device %i\n", id);
		return;
    }
	    
    int rc = freespace_openDevice(id);
    if (rc != 0) {
        printf("Error opening device.\n");
        return;
    }

    rc = freespace_flush(id);
    if (rc != 0) {
        printf("Error flushing device.\n");
        return;
    }

	Msg("Added freespace device %d to index %d\n", id, _deviceCount);

    _threadState.deviceIds[_deviceCount] = id;
    _threadState.deviceAngles[_deviceCount].Init();
        
    struct freespace_message message;
    memset(&message, 0, sizeof(message));
	message.messageType = FREESPACE_MESSAGE_DATAMODECONTROLV2REQUEST;
	message.dataModeControlV2Request.packetSelect = 2;
	message.dataModeControlV2Request.modeAndStatus |= 0 << 1;
    
    if (freespace_sendMessage(id, &message) != FREESPACE_SUCCESS) {
        printf("Could not send message: %d.\n", rc);
    }

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
        freespace_flush(id);
	}

    Msg("%d> Cleaning up...\n", id);
    freespace_closeDevice(id);
	_deviceCount--;
}

static void hotplug_Callback(enum freespace_hotplugEvent event, FreespaceDeviceId id, void* params) {
	MotionSensor* sensor = (MotionSensor*) params;

	if (event == FREESPACE_HOTPLUG_REMOVAL) {
        Msg("Closing removed freespace device %d\n", id);
        sensor->_removeDevice(id);
    } else if (event == FREESPACE_HOTPLUG_INSERTION) {
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
	for (int i = 0; i<MAX_SENSORS; i++){
		lastseq[i] = 0;
	}

	int i = 0;
	while (!state->quit)
	{

		i = (i+1) % MAX_SENSORS;
		id = state->deviceIds[i];
		
		if (id < 0)
			continue;

		rc = freespace_readMessage(id, &message, 5);
		if (rc == FREESPACE_ERROR_TIMEOUT || rc == FREESPACE_ERROR_INTERRUPTED) {
			continue;
		}

		if (message.messageType == FREESPACE_MESSAGE_BODYFRAME && message.bodyFrame.sequenceNumber != lastseq[i]) {

			lastseq[i] = message.bodyFrame.sequenceNumber;

			MahonyAHRSupdateIMU(
				message.bodyFrame.angularVelX / 1000.0f, 
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

			state->deviceAngles[i][ROLL]  = roll;
			state->deviceAngles[i][PITCH] = pitch;
			state->deviceAngles[i][YAW]   = yaw;
		}
	}

	state->isDone = true;

	return 0;
}
