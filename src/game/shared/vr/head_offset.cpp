#include "cbase.h"
#include "viewrender.h"
#include "vr_controller.h"

void CViewRender::ApplyHeadOffset(CViewSetup *view)
{
	view->origin += VR_Controller()->getHeadOffset();
}