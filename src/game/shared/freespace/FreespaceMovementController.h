#include "IMovementController.h" // The movement control interface
#include <stdio.h>
#include <math.h>
#include "freespace.h"
#include "math/quaternion.h"
#include "math/vec3.h"

#define SMOOTHING_WINDOW_SIZE 3


class FreespaceMovementController : implements IMovementController {
 
public:
 
	FreespaceMovementController();
	~FreespaceMovementController();
 
 	int		getOrientation(float &pitch, float &yaw, float &roll);
 	int		getPosition(float &x, float &y, float &z);
 	bool	hasPositionTracking(void);
 	bool	hasOrientationTracking(void);
 	bool	isTrackerInitialized() { return _intialized; }
	void	update(void);
	void	setOrientationAxis(int pitch, int roll, int yaw);
	void	setRollEnabled(bool enabled);

protected: 
	void Init(int pitchAxis, int rollAxis, int yawAxis, bool rollEnabled);
	bool _intialized;
	FreespaceDeviceId _device;
	QAngle TrackedAngles[SMOOTHING_WINDOW_SIZE];
	QAngle LastChange;
	QAngle CurAngle;
	int		RollAxis;
	int		PitchAxis;
	int		YawAxis;
	bool	RollEnabled;
};

void UTIL_getHeadOrientation(float &pitch, float &yaw, float &roll);