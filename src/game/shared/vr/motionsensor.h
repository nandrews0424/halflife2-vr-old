#include "vr/sensor_fusion.h"

#define MAX_SENSORS 2

struct InputThreadState 
{
	ThreadHandle_t handle;
	float _fake[10];
	QAngle deviceAngles[MAX_SENSORS];
	float _fake2[10];
	FreespaceDeviceId deviceIds[MAX_SENSORS];
	float _wtf[10];
	int sampleCount[MAX_SENSORS];
	int errorCount[MAX_SENSORS];
	int lastReturnCode[MAX_SENSORS];
	
	float pitch[MAX_SENSORS];
	float roll[MAX_SENSORS];
	float yaw[MAX_SENSORS];

	SensorFusion sensorFusion[MAX_SENSORS];

	bool quit;
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
	struct	InputThreadState _threadState;
	bool	_initialized;
	int 	_deviceCount;
};
