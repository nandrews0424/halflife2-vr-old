#include "cbase.h"
#include "vr/vr_controller.h"

VrController* _vrController;

VrController::VrController()
{
	Msg("Initializing VR Controller");
	
	for (int i=0; i<SENSOR_COUNT; i++)
	{
		_sensors[i] = new MotionSensor();
		_previousYaw[i] = 0;
		_totalAccumulatedYaw[i] = 0;
		_angles[i].Init();
		_calibrationAngles[i].Init();
	}
	
	_vrController = this;
	_initialized = true;
	
	Msg("VR Controller initialized");
}


VrController::~VrController()
{
	for (int i=0; i<SENSOR_COUNT; i++){
		delete _sensors[i];
	}
}

QAngle	VrController::headOrientation( void )
{
	return _angles[HEAD];
}

QAngle	VrController::weaponOrientation( void )
{
	return _angles[WEAPON];
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
	return _initialized && _sensors[0]->deviceCount() >= 2;
}

void	VrController::update()
{
	_updateCount++;
	
	if (!_sensors[HEAD]->initialized()) {
		Msg("HEAD Sensor not initialized properly, nothing to do here...\n");
		return;
	}

	// HEAD ORIENTATION
	_angles[HEAD] = _sensors[HEAD]->getOrientation(0);
	
	float previousYaw = _previousYaw[HEAD];
	float currentYaw = _angles[HEAD][YAW];
	float deltaYaw = currentYaw - previousYaw;
	
	_angles[HEAD][YAW] = deltaYaw;
	_previousYaw[HEAD] = currentYaw; 
	_totalAccumulatedYaw[HEAD] += deltaYaw;

	_angles[HEAD] -= _calibrationAngles[HEAD];
	
	// END HEAD ORIENTATION

	
	// WEAPON ORIENTATION
	if (_sensors[HEAD]->deviceCount() <= 1) 
	{
		VectorCopy(_angles[HEAD], _angles[WEAPON]);
		return;		 
	}

	_angles[WEAPON] = _sensors[HEAD]->getOrientation(1);
	
	previousYaw = _previousYaw[WEAPON];
	currentYaw = _angles[WEAPON][YAW];
	deltaYaw = currentYaw - previousYaw;

	_angles[WEAPON][YAW] = deltaYaw;
	_previousYaw[WEAPON] = currentYaw;
	_totalAccumulatedYaw[WEAPON];

	Msg("Weapon Angles p: %i r: %i y: %i\n", _angles[WEAPON][PITCH],_angles[WEAPON][ROLL],_angles[WEAPON][YAW]);
	
	_angles[WEAPON] -= _calibrationAngles[WEAPON];
	
	//END WEAPON ORIENTATION

	//temp override ....
	VectorCopy(_angles[HEAD], _angles[WEAPON]);


	if (_updateCount % 120 == 0) {
		//occasionally we need to call this to pick up any new or dropped devices...
		//freespace_perform();
	}


};

void VrController::calibrate()
{
	for (int i=0; i<SENSOR_COUNT; i++) {
		//add back in current calibration angles as they've already been factored out of the cached angles
		VectorCopy(_angles[i] + _calibrationAngles[i], _calibrationAngles[i]);
		_calibrationAngles[i][YAW] = 0;

		if (i == WEAPON) {
			_calibrationAngles[WEAPON][YAW] +=  (_angles[HEAD][YAW] - _angles[WEAPON][YAW]); // Whatever it takes to set the weapon angle equal to the head angle...
		}

	}
}

void VrController::shutDown()
{
	_initialized = false;
	delete _sensors[0];
}



extern VrController* VR_Controller()
{
	return _vrController;
}
