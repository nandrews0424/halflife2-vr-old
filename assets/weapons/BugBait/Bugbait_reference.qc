$cd "D:\dev\hl2-vr-bkp\assets\weapons\BugBait"

$modelname "weapons/V_bugbait.mdl"
$model "studio" "Bugbait_reference.smd"

$cdmaterials "models\Weapons\V_hand\"
$cdmaterials "models\Weapons\V_bugbait\"

$hboxset "default"
$hbox 0 "ValveBiped.Bip01_Spine4" 0.000  0.000  -1.938  2.033  1.001  0.000
$hbox 0 "ValveBiped.Bip01_R_UpperArm" -1.320  -3.572  -2.929  11.693  2.948  3.010
$hbox 0 "ValveBiped.Bip01_R_Forearm" -1.959  -3.135  -2.804  11.482  2.550  3.047
$hbox 0 "ValveBiped.Bip01_R_Hand" -0.963  -3.455  -1.876  4.369  1.368  2.160
$hbox 0 "ValveBiped.Bip01_R_Finger3" 0.000  -0.786  -0.441  1.751  0.371  0.613
$hbox 0 "ValveBiped.Bip01_R_Finger0" 0.000  -0.468  -0.233  2.038  0.976  1.245
$hbox 0 "ValveBiped.Bip01_R_Finger01" -0.109  -0.397  -0.472  1.263  0.894  0.738
$hbox 0 "ValveBiped.cube3" -2.573  -2.668  -1.921  2.602  1.813  2.404
$hbox 0 "ValveBiped.cube2" -3.833  -2.332  -2.606  0.000  0.690  2.236
$hbox 0 "ValveBiped.cube1" -3.667  -2.484  -1.712  0.000  1.741  2.035
$hbox 0 "ValveBiped.cube" -3.311  -0.964  -2.103  0.000  1.597  1.124

// Model uses material "v_hand_sheet.vmt"
// Model uses material "bugbait_sheet.vmt"
// Model uses material "bugbait_inside.vmt"

$attachment "0" "ValveBiped.Bip01_R_Hand" 4.00 -4.00 0.00 rotate -0.00 0.00 0.00

$surfaceprop "default"

$illumposition 14.448 -8.144 -16.050

$sequence drawback "drawback" ACT_VM_HAULBACK 1 fps 30.00 {
  { event 3900 4 }
}

$sequence throw "throw" ACT_VM_THROW 1 fps 30.00 {
  { event 3005 1 }
}

$sequence idle01 "idle01" loop ACT_VM_IDLE 1 fps 30.00
$sequence draw "draw" ACT_VM_DRAW 1 fps 30.00
$sequence holster "holster" ACT_VM_HOLSTER 1 fps 30.00
$sequence squeeze "squeeze" ACT_VM_SECONDARYATTACK 1 fps 30.00