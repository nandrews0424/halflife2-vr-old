$cd "D:\valve-toolkit\weapons\357"

$origin 13.5 -6.5 -5.5

$modelname "weapons/v_357.mdl"
$model "studio" "Python_357_reference.smd"

$cdmaterials "models\Weapons\V_hand\"
$cdmaterials "models\Weapons\V_357\"

$hboxset "default"
$hbox 0 "Base" -6.212  -0.713  -1.956  7.724  4.071  8.058
$hbox 0 "Bip01_L_Forearm" 0.000  -2.880  -3.014  11.482  2.540  2.542
$hbox 0 "Bip01_L_Hand" 0.000  -2.093  -2.103  4.592  0.708  1.751
$hbox 0 "Bip01_L_Finger0" 0.000  -0.444  -1.279  2.236  0.916  1.135
$hbox 0 "Arm" -2.748  -2.963  -2.079  2.976  7.091  13.080
$hbox 0 "Hand" -1.794  -1.997  -2.345  1.426  5.099  12.268
$hbox 0 "Cylinder" -1.050  -1.060  -1.431  1.060  1.079  0.731

// Model uses material "v_hand_sheet.vmt"
// Model uses material "357_sheet.vmt"

$attachment "muzzle" "muzzle" -0.00 -0.00 0.00 rotate -90.00 90.00 0.00

$surfaceprop "default"

$illumposition 12.453 -3.224 -9.997


$sequence idle01 "idle01" loop ACT_VM_IDLE 1 fps 30.00 node Ready
$sequence fire "fire" ACT_VM_PRIMARYATTACK 1 fps 30.00 node Fire {
  { event AE_MUZZLEFLASH 0 "357 muzzle" }
}

$sequence draw "draw" ACT_VM_DRAW 1 fps 30.00 node Ready
$sequence reload "reload" ACT_VM_RELOAD 1 fps 30.00 node Ready {
  { event AE_CL_PLAYSOUND 28 "Weapon_357.OpenLoader" }
  { event AE_CL_PLAYSOUND 39 "Weapon_357.RemoveLoader" }
  { event 3015 55 }
  { event AE_CL_PLAYSOUND 67 "Weapon_357.ReplaceLoader" }
  { event AE_CL_PLAYSOUND 92 "Weapon_357.Spin" }
}

$sequence holster "holster" ACT_VM_HOLSTER 1 fps 30.00 node Ready
$sequence idletolow "idletolow" ACT_VM_LOWERED_TO_IDLE 1 fps 30.00 transition Ready Low
$sequence lowtoidle "lowtoidle" ACT_VM_IDLE_TO_LOWERED 1 fps 30.00 transition Low Ready
$sequence lowidle "lowidle" loop ACT_VM_IDLE_LOWERED 1 fps 30.00 node Low