$cd "D:\valve-toolkit\weapons\pistol"

$origin 22 -7 -5.5

$modelname "weapons/v_Pistol.mdl"
$model "studio" "Pistol_reference.smd"

$cdmaterials "models\Weapons\V_hand\"
$cdmaterials "models\Weapons\V_Pistol\"

$hboxset "default"
$hbox 0 "ValveBiped.Bip01_Spine4" 0.000  0.000  -1.938  2.033  1.001  1.938
$hbox 0 "ValveBiped.Bip01_L_Forearm" 0.000  -2.880  -3.014  11.482  2.540  2.542
$hbox 0 "ValveBiped.Bip01_L_Hand" -0.940  -1.938  -2.103  4.304  0.992  1.887
$hbox 0 "ValveBiped.Bip01_L_Finger3" 0.000  -0.778  -0.625  1.826  0.400  0.439
$hbox 0 "ValveBiped.Bip01_L_Finger0" 0.000  -0.455  -1.279  1.790  1.123  0.686
$hbox 0 "ValveBiped.Bip01_L_Finger01" 0.000  -0.384  -0.712  1.275  0.876  0.361
$hbox 0 "ValveBiped.Bip01_R_Forearm" -1.936  -3.135  -2.711  11.482  2.550  3.047
$hbox 0 "ValveBiped.Bip01_R_Hand" -0.847  -2.781  -2.009  6.115  1.043  2.113
$hbox 0 "ValveBiped.Bip01_R_Finger0" 0.000  -0.468  -0.000  2.212  0.962  1.217
$hbox 0 "ValveBiped.Bip01_R_Finger01" 0.000  -0.358  -0.568  1.261  1.035  0.696
$hbox 0 "ValveBiped.base" -0.738  -3.635  -4.366  0.783  3.122  5.938
$hbox 0 "ValveBiped.square" -0.704  -0.635  -4.287  0.672  1.017  4.029
$hbox 0 "ValveBiped.clip" -0.711  -2.509  -1.420  0.517  2.634  1.139

// Model uses material "v_hand_sheet.vmt"
// Model uses material "v_pistol_sheet.vmt"

$attachment "muzzle" "ValveBiped.muzzle" -0.00 -0.00 -0.00 rotate -0.00 -0.00 0.00
$attachment "1" "ValveBiped.eject" -1.00 -0.00 -0.00 rotate -0.00 90.00 180.00

$surfaceprop "default"

$illumposition 12.972 0.702 -35.781

$sequence idle01 "idle01" loop ACT_VM_IDLE 1 fps 30.00 node 0
$sequence idle01empty "idle01empty" loop ACT_VM_IDLE_EMPTY 1 fps 30.00 node 0
$sequence fire "fire" ACT_VM_PRIMARYATTACK 1 fps 30.00 node 2 {
  { event AE_MUZZLEFLASH 0 "PISTOL MUZZLE" }
}

$sequence fire1 "fire1" ACT_VM_RECOIL1 1 fps 30.00 node 2 {
  { event AE_MUZZLEFLASH 0 "PISTOL MUZZLE" }
}

$sequence fire2 "fire2" ACT_VM_RECOIL2 1 fps 30.00 node 2 {
  { event AE_MUZZLEFLASH 0 "PISTOL MUZZLE" }
}

$sequence fire3 "fire3" ACT_VM_RECOIL3 1 fps 30.00 node 2 {
  { event AE_MUZZLEFLASH 0 "PISTOL MUZZLE" }
}

$sequence fireempty "fireempty" ACT_VM_DRYFIRE 1 fps 30.00 node 2
$sequence draw "draw" ACT_VM_DRAW 1 fps 30.00 node 0
$sequence drawempty "drawempty" fps 30.00 node 0
$sequence reload "reload" ACT_VM_RELOAD 1 fps 30.00 node 0
$sequence holster "holster" ACT_VM_HOLSTER 1 fps 30.00 node 0
$sequence holsterempty "holsterempty" ACT_VM_HOLSTER 1 fps 30.00 node 0
$sequence idletolow "idletolow" ACT_VM_LOWERED_TO_IDLE 1 fps 30.00 transition 0 1
$sequence lowtoidle "lowtoidle" ACT_VM_IDLE_TO_LOWERED 1 fps 30.00 transition 1 0
$sequence lowidle "lowidle" loop ACT_VM_IDLE_LOWERED 1 fps 30.00 node 1