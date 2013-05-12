$cd "E:\Work\Rift Hydra Mods\HL2\StunBaton"

$modelname "Weapons/V_Stunbaton.mdl"
$model "studio" "Stunbaton_reference.smd"

$cdmaterials "models\Weapons\V_hand\"
$cdmaterials "models\Weapons\V_Stunbaton\"

$hboxset "default"
$hbox 0 "Bip01 R UpperArm" -0.229  -3.262  -3.280  13.447  3.720  2.839
$hbox 0 "Bip01 R Forearm" 0.000  -3.273  -2.683  15.997  3.016  2.875
$hbox 0 "Bip01 R Hand" 0.000  -1.835  -2.277  4.823  1.611  2.666
$hbox 0 "Bip01 R Finger0" -0.011  -0.406  -0.644  1.507  0.936  1.437
$hbox 0 "Bip01 R Finger01" 0.000  -0.424  -0.449  1.035  0.753  0.716
$hbox 0 "Bip01 R Finger1" 0.000  -0.873  -0.674  1.546  0.548  0.503
$hbox 0 "Bip01 R Finger11" 0.000  -0.611  -0.483  1.667  0.469  0.689
$hbox 0 "Bip01 R Finger21" 0.000  -0.633  -0.668  1.818  0.465  0.556
$hbox 0 "Bip01 R Finger3" 0.000  -1.061  -0.738  1.507  0.286  0.537
$hbox 0 "Bip01 R Finger31" 0.000  -0.725  -0.529  1.554  0.311  0.612
$hbox 0 "Bip01 R Finger41" -0.262  -0.647  -0.611  1.159  0.397  0.401
$hbox 0 "Dummy14" -1.642  -1.594  -5.528  1.642  1.607  20.312
$hbox 0 "Dummy15" -0.976  -0.973  -1.223  0.973  0.969  0.135

$attachment "0" "tip" 0.00 0.00 -0.00 rotate -0.00 -0.00 -0.00

$surfaceprop "default"

$illumposition 12.811 -9.662 -12.948

$sequence idle01 "idle01" loop ACT_VM_IDLE 1 fps 30.00
$sequence draw "draw" ACT_VM_DRAW 1 fps 30.00
$sequence attackch "attackch" ACT_VM_HITCENTER 1 fps 30.00
$sequence attackcm "attackcm" ACT_VM_MISSCENTER 1 fps 30.00