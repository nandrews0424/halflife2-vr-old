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
	QAngle head = headOrientation();
	QAngle body;
	
	//for now just match the YAW exactly
	body[YAW] = head[YAW]; 
	return body;
}

bool VrController::hasWeaponTracking( void ) 
{
	return _initialized && _freespace->deviceCount() >= 2;
}

void	VrController::update(float previousViewYaw)
{
	if (!_freespace->initialized()) {
		Msg("HEAD Sensor not initialized properly, nothing to do here...\n");
		return;
	}

	if (_updateCounter++ % 120 == 0) {
		Msg("Calling freespace perform to check for hot loaded devices");
		freespace_perform();
	}


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
	_weaponAngle[YAW] = previousViewYaw + _totalAccumulatedYaw[WEAPON];

	Msg("VR Controller Weapon Angles p: %f r: %f y: %f\n", _weaponAngle[PITCH],_weaponAngle[ROLL],_weaponAngle[YAW]);
	
	_weaponAngle -= _weaponCalibration;
};

void VrController::calibrate()
{
	VectorCopy(_headAngle + _headCalibration, _headCalibration);
	VectorCopy(_weaponAngle + _weaponCalibration, _weaponCalibration);

	// Handle Yaw separately
	/* TODO
	for (int i=0; i<SENSOR_COUNT; i++) {
		//add back in current calibration angles as they've already been factored out of the cached angles
		VectorCopy(_angles[i] + _calibrationAngles[i], _calibrationAngles[i]);
		_calibrationAngles[i][YAW] = 0;

		if (i == WEAPON) {
			_calibrationAngles[WEAPON][YAW] +=  (_angles[HEAD][YAW] - _angles[WEAPON][YAW]); // Whatever it takes to set the weapon angle equal to the head angle...
		}
	}
	*/
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