$cd "D:\dev\hl2-vr-bkp\assets\weapons\smg1"

$origin 15.5 -7 -6.5

$modelname "weapons/v_smg1.mdl"
$model "studio" "Smg1_reference.smd"

$cdmaterials "models\Weapons\V_hand\"
$cdmaterials "models\Weapons\V_smg1\"

$hboxset "default"
$hbox 0 "ValveBiped.Bip01_Spine2" 0.000  -0.000  -8.400  11.636  20.498  0.000
$hbox 0 "ValveBiped.Bip01_Spine4" 0.000  0.000  0.000  2.033  1.001  1.938
$hbox 0 "ValveBiped.Bip01_L_UpperArm" -1.385  -3.578  -3.093  11.693  2.942  2.846
$hbox 0 "ValveBiped.Bip01_L_Forearm" -2.023  -3.145  -3.129  11.775  2.540  2.721
$hbox 0 "ValveBiped.Bip01_L_Hand" 0.000  -1.938  -2.103  4.304  0.708  1.887
$hbox 0 "ValveBiped.Bip01_L_Finger3" 0.000  -0.778  -0.625  1.826  0.400  0.439
$hbox 0 "ValveBiped.Bip01_L_Finger0" 0.000  -0.455  -1.279  2.198  1.123  0.471
$hbox 0 "ValveBiped.Bip01_L_Finger01" 0.000  -0.333  -0.647  1.275  0.712  0.361
$hbox 0 "ValveBiped.base" -3.794  -6.408  -15.488  1.693  3.229  13.888
$hbox 0 "ValveBiped.handle" -0.575  -5.080  -0.798  0.474  0.433  0.828

// Model uses material "v_smg1_sheet.vmt"
// Model uses material "v_hand_sheet.vmt"
// Model uses material "texture5.vmt"
// Model uses material "texture4.vmt"

$attachment "muzzle" "ValveBiped.muzzle" 0.00 -0.00 -0.00 rotate 0.00 -0.00 0.00
$attachment "1" "ValveBiped.eject" -0.00 -0.00 0.00 rotate 0.00 0.00 0.00

$surfaceprop "default"

$illumposition 17.271 -1.154 -16.696

$sequence fire01 "fire01" ACT_VM_PRIMARYATTACK 1 snap rotate -90 fps 30.00 node Fire {
  { event AE_MUZZLEFLASH 0 "SMG1 MUZZLE" }
  { event 6001 0 "0" }
}

$sequence fire02 "fire02" ACT_VM_RECOIL1 1 snap rotate -90 fps 30.00 node Fire {
  { event AE_MUZZLEFLASH 0 "SMG1 MUZZLE" }
  { event 6001 0 "0" }
}

$sequence fire03 "fire03" ACT_VM_RECOIL2 1 snap rotate -90 fps 30.00 node Fire {
  { event AE_MUZZLEFLASH 0 "SMG1 MUZZLE" }
  { event 6001 0 "0" }
}

$sequence fire04 "fire04" ACT_VM_RECOIL3 1 snap rotate -90 fps 30.00 node Fire {
  { event AE_MUZZLEFLASH 0 "SMG1 MUZZLE" }
  { event 6001 0 "0" }
}

$sequence altfire "altfire" ACT_VM_SECONDARYATTACK 1 snap rotate -90 fps 30.00 node Fire

$sequence reload "reload" ACT_VM_RELOAD 1 snap rotate -90 fps 30.00 node Ready