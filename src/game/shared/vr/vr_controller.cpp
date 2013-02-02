#include "cbase.h"
#include "vr/vr_controller.h"


VrController* _vrController;

VrController::VrController()
{
	Msg("Initializing VR Controller");
	
	for (int i=0; i<SENSOR_COUNT; i++)
	{
		_previousYaw[i] = 0;
		_totalAccumulatedYaw[i] = 0;
	}
	
	_headAngle.Init();
	_headCalibration.Init();
	
	_bodyAngle.Init();

	_weaponAngle.Init();
	_weaponCalibration.Init();

	_freespace = new MotionSensor();
	_vrController = this;
	_initialized = true;
	
	Msg("VR Controller initialized");
}


VrController::~VrController()
{
	delete _freespace;
}

QAngle	VrController::headOrientation( void )
{
	return _headAngle;
}

QAngle	VrController::weaponOrientation( void )
{
	return _weaponAngle;
}

QAngle	VrController::bodyOrientation( void )
{
	return _bodyAngle;
}

bool VrController::hasWeaponTracking( void ) 
{
	return _initialized && _freespace->deviceCount() >= 2;
}

void	VrController::update(float previousViewYaw)
{
	if (!_freespace->initialized()) {
		Msg("Trackers not initialized properly, nothing to do here...\n");
		return;
	}

	if (_updateCounter++ % 240 == 0) {
		freespace_perform();
	}

	// BODY ORIENTATION
	_bodyAngle[YAW] = previousViewYaw - _totalAccumulatedYaw[HEAD];
	
	// HEAD ORIENTATION
	_freespace->getOrientation(0, _headAngle);
	
	float previousYaw = _previousYaw[HEAD];
	float currentYaw = _headAngle[YAW];
	float deltaYaw = currentYaw - previousYaw;
	
	_headAngle[YAW] = deltaYaw + previousViewYaw;
	_previousYaw[HEAD] = currentYaw; 
	_totalAccumulatedYaw[HEAD] += deltaYaw;

	_headAngle -= _headCalibration;
	
	// WEAPON ORIENTATION
	
	if (!hasWeaponTracking()) 
	{
		VectorCopy(_headAngle, _weaponAngle);
		return;		 
	}

	_freespace->getOrientation(WEAPON, _weaponAngle);
				
	previousYaw = _previousYaw[WEAPON];
	currentYaw = _weaponAngle[YAW];
	deltaYaw = currentYaw - previousYaw;

	_previousYaw[WEAPON] = currentYaw;
	_totalAccumulatedYaw[WEAPON] += deltaYaw;
	_weaponAngle[YAW] = previousViewYaw + _totalAccumulatedYaw[WEAPON] - _totalAccumulatedYaw[HEAD];

	_weaponAngle -= _weaponCalibration;
};

void VrController::calibrate()
{
	VectorCopy(_headAngle + _headCalibration, _headCalibration);
	_headCalibration[YAW] = 0;

	if ( hasWeaponTracking() ) 
	{
		VectorCopy(_weaponAngle + _weaponCalibration, _weaponCalibration);
		calibrateWeapon();
	}

	
}

// This only calibrates the yaw
void VrController::calibrateWeapon() {
	
	Msg("Calibrating weapon: \n");
	
	QAngle head, weapon;
	_freespace->getOrientation(0, head);
	_freespace->getOrientation(1, weapon);

	_weaponCalibration[YAW] = weapon[YAW] - head[YAW];
}

void VrController::shutDown()
{
	_initialized = false;
	delete _freespace;
}

extern VrController* VR_Controller()
{
	return _vrController;
}