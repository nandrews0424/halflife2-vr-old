#include "IMovementController.h" // The movement control interface
#include <stdio.h>
#include <math.h>
#include "freespace.h"
#include "math/quaternion.h"
#include "math/vec3.h"

class FreespaceMovementController : implements IMovementController {
 
public:
 
	FreespaceMovementController();
 	~FreespaceMovementController();
 
 	int		getOrientation(float &pitch, float &yaw, float &roll);
 	int		getPosition(float &x, float &y, float &z);
 	bool	hasPositionTracking(void);
 	bool	hasOrientationTracking(void);
 	bool	isTrackerInitialized() { return _intialized; }
 
	/**
	* Tells the tracker that its time/safe to update.
	*/
	void	update(void);

protected: 
	bool _intialized;
	FreespaceDeviceId _device;
	struct freespace_UserFrame _userFrame;  // conveys device position and orientation in quaernion
	struct Vec3f _angle;
	CUtlVector<QAngle> _recentAngles;
};