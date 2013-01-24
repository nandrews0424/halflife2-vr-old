#include <stdio.h>
#include <math.h>
#include "freespace.h"
#include "vr/motionsensor.h"


/*	===================
	VR Controller - Handles coordination of all the raw sensor data, syncing across them and turning them into usable game inputs
	=================== */

#define SENSOR_COUNT 1

enum VrTrackedPart
{
	HEAD = 0,
	WEAPON = 1,
	BODY = 2
};

class VrController 
{
 
public:
 
	VrController();
	~VrController();
 
	QAngle  headOrientation( void );
	QAngle  weaponOrientation( void );
	QAngle  bodyOrientation( void );
	void	calibrate( void );
 	void	update(void);
	bool	initialized() { return _initialized; }
	
protected:

	MotionSensor _sensors[SENSOR_COUNT];
	QAngle _cachedAngles[SENSOR_COUNT];
	float _totalAccumulatedYaw[SENSOR_COUNT];
	float _previousYaw[SENSOR_COUNT];
	bool _initialized;
	
	// WTF @ this seems to pick up the item above it...
	QAngle _calibrationAngles[SENSOR_COUNT];
	
};

VrController* VR_Controller();
