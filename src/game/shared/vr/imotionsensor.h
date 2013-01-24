//Include interface enforement directives
#include "CPPInterfaces2.h"
 
/****************************************************
*  This interface is used to integrate all 
*  Valve movement controllers
*
*
*  Each movement controller provides 6-DOF on
*  a particular object (body part, other tracked object
*
*  This interface assumes that any implementing class
*  will perform any required post processing to transform
*  tracking results into a right handed coordinate system
*  with +Z up -- with units in inches.
*
*****************************************************/
#ifndef IMOTION_SENSOR_H
#define IMOTION_SENSOR_H
DeclareInterface(IMotionSensor)
	QAngle	getOrientation();
	bool	initialized();
 	void	update();
 	bool	hasOrientation();
EndInterface(IMotionSensor)
#endif //IMOVEMENT_CONTROLLER_H