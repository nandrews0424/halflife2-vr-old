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
	_bodyCalibration.Init();
	_bodyCalibration[YAW] = -1; // triggers first pass initialization

	_weaponAngle.Init();
	_weaponCalibration.Init();

	_vrIO = _vrio_getInProcessClient();
	_vrIO->initialize();

	_vrController = this;
	_initialized = true;
	
	Msg("VR Controller initialized");
}


VrController::~VrController()
{
	delete _vrIO;
}

bool	VrController::initialized( void )
{
	return _initialized; //TODO: need deviceCount && _vrIO->deviceCount() > 0; //for now
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

bool VrController::hasWeaponTracking( void ) //TODO: should be orientation
{
	return _initialized; // && _vrIO->channelHasOrientation(HEAD);
}

void	VrController::update(float previousViewYaw)
{
	if (!false) { //todo: check vrIO state
		Msg("Trackers not initialized properly, nothing to do here...\n");
		return;
	}
	
	VRIO_Message message;
	_vrIO->think();
	

	// HEAD ORIENTATION

	_vrIO->getOrientation(HEAD, message);
	_headAngle[PITCH] = message.pitch;
	_headAngle[ROLL] = message.roll;
	_headAngle[YAW] = message.yaw;
	
	float previousYaw = _previousYaw[HEAD];
	float currentYaw = _headAngle[YAW];
	float deltaYaw = currentYaw - previousYaw;
	
	_headAngle[YAW] = deltaYaw + previousViewYaw;
	_previousYaw[HEAD] = currentYaw; 
	_totalAccumulatedYaw[HEAD] += deltaYaw;
		
	_headAngle -= _headCalibration;
	


	// BODY ORIENTATION
	VectorCopy(_headAngle, _bodyAngle);

	_bodyAngle[YAW] = previousViewYaw - _totalAccumulatedYaw[HEAD];  // this gives us the global original value
	
	// if uninitialized, set initial body calibration
	if (_bodyCalibration[YAW] < 0) {
		_bodyCalibration[YAW] = -_totalAccumulatedYaw[HEAD];
	}
	
	_bodyAngle -= _bodyCalibration;
	


	// WEAPON ORIENTATION

	if (!hasWeaponTracking()) 
	{
		VectorCopy(_headAngle, _weaponAngle);
		return;		 
	}

	_vrIO->getOrientation(WEAPON, message);
	_weaponAngle[PITCH] = message.pitch;
	_weaponAngle[ROLL] = message.roll;
	_weaponAngle[YAW] = message.yaw;
				
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

	VectorCopy(_weaponAngle + _weaponCalibration, _weaponCalibration);
	calibrateWeapon();
}

// This recenters all elements
void VrController::calibrateWeapon() {
	
	Msg("Calibrating weapon: \n");
	
	VRIO_Message head, weapon;
	_vrIO->getOrientation(HEAD, head);
	_vrIO->getOrientation(WEAPON, weapon);

	_weaponCalibration[YAW] = weapon.yaw- head.yaw;
	_bodyCalibration[YAW] = -_totalAccumulatedYaw[HEAD];
}

void VrController::shutDown()
{
	_initialized = false;
	delete _vrIO;
}

extern VrController* VR_Controller()
{
	return _vrController;
}