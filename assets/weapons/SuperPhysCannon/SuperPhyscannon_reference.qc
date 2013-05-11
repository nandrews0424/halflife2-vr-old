$cd "D:\valve-toolkit\weapons\SuperPhysCannon"
$origin -12 -28.5 58
$modelname "weapons/v_physcannon.mdl"
$model "studio" "SuperPhyscannon_reference.smd"
$poseparameter active 0.00 1.00
$cdmaterials "models\weapons\v_physcannon\"
$cdmaterials "models\weapons\v_hand\"
$hboxset "default"
$hbox 0 "Base" -6.977  -8.508  -15.484  14.471  5.898  18.091
$hbox 0 "Prong_B" -1.620  -5.240  -1.378  4.704  1.820  11.821
$hbox 0 "Prong_A" -0.796  -1.506  -1.822  0.655  7.236  11.386
$hbox 0 "Doodad_A1" -2.188  -0.599  -0.272  0.678  2.401  0.818
$hbox 0 "Handle" -3.116  -3.526  -10.562  3.400  1.962  2.144
$hbox 0 "square" -0.034  -5.232  -7.112  5.406  0.799  0.644

// Model uses material "v_hand_sheet.vmt"
// Model uses material "v_physcannon_sheet.vmt"
$attachment "muzzle" "Muzzle" -0.00 0.25 -2.00 rotate 0.00 0.00 0.00
$attachment "fork1b" "Base_A" -0.00 -0.00 -0.00 rotate 0.00 0.00 0.00
$attachment "fork1m" "Prong_A" 0.00 0.00 -0.00 rotate -0.00 0.00 0.00
$attachment "fork1t" "Tip_A" -0.00 -0.00 0.00 rotate 0.00 0.00 0.00
$attachment "fork2b" "Base_B" -0.00 0.00 0.00 rotate 0.00 0.00 0.00
$attachment "fork2m" "Prong_B" 0.00 0.00 0.00 rotate -0.00 0.00 0.00
$attachment "fork2t" "Tip_B" -0.00 -0.00 0.00 rotate 0.00 0.00 0.00
$attachment "fork3b" "Base_C" -0.00 0.00 0.00 rotate 0.00 0.00 0.00
$attachment "fork3m" "Prong_C" 0.00 0.00 0.00 rotate -0.00 0.00 0.00
$attachment "fork3t" "Tip_C" -0.00 -0.00 0.00 rotate 0.00 0.00 0.00
$surfaceprop "default"

$illumposition 47.232 -8.546 -11.292

$sequence fire "fire" ACT_VM_PRIMARYATTACK 1 fps 30.00
$sequence altfire "altfire" ACT_VM_SECONDARYATTACK 1 fps 30.00

$animation ProngsShut "ProngsShut" fps 30.000000
$animation ProngsOpen "ProngsOpen" fps 30.000000

$sequence prong_blend "ProngsShut" fps 30.00 {
  blendwidth 2
  blend active 0.000000 1.000000
  autoplay
 ProngsShut ProngsOpen
}