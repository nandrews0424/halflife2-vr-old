#include "IMovementController.h" // The movement control interface
#include <stdio.h>
#include <math.h>
#include "freespace.h"

#define SMOOTHING_WINDOW_SIZE 3

struct InputThreadState {
	ThreadHandle_t _handle;
	bool _quit;
	bool _initialized;
	FreespaceDeviceId deviceId;
	freespace_UserFrame userFrame;
};


class FreespaceMovementController : implements IMovementController {
 
public:
 
	FreespaceMovementController();
	~FreespaceMovementController();
 
 	int		getOrientation(float &pitch, float &yaw, float &roll);
 	int		getPosition(float &x, float &y, float &z);
 	bool	hasPositionTracking(void);
 	bool	hasOrientationTracking(void);
	bool	isTrackerInitialized() { return _state->_initialized; }
	void	update(void);
	void	setOrientationAxis(int pitch, int roll, int yaw);
	void	setRollEnabled(bool enabled);

protected: 
	void Init(int pitchAxis, int rollAxis, int yawAxis, bool rollEnabled);
	struct InputThreadState* _state;
	QAngle TrackedAngles[SMOOTHING_WINDOW_SIZE];
	QAngle LastChange;
	QAngle CurAngle;
	int		RollAxis;
	int		PitchAxis;
	int		YawAxis;
	bool	RollEnabled;
};

void UTIL_getHeadOrientation(float &pitch, float &yaw, float &roll);