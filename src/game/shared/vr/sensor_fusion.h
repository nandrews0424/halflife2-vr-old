//=====================================================================================================
// MahonyAHRS.h
//=====================================================================================================
//
// Madgwick's implementation of Mayhony's AHRS algorithm.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author			Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
//=====================================================================================================


#ifndef MahonyAHRS_h
#define MahonyAHRS_h

class SensorFusion {

public:
	
	SensorFusion( void );
	void MahonyAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
	void MahonyAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az);
	Quaternion& Read( void );
	
protected:

	Quaternion q;
	volatile float
		twoKp, 
		twoKi, 
		integralFBx,
		integralFBy, 
		integralFBz;	// integral error terms scaled by Ki
};

#endif
