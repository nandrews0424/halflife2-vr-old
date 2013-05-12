$cd "D:\dev\hl2-vr-bkp\assets\weapons\Crossbow"

$origin -7 -19 -6.8

$modelname "Weapons/v_crossbow.mdl"
$model "studio" "Crossbow_reference3.smd"

$cdmaterials "models\weapons\v_crossbow\"
$cdmaterials "models\Weapons\V_hand\"

$texturegroup skinfamilies
{
	{
		"string.vmt"
		"v_hand_sheet.vmt"
		"crossbow dirtmap.vmt"
	}
	{
		"string.vmt"
		"v_hand_sheet.vmt"
		"crossbow dirtmap_glow.vmt"
	}
}

$hboxset "default"
$hbox 0 "Crossbow_model.Base" 0.000  0.000  0.000  10.363  16.991  17.135
$hbox 0 "Crossbow_model.Bip01_L_Hand" 0.000  -2.098  -2.103  3.349  0.701  1.887
$hbox 0 "Crossbow_model.Bip01_L_UpperArm" -1.385  -3.578  -3.093  11.693  2.942  2.846
$hbox 0 "Crossbow_model.Bip01_L_Forearm" -2.023  -3.145  -3.129  11.482  2.540  2.721
$hbox 0 "Crossbow_model.Bip01_L_Finger4" 0.000  -0.558  -0.766  1.580  0.636  0.392
$hbox 0 "Crossbow_model.Bip01_L_Finger1" -0.426  -0.818  -0.444  1.719  0.443  0.712
$hbox 0 "Crossbow_model.Bip01_L_Finger0" -0.252  -0.474  -1.127  2.235  0.909  0.615
$hbox 0 "Crossbow_model.Crossbow_base" -15.076  -3.363  -12.717  14.494  5.518  21.035
$hbox 0 "Crossbow_model.bowr1" -5.666  -1.567  -3.936  2.962  0.332  0.529
$hbox 0 "Crossbow_model.bowl1" -2.059  -1.567  -3.909  5.687  0.332  0.557
$hbox 0 "Crossbow_model.bowr2" -0.445  -2.065  -0.706  6.688  0.580  6.898
$hbox 0 "Crossbow_model.bowl2" -6.366  -2.041  -0.694  0.576  0.554  6.857

$attachment "spark" "Crossbow_model.Spark2" 0.50 0.00 0.00 rotate -90.00 -0.00 0.00
$attachment "bolt_start" "Crossbow_model.bolt" 0.00 -0.00 16.00 rotate -90.00 -0.00 0.00
$attachment "bolt_end" "Crossbow_model.bolt" 0.00 0.00 -4.00 rotate -90.00 -0.00 0.00

$surfaceprop "default"

$illumposition 23.040 -5.678 -13.336

$sequence idle "idle" loop ACT_VM_IDLE 1 fps 30.00 node Ready
$sequence idle_empty "idle_empty" loop ACT_VM_FIDGET 1 fps 30.00 node Ready
$sequence draw "draw" ACT_VM_DRAW 1 fps 30.00 node Ready {
  { event 3013 14 }
  { event 3016 28 }
}

$sequence fire "fire" ACT_VM_PRIMARYATTACK 1 fps 30.00 node Ready
$sequence reload "reload" ACT_VM_RELOAD 1 fps 30.00 node Ready {
  { event 3005 28 }
  { event 3013 35 }
  { event 3016 55 }
}

$sequence holster "holster" ACT_VM_HOLSTER 1 fps 30.00 node Ready
$sequence idletolow "idletolow" ACT_VM_LOWERED_TO_IDLE 1 fps 30.00 transition Ready Low
$sequence lowtoidle "lowtoidle" ACT_VM_IDLE_TO_LOWERED 1 fps 30.00 transition Low Ready
$sequence lowidle "lowidle" loop ACT_VM_IDLE_LOWERED 1 fps 30.00 node Low
