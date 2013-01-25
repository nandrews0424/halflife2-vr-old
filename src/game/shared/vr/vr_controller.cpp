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
		_cachedAngles[i].Init();
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
	if (!_sensors[HEAD]->initialized()) {
		Msg("HEAD Sensor not initialized properly, nothing to do here...\n");
		return;
	}

	// HEAD ORIENTATION
	_cachedAngles[HEAD] = _sensors[HEAD]->getOrientation(0);
	
	float previousYaw = _previousYaw[HEAD];
	float currentYaw = _cachedAngles[HEAD][YAW];
	float deltaYaw = currentYaw - previousYaw;
	
	_cachedAngles[HEAD][YAW] = deltaYaw;
	_previousYaw[HEAD] = currentYaw; 
	_totalAccumulatedYaw[HEAD] += deltaYaw;

	_cachedAngles[HEAD] -= _calibrationAngles[HEAD];

	// END HEAD ORIENTATION
	//Msg("Yaw change %f prev %f total %\n", deltaYaw, previousYaw, _totalAccumulatedYaw[HEAD]);
	
	Msg("Calibration Angle %f %f %f", _calibrationAngles[HEAD][PITCH], _calibrationAngles[HEAD][ROLL], _calibrationAngles[HEAD][YAW]);
	
	//todo: weapon orientation will be relative to either head or the "baseline"
	VectorCopy(_cachedAngles[HEAD], _cachedAngles[WEAPON]);
};

void VrController::calibrate()
{
	for (int i=0; i<SENSOR_COUNT; i++) {
		//add back in current calibration angles as they've already been factored out of the cached angles
		VectorCopy(_cachedAngles[i] + _calibrationAngles[i], _calibrationAngles[i]);
		_calibrationAngles[i][YAW] = 0;
	}
}


extern VrController* VR_Controller()
{
	return _vrController;
}
