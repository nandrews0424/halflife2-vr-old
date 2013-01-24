#include "vr/imotionsensor.h"

struct InputThreadState 
{
	ThreadHandle_t handle;
	QAngle angle;
	FreespaceDeviceId deviceId;
	bool quit;
	bool isDone;
};

class MotionSensor {

public:
	
	MotionSensor();
	~MotionSensor();
 
	QAngle	getOrientation( void );
	bool	initialized();
	void	update() {};
 	bool	hasOrientation();

protected:
	void	Init(int deviceNumber);
	struct	InputThreadState* _threadState;
	bool	_initialized;
};