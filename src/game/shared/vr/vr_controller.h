#include <stdio.h>
#include <math.h>
#include "..\..\vr_io\vr_io.h" // WTF @ THIS.... it's in the include path, I hate VS

// WinBase.h does so strange defines that break 
#undef CreateEvent
#undef CopyFile

/*	===================
	VR Controller - Handles coordination of all the raw sensor data, syncing across them and turning them into usable game inputs
	=================== */

#define SENSOR_COUNT 4

struct HydraControllerData
{
	QAngle angle;
	Vector pos;

	float xAxis;
	float yAxis;

	void init()
	{
		angle.Init();
		pos.Init();
		xAxis=0;
		yAxis=0;
	}
};

struct HmdInfo
{
       unsigned  HResolution, VResolution;
       float     HScreenSize, VScreenSize;
       float     VScreenCenter;
       float     EyeToScreenDistance;
       float     LensSeparationDistance;
       float     InterpupillaryDistance;
       float     DistortionK[4];
};

class VrController 
{
 
public:
 
	VrController();
	~VrController();
 
	QAngle  headOrientation( void );
	QAngle  weaponOrientation( void );
	QAngle  bodyOrientation( void );
	void	update( float originalYaw );
	void	calibrate( void );
	void	calibrateWeapon( void );

	bool	hydraConnected( void );
	void	hydraRight(HydraControllerData &data);
	void	hydraLeft(HydraControllerData &data);
	
	HmdInfo hmdInfo( void );

	bool	initialized( void );
	bool	hasWeaponTracking( void );
	void	shutDown( void );

	void	getHeadOffset(Vector &headOffset, bool ignoreRoll);
	void	getShootOffset(Vector &shootOffset);
	void	getWeaponOffset(Vector &offset, bool calibrated=true);
				
protected:
	float _totalAccumulatedYaw[SENSOR_COUNT];

	float _previousYaw[SENSOR_COUNT];
	bool _initialized;
	
	IVRIOClient* _vrIO;
	

	QAngle _headAngle;
	QAngle _headCalibration;
	QAngle _weaponAngle;
	QAngle _weaponCalibration;
	QAngle _bodyCalibration;
	QAngle _bodyAngle;
	Vector _weaponOffsetCalibration;
	
	unsigned int _updateCounter;
};

VrController* VR_Controller();
