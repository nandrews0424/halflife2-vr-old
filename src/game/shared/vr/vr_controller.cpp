#include "cbase.h"
#include "vr/vr_controller.h"

ConVar vr_neck_length( "vr_neck_length", "12", 0 );
ConVar vr_spine_length( "vr_spine_length", "50", 0 );
ConVar vr_swap_trackers( "vr_swap_trackers", "0", 0 );

VrController* _vrController;


#define MM_TO_UNITS(x) (x/30.f)

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
	_weaponOffsetCalibration.Init();

	_vrIO = _vrio_getInProcessClient();
	_vrIO->initialize();

	_vrController = this;

	if ( _vrIO->getChannelCount() > 0 )
	{
		Msg("VR Controller intialized with %i devices...  \n", _vrIO->getChannelCount());
	}
	else 
	{
		Msg("VR Controller initialized with no devices");
	}

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



bool	VrController::hydraConnected( void )
{
	return _vrIO->hydraConnected();
}

void	VrController::hydraRight(HydraControllerData &data)
{
	Hydra_Message m;
	
	_vrIO->hydraData(m);
	
	data.init();
	data.angle.Init(m.anglesRight[0], m.anglesRight[1], m.anglesRight[2]);
	data.pos.Init(m.posRight[0], m.posRight[1], m.posRight[2]);
	data.xAxis = m.rightJoyX;
	data.yAxis = m.rightJoyY;
}

void	VrController::hydraLeft(HydraControllerData &data)
{
	Hydra_Message m;
	
	_vrIO->hydraData(m);
	
	data.init();
	data.angle.Init(m.anglesLeft[0], m.anglesLeft[1], m.anglesLeft[2]);
	data.pos.Init(m.posLeft[0], m.posLeft[1], m.posLeft[2]);
	data.xAxis = m.leftJoyX;
	data.yAxis = m.leftJoyY;
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
	

	VRIO_Channel headChannel = HEAD;
	VRIO_Channel weaponChannel = WEAPON;
	if ( vr_swap_trackers.GetBool() && _vrIO->getChannelCount() == 2 )
	{
		headChannel = WEAPON;
		weaponChannel = HEAD;
	}
	
	// HEAD ORIENTATION
	
	_vrIO->getOrientation(headChannel, message);
		
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

	_vrIO->getOrientation(weaponChannel, message);
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

	// Msg("Weapon angle %.1f %.1f %.1f\n", _weaponAngle.x, _weaponAngle.y, _weaponAngle.z);

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
	
	if ( !vr_swap_trackers.GetBool() )
	{
		_vrIO->getOrientation(HEAD, head);
		_vrIO->getOrientation(WEAPON, weapon);
	}
	else 
	{
		_vrIO->getOrientation(WEAPON, head);
		_vrIO->getOrientation(HEAD, weapon);
	}

	
	_weaponCalibration[YAW] = weapon.yaw- head.yaw;
	_bodyCalibration[YAW] = -_totalAccumulatedYaw[HEAD];
	
	Vector weapOffset;
	getWeaponOffset(weapOffset);
	_weaponOffsetCalibration += weapOffset;
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

void VrController::getWeaponOffset(Vector &offset)
{
	offset.Init();
	if ( hydraConnected() )
	{
		HydraControllerData data;
		hydraRight(data);

		offset.y = MM_TO_UNITS(data.pos.x);
		offset.z = MM_TO_UNITS(data.pos.y);
		offset.x = -MM_TO_UNITS(data.pos.z);
			
		offset -= _weaponOffsetCalibration;

		// TODO: add calibration angles etc...
	}
} 

