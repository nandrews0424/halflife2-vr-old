#include "cbase.h"
#include "viewrender.h"

void CViewRender::ApplyHeadOffset(CViewSetup *view)
{
	Vector offset = view->origin;

	//reset these as we'll recalculate them 
	//this will remove some bob effects but I think we want that anyway
	//otherwise use prev angle calculation to undo previous neck model offset
	float neckLength = 12;
	offset.z -= neckLength;
	QAngle angles = view->angles;
	Vector up;
	AngleVectors(angles, NULL, NULL, &up);
	up *= neckLength;
	view->origin = offset + up;

}