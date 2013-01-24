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
	
	_vrController = this;
	_initialized = true;
	
	Msg("VR Controller initialized");
}


VrController::~VrController()
{
}

QAngle	VrController::headOrientation( void )
{
	return _cachedAngles[HEAD];
}

QAngle	VrController::weaponOrientation( void )
{
	return _cachedAngles[WEAPON];
}

QAngle	VrController::bodyOrientation( void )
{
	QAngle head = headOrientation();
	QAngle body;
	
	//for now just match the YAW exactly
	body[YAW] = head[YAW]; 
	return body;
}


void	VrController::update()
{
	if (!_sensors[HEAD].initialized()) {
		Msg("HEAD Sensor not initialized properly, nothing to do here...\n");
		return;
	}

	// HEAD ORIENTATION
	_cachedAngles[HEAD] = _sensors[HEAD].getOrientation();
	
	float previousYaw = _previousYaw[HEAD];
	float currentYaw = _cachedAngles[HEAD][YAW];
	float deltaYaw = currentYaw - previousYaw;
	
	_previousYaw[HEAD] = currentYaw;
	_cachedAngles[HEAD][YAW] = deltaYaw;
	_totalAccumulatedYaw[HEAD] += deltaYaw;

	Msg("Subtracting calibration angles pitch: %f roll: %f yaw: %f\n", _calibrationAngles[HEAD][PITCH], _calibrationAngles[HEAD][ROLL], _calibrationAngles[HEAD][YAW]);
	// _cachedAngles[HEAD] -= _calibrationAngles[HEAD];

	// END HEAD ORIENTATION


	//todo: weapon orientation will be relative to either head or the "baseline"
	VectorCopy(_cachedAngles[HEAD], _cachedAngles[WEAPON]);
};


void	VrController::calibrate()
{
	for (int i = 0; i < SENSOR_COUNT; i++)
		VectorCopy(_cachedAngles[i], _calibrationAngles[i]);
}

extern VrController* VR_Controller()
{
	return _vrController;
}
