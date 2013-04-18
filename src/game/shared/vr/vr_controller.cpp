#include "cbase.h"
#include "vr/vr_controller.h"

ConVar vr_neck_length( "vr_neck_length", "4", 0 );
ConVar vr_spine_length( "vr_spine_length", "30", 0 );
ConVar vr_swap_trackers( "vr_swap_trackers", "0", 0 );
ConVar vr_weapon_movement_scale( "vr_weapon_movement_scale", ".75", FCVAR_ARCHIVE, "Scales tracked weapon positional tracking");

VrController* _vrController;

#define MM_TO_INCHES(x) (x/1000.f)*(1/METERS_PER_INCH)

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

// TODO: would be better to expose these not as "hydra" controllers

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

	// without additional tracking it is simply the head angle minus the total accumulated head yaw
	VectorCopy(_headAngle, _bodyAngle);
	_bodyCalibration[YAW] += deltaYaw;  // any head movement is negated from body orientation
	_bodyAngle -= _bodyCalibration;
	_bodyAngle[YAW] = previousViewYaw - _bodyCalibration[YAW];

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

	// zero out the calibrations except yaw, all the trackers I'm 

	if ( hydraConnected() ) {
		_weaponCalibration[PITCH] = 0;
		_weaponCalibration[ROLL] = 0;
	}
	
	_weaponCalibration[YAW] = weapon.yaw - head.yaw;
	_bodyCalibration[YAW] = 0;
	
	// todo: if weapon tracking only 

	Vector weapOffset;
	getWeaponOffset(weapOffset, false);

	Msg("Zeroing weapon offsets %.1f %.1f %.1f\n", weapOffset.x, weapOffset.y, weapOffset.z);
	// todo: here we want to do a bit of offset handling for calibrating at the shoulder as that seems to be the standard with a hydra
	Vector shoulderCalibrationOffset(-10, -5, 5);
	_weaponOffsetCalibration = weapOffset + shoulderCalibrationOffset;
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
	
	QAngle headAngle = headOrientation();

	if ( ignoreRoll )	{
		headAngle.z = 0;
	}

	Vector up;
	AngleVectors(headAngle, NULL, NULL, &up);
	headOffset += up*neckLength;
	
	// TODO: collision detection necessary with larger sizes
	// Msg("getHeadOffset(%.1f) position %f %f %f\n", neckLength, headOffset.x, headOffset.y, headOffset.z);
}

// TODO: more we can do here....
void VrController::getShootOffset(Vector &shootOffset)
{
	getHeadOffset(shootOffset);
}

void VrController::getWeaponOffset(Vector &offset, bool calibrated)
{
	offset.Init();
	if ( hydraConnected() )
	{
		HydraControllerData data;
		hydraRight(data);

		offset.y = MM_TO_INCHES(data.pos.x);
		offset.z = MM_TO_INCHES(data.pos.y);
		offset.x = -MM_TO_INCHES(data.pos.z);
		
		if (calibrated)
		{
			offset -= _weaponOffsetCalibration; // weapon calibration is stored in hydra-space

			// orient weapon offset with the body as the hydra base stays body relative (but dump any pitch & roll, only care about yaw)
			QAngle body(0, bodyOrientation().y, 0);
			Vector forward, right, up;
			AngleVectors(body, &forward, &right, &up);
			offset = forward*offset.x + right*offset.y + up*offset.z;

			offset *= vr_weapon_movement_scale.GetFloat();

		}
	}
} 

HmdInfo VrController::hmdInfo()
{
       HmdInfo h;

       HMDDeviceInfo info = _vrIO->getHMDInfo();

       for (int i=0;i<4;i++)
               h.DistortionK[i] = info.DistortionK[i];

       h.HResolution = info.HResolution;
       h.HScreenSize = info.HScreenSize;
       h.VResolution = info.VResolution;
       h.VScreenSize = info.VScreenSize;
       h.VScreenCenter = info.VScreenCenter;
       h.InterpupillaryDistance = info.InterpupillaryDistance;
       h.LensSeparationDistance = info.LensSeparationDistance;
       h.EyeToScreenDistance = info.EyeToScreenDistance;

       return h;
}
