#include <stdio.h>
#include <math.h>
#include "vr_io.h"

// WinBase.h does so strange defines that break 
#undef CreateEvent
#undef CopyFile

/*	===================
	VR Controller - Handles coordination of all the raw sensor data, syncing across them and turning them into usable game inputs
	=================== */

#define SENSOR_COUNT 4

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
	
	bool	initialized( void );
	bool	hasWeaponTracking( void );
	void	shutDown( void );
	
protected:
	float _totalAccumulatedYaw[SENSOR_COUNT];

	float _fake123124[10];
	float _previousYaw[SENSOR_COUNT];
	float _fak12e[10];
	bool _initialized;
	
	IVRIOClient* _vrIO;
	float _fake3[10];
	QAngle _headAngle;
	QAngle _headCalibration;
	QAngle _weaponAngle;
	QAngle _weaponCalibration;
	QAngle _bodyCalibration;
	QAngle _bodyAngle;
	
	float _fake[10];
	
	float _fake2[10];
	unsigned int _updateCounter;
};

VrController* VR_Controller();
