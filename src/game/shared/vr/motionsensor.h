#include "vr/imotionsensor.h"
#include "vr/sensor_fusion.h"

#define MAX_SENSORS 2

struct InputThreadState 
{
	ThreadHandle_t handle;
	QAngle deviceAngles;
	FreespaceDeviceId deviceId;
	int sampleCount;
	int errorCount;
	int lastReturnCode;
	uint16_t lastseq;
	
	float pitch;
	float roll;
	float yaw;

	SensorFusion sensorFusion;

	bool quit;

	void Init()
	{
		quit = false;
		sampleCount = 0;
		errorCount = 0;
		lastReturnCode = 0;
		deviceId = -1;
		deviceAngles.Init();
		pitch = 0;
		roll = 0;
		yaw = 0;
	}

	void Read()
	{
		freespace_message message;

		lastReturnCode = freespace_readMessage(deviceId, &message, 5);
		if (lastReturnCode == FREESPACE_ERROR_TIMEOUT || lastReturnCode == FREESPACE_ERROR_INTERRUPTED) {
			errorCount++;
			return;
		}

		if (message.messageType == FREESPACE_MESSAGE_BODYFRAME && message.bodyFrame.sequenceNumber != lastseq) {

			lastseq = message.bodyFrame.sequenceNumber;

			sensorFusion.MahonyAHRSupdateIMU(
				message.bodyFrame.angularVelX / 1000.0f, 
				message.bodyFrame.angularVelY / 1000.0f,
				message.bodyFrame.angularVelZ / 1000.0f,
				message.bodyFrame.linearAccelX ,
				message.bodyFrame.linearAccelY ,
				message.bodyFrame.linearAccelZ );

			sensorFusion.ReadAngles(deviceAngles);
			sampleCount++;
		}	
	}
};

class MotionSensor {

public:
	
	MotionSensor();
	~MotionSensor();
 
	void	getOrientation( int deviceIndex, QAngle &angle );
	bool	initialized();
	void	update() {};
 	bool	hasOrientation();
 	int 	deviceCount() { return _deviceCount; }
	
	// These ideally would be protected
	void	_initDevice(FreespaceDeviceId id);
	void	_removeDevice(FreespaceDeviceId id);


protected:
	struct	InputThreadState _threadState[MAX_SENSORS];
	bool	_initialized;
	int 	_deviceCount;
};
