#include "cbase.h"
#include "vr/vr_controller.h"

VrController* _vrController;

VrController::VrController()
{
	Msg("Initializing VR Controller");

	for (int i=0; i<SENSOR_COUNT; i++)
	{
		_sensors[i] = new MotionSensor(i+1);
		_previousYaw[i] = 0;
		_totalAccumulatedYaw[i] = 0;
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
	_cachedAngles[HEAD] = _sensors[HEAD]->getOrientation();
	
	float previousYaw = _previousYaw[HEAD];
	float currentYaw = _cachedAngles[HEAD][YAW];
	float deltaYaw = currentYaw - previousYaw;
	
	_cachedAngles[HEAD][YAW] = deltaYaw;
	_previousYaw[HEAD] = currentYaw;
	_totalAccumulatedYaw[HEAD] += deltaYaw;

	// END HEAD ORIENTATION
	Msg("Yaw change %f prev %f total %\n", deltaYaw, previousYaw, _totalAccumulatedYaw[HEAD]);
	
	//todo: weapon orientation will be relative to either head or the "baseline"
	VectorCopy(_cachedAngles[HEAD], _cachedAngles[HEAD]);
};

extern VrController* VR_Controller()
{
	return _vrController;
}
