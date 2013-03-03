#include "cbase.h"
#include "viewrender.h"
#include "vr_controller.h"

void CViewRender::ApplyHeadOffset(CViewSetup *view)
{
	Vector headOffset(0,0,0);
	VR_Controller()->getHeadOffset(headOffset, false);
	view->origin += headOffset;
}