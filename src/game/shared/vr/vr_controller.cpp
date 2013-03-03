#include "cbase.h"
#include "vr/vr_controller.h"

ConVar vr_neck_length( "vr_neck_length", "12", 0 );
ConVar vr_spine_length( "vr_spine_length", "50", 0 );

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
	shutDown();
}

bool	VrController::initialized( void )
{
	return _initialized && _vrIO->getChannelCount() > 0; //for now
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
	return _initialized && _vrIO->getChannelCount() > 1;  // TODO: && _vrIO->channelHasOrientation(HEAD);
}

void	VrController::update(float previousViewYaw)
{
	if (_vrIO->getChannelCount() == 0) { //todo: check vrIO state
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
	
	if (_initialized)
	{
		Msg("Shutting down VR Controller");
		_initialized = false;
		_vrIO->dispose();
	}
	else 
	{
		Msg("VR Controller already shut down, nothing to do here.");
	}
	// delete _vrIO; 
}

extern VrController* VR_Controller()
{
	return _vrController;
}


Vector VrController::getHeadOffset()
{
	Vector position;
	float neckLength = vr_neck_length.GetFloat();
	position.z -= neckLength;
	
	QAngle headAngle = _vrController->headOrientation();
	Vector up;
	AngleVectors(headAngle, NULL, NULL, &up);
	up *= neckLength;
	position += up;
	
	if (false) {
		QAngle spineAngle = _vrController->headOrientation();
		up;
		AngleVectors(spineAngle, NULL, NULL, &up);
	
		// TODO: Apply the same technique
	}

	// TODO: collision detection necessary with larger sizes
	// Msg("getHeadOffset() position %f %f %f", position.x, position.y, position.z);

	return position;
}

// TODO: more we can do here....
Vector VrController::getShootOffset()
{
	return getHeadOffset();
}

void getViewModelAngles()
{
	// View model rotations often need to be adjusted but may be fixed by proper origin adjustments...
}

// Viewmodels aren't properly origined and thus a translation needs to be calculated to adjust for the rotation about an invalid origin
Vector VrController::calculateViewModelRotationTranslation(Vector desiredRotationOrigin)
{
	QAngle angles = _vrController->weaponOrientation() - _vrController->headOrientation();
	angles.x*=-1;
	
	Msg("Calculating viewmodel translation from angles %f %f %f\n", angles.x, angles.y, angles.z);
	Vector moved;
	matrix3x4_t rotateMatrix;
	AngleMatrix(angles, rotateMatrix);
	VectorRotate( desiredRotationOrigin, rotateMatrix, moved);

	return moved - desiredRotationOrigin;
}