#include "vr/imotionsensor.h"

#DEFINE MAX_SENSORS 2

struct InputThreadState 
{
	ThreadHandle_t handle;
	QAngle deviceAngles[MAX_SENSORS];
	FreespaceDeviceId deviceIds[MAX_SENSORS];
	bool quit;
	bool isDone;
};

class MotionSensor {

public:
	
	MotionSensor();
	~MotionSensor();
 
	QAngle	getOrientation( int deviceIndex );
	bool	initialized();
	void	update() {};
 	bool	hasOrientation();
 	int 	deviceCount() { return _deviceCount; }

protected:
	struct	InputThreadState _threadState;
	bool	_initialized;
	int 	_deviceCount;
};
