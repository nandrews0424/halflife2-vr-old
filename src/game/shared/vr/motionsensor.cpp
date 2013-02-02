#include "cbase.h"
#include <windows.h>
#include "freespace.h"
#include "vr/motionsensor.h"
#include "vr/sensor_fusion.h"
#include <windows.h>
#include <sys/timeb.h>



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
	// Trying no local variables which seemed to be shared across threads.
	while (!((InputThreadState*)params)->quit)
	{
		((InputThreadState*)params)->Read();	
	}
	return 0;
}

MotionSensor* _freespaceSensor;

MotionSensor::MotionSensor() 
{
	Msg("Initializing freespace drivers\n");

	_deviceCount = 0;
	struct freespace_message message;
	
	if ( freespace_init() != FREESPACE_SUCCESS ) {
		printf("Freespace initialization error...\n");
		return;
	}
	
	freespace_setDeviceHotplugCallback(hotplug_Callback, this);
	freespace_perform();

	_freespaceSensor = this;
	_initialized = true;
	
	Msg("Freespace initialized successfully\n");
}


MotionSensor::~MotionSensor() 
{
	Msg("Shutting downfreespace devices\n");
	for (int idx=0; idx < MAX_SENSORS; idx++) {
		if (_threadState[idx].deviceId >= 0) {
			_removeDevice(_threadState[idx].deviceId);
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
	
	int i;
	for ( i=0; i<MAX_SENSORS; i++ )
	{
		if ( _threadState[i].deviceId = -1 ) {
			break;
		}
	}

	// todo: is this the issue.....
	i = _deviceCount;
	
	_threadState[i].Init();
	_threadState[i].deviceId = id;
	_threadState[i].handle = CreateSimpleThread(MotionSensor_Thread, &_threadState[i]);
	_deviceCount++;
	
	Msg("Freespace sensor %i initialized (id: %i)", _deviceCount, id);
}

void MotionSensor::_removeDevice(FreespaceDeviceId id) {
    struct freespace_message message;
    int rc;
    int i;
	
    // Shut down the thread and remove it from the list of active trackers...
    for (int i = 0; i < MAX_SENSORS; i++) {
        if (_threadState[i].deviceId == id) {
            
			_threadState[i].quit = true;

			if (ThreadJoin(_threadState[i].handle, 200)) {
				Msg("Freespace input thread shut down successfully...\n");
			} else {
				Msg("Freespace input thread join timed out, releasing thread handle...\n");
				ReleaseThreadHandle(_threadState[i].handle);
			}

			_threadState[i].Init();
			
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
	
	if (_threadState[deviceIndex].deviceId == -1) {
		angle.Init();
		return;
	}

	angle[PITCH] = _threadState[deviceIndex].pitch;
	angle[ROLL] = _threadState[deviceIndex].roll;
	angle[YAW] = _threadState[deviceIndex].yaw;
}

bool MotionSensor::initialized()
{
	return _initialized;
}

bool MotionSensor::hasOrientation()
{
	return true;
}
