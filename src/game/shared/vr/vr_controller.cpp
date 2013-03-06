#include "cbase.h"
#include "vr/vr_controller.h"

ConVar vr_neck_length( "vr_neck_length", "10", 0 );
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


void VrController::getHeadOffset(Vector &headOffset, bool ignoreRoll = false)
{
	float neckLength = vr_neck_length.GetFloat();
	headOffset.z -= neckLength;
	
	QAngle headAngle = _vrController->headOrientation();

	if ( ignoreRoll )	{
		headAngle.z = 0;
	}

	Vector up;
	AngleVectors(headAngle, NULL, NULL, &up);
	headOffset += up*neckLength;
	
	// TODO: Apply the same technique to a spine length if tracking body orientation separately...
	if (false) {
		QAngle spineAngle = _vrController->headOrientation();
		up;
		AngleVectors(spineAngle, NULL, NULL, &up);
	}

	// TODO: collision detection necessary with larger sizes
	// Msg("getHeadOffset() position %f %f %f", position.x, position.y, position.z);
}

// TODO: more we can do here....
void VrController::getShootOffset(Vector &shootOffset)
{
	getHeadOffset(shootOffset);
}

void applyGestures()
{
	// todo: a method that looks for special orientation relationships or changes and triggers input buttons...
}

Vector rotateVector(Vector &vector, QAngle &angle)
{
	Vector result(0,0,0);

	angle.x = DEG2RAD(angle.x);
	angle.y = DEG2RAD(angle.y);
	angle.z = DEG2RAD(angle.z);
	
	result.y += vector.y*cos(angle.x) - vector.z*sin(angle.x);
	result.z += vector.y*sin(angle.x) + vector.z*cos(angle.x);
	
	result.x += vector.x*cos(angle.y) + vector.z*sin(angle.y);
	result.z += -vector.x*sin(angle.y) + vector.z*cos(angle.y);
			
	result.x += vector.x*cos(angle.z) - vector.y*sin(angle.z);
	result.y += vector.x*sin(angle.z) + vector.y*cos(angle.z);
	
	return result;
}


// Viewmodels aren't properly origined and thus a translation needs to be calculated to adjust for the rotation about an invalid origin
Vector VrController::calculateViewModelRotationTranslation(Vector desiredRotationOrigin)
{
	QAngle weapon = _vrController->weaponOrientation();
	QAngle head = _vrController->headOrientation();
	QAngle angles = weapon - head;

	if ( angles.z < -180.f )
		angles.z += 360.f;
	if ( angles.z > 180.f )
		angles.z -= 360.f;
	
	Vector offset(0,0,0);
	

	// pitch effects - good enough
	offset.x += -6 * cos(DEG2RAD(angles.x));
	offset.z +=  10.5 * sin(DEG2RAD(angles.x));
		
	//Msg("%.1f deg pitch effects x: %.1f y: %.1f z: %.1f\n", angles.x, offset.x, offset.y, offset.z);

	// yaw effects 
	offset.x += 2 * sin(DEG2RAD(angles.y));
	offset.y += 15.5 * sin(DEG2RAD(angles.y));

	if (angles.y < 0) {
		// Msg("%.1f x axis boost per negative yaw\n", -8 * sin(DEG2RAD(angles.y)));
		offset.x -= 9.5 * sin(DEG2RAD(angles.y));
	}
	
	//Msg("+ %.1f yaw pitch effects x: %.1f y: %.1f z: %.1f\n", angles.y, offset.x, offset.y, offset.z);

	// roll effects
	offset.y +=	 8 * sin(DEG2RAD(angles.z));  // side to side is fine...

	if (angles.z < 0) {
		offset.y += -3 * sin(DEG2RAD(angles.z)); 
		offset.z += 5 * sin(DEG2RAD(angles.z));
	}

	
	
	return offset;
}




