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
#ifndef IMOVEMENT_CONTROLLER_H
#define IMOVEMENT_CONTROLLER_H
DeclareInterface(IMovementController)
	int	getOrientation(float &pitch, float &yaw, float &roll);
	int	getPosition(float &x, float &y, float &z);
 	bool isTrackerInitialized();
 	void update();
 	bool hasPositionTracking();
 	bool	hasOrientationTracking();
	void	setOrientationAxis(int pitch, int roll, int yaw);
	void	setRollEnabled(bool enabled);

EndInterface(IMovementController)
#endif //IMOVEMENT_CONTROLLER_H