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
	void	update(void);
	bool	initialized() { return _initialized; }
	
protected:
	float _totalAccumulatedYaw[SENSOR_COUNT];
	float _previousYaw[SENSOR_COUNT];
	bool _initialized;
	
	MotionSensor* _sensors[SENSOR_COUNT];
	QAngle _cachedAngles[SENSOR_COUNT];
	float _fake[100];
	

};

VrController* VR_Controller();
