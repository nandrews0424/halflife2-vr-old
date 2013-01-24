#include "IMovementController.h" // The movement control interface
#include <stdio.h>
#include <math.h>
#include "freespace.h"

#define SMOOTHING_WINDOW_SIZE 10
#define MAX_TRACKERS 3

struct TrackerData {
	QAngle TrackedAngles[SMOOTHING_WINDOW_SIZE];
	QAngle CalAngle;  //calibrated offset angle to match other trackers
	QAngle CurAngle;
	QAngle LastChange;
	int RollAxis;
	int PitchAxis;
	int YawAxis;
	bool invertPitch;
	bool invertRoll;
	bool invertYaw;
	int CurSample;
	int NumSamples;
	bool RollEnabled;
	FreespaceDeviceId id;
	freespace_UserFrame userFrame;
};

class FreespaceMovementController : implements IMovementController {
 
public:
 
	FreespaceMovementController();
	~FreespaceMovementController();
 
 	int		getOrientation(float &pitch, float &yaw, float &roll, int idx);
 	int		getPosition(float &x, float &y, float &z);
 	bool	hasPositionTracking(void);
 	bool	hasOrientationTracking(void);
	bool	isTrackerInitialized() { return _initialized; }
	void	update(void);
	void	setOrientationAxis(int pitch, int roll, int yaw);
	void	setRollEnabled(bool enabled);
	void	calibrate();

protected:
	void InitTrackers();
	void Init(int pitchAxis, int rollAxis, int yawAxis, bool rollEnabled);
	bool _initialized;
	int _numTrackers;
};
