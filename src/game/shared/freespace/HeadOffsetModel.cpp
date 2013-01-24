
#include "cbase.h"
#include "view.h"
#include "iviewrender.h"
#include "iviewrender_beams.h"
#include "view_shared.h"
#include "ivieweffects.h"
#include "iinput.h"
#include "iclientmode.h"
#include "prediction.h"
#include "viewrender.h"
#include "c_te_legacytempents.h"
#include "cl_mat_stub.h"
#include "tier0/vprof.h"
#include "IClientVehicle.h"
#include "engine/IEngineTrace.h"
#include "mathlib/vmatrix.h"
#include "rendertexture.h"
#include "c_world.h"
#include <KeyValues.h>
#include "igameevents.h"
#include "smoke_fog_overlay.h"
#include "bitmap/tgawriter.h"
#include "hltvcamera.h"
#include "input.h"
#include "filesystem.h"
#include "materialsystem/itexture.h"
#include "toolframework_client.h"
#include "tier0/icommandline.h"
#include "IEngineVGui.h"
#include <vgui_controls/Controls.h>
#include <vgui/ISurface.h>
#include "ScreenSpaceEffects.h"

// previous includes mirror those in view.cpp


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