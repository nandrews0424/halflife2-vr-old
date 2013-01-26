#include <stdio.h>
#include <math.h>
#include "freespace.h"
#include "vr/motionsensor.h"


/*	===================
	VR Controller - Handles coordination of all the raw sensor data, syncing across them and turning them into usable game inputs
	=================== */

#define SENSOR_COUNT 3

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
	void	update( float originalYaw );
	void	calibrate( void );
	void	calibrateWeapon( void );

	bool	initialized() { return _initialized; }
	bool	hasWeaponTracking();
	void	shutDown();
	
protected:
	float _totalAccumulatedYaw[SENSOR_COUNT];

	float _fake123124[10];
	float _previousYaw[SENSOR_COUNT];
	float _fak12e[10];
	bool _initialized;
	
	MotionSensor* _freespace;
	float _fake3[10];
	QAngle _headAngle;
	QAngle _headCalibration;
	QAngle _weaponAngle;
	QAngle _weaponCalibration;
	
	float _fake[10];
	
	float _fake2[10];
	unsigned int _updateCounter;
};

VrController* VR_Controller();
