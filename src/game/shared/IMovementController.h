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
	/**
	*  Returns the orientation from the tracker.  Assumes angles are relative to a right handed coord system with +Z up.
	*  Assumes update() has been called.
	*/
	int	getOrientation(float &pitch, float &yaw, float &roll);
 
	/**
	*  Returns the position from the tracker.  Assumes coordinates are relative to a right handed coord system with +Z up.
	*  Assumes update() has been called.
	*/
	int	getPosition(float &x, float &y, float &z);
 
    /**
    * Returns true if the tracker is initialized and ready to track
    */
	bool isTrackerInitialized();
 
	/**
	*  Reads the hardware and updates local internal state variables for later read by accessors.
	*/
	void update();
 
    /**
    *  Returns true if the tracker has good position info
    */
	bool hasPositionTracking();
 
    /**
    *  Returns true if the tracker has good/reliable orientation info
    */
	bool	hasOrientationTracking();
EndInterface(IMovementController)
#endif //IMOVEMENT_CONTROLLER_H