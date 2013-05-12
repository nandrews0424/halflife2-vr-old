$cd "D:\dev\hl2-vr-bkp\assets\weapons\Shotgun"

$origin -10 -20 -4.5

$modelname "weapons/v_shotgun.mdl"
$model "studio" "Shotgun_reference.smd"

$cdmaterials "models\Weapons\V_hand\"
$cdmaterials "models\Weapons\V_shotgun\"
$cdmaterials "models\weapons\shell\"
$cdmaterials "models\Weapons\W_shotgun\"

$hboxset "default"
$hbox 0 "ValveBiped.Bip01_Spine2" 0.000  0.000  -4.015  11.946  24.170  0.000
$hbox 0 "ValveBiped.Bip01_Spine4" 0.000  0.000  0.000  2.033  1.001  1.938
$hbox 0 "ValveBiped.Bip01_L_Forearm" -1.968  -3.120  -3.061  11.482  1.909  2.721
$hbox 0 "ValveBiped.Bip01_L_Hand" -1.002  -2.207  -2.103  4.805  1.469  2.103
$hbox 0 "ValveBiped.Bip01_L_Finger4" -0.161  -1.158  -1.215  1.684  0.635  0.348
$hbox 0 "ValveBiped.Bip01_L_Finger41" -1.152  -0.669  -1.215  1.060  0.621  0.408
$hbox 0 "ValveBiped.Bip01_L_Finger3" 0.000  -1.574  -1.126  1.827  0.400  0.793
$hbox 0 "ValveBiped.Bip01_L_Finger2" -0.190  -2.038  -1.039  1.784  0.417  0.677
$hbox 0 "ValveBiped.Bip01_L_Finger0" -0.575  -0.500  -1.525  2.504  2.041  0.917
$hbox 0 "ValveBiped.Bip01_L_Finger01" -0.415  -0.404  -0.914  1.687  2.220  0.361
$hbox 0 "ValveBiped.Gun" -0.947  -6.407  -22.324  1.411  2.388  22.387
$hbox 0 "ValveBiped.Pump" -1.479  -4.397  -3.906  2.637  1.149  12.420
$hbox 0 "ValveBiped.square" -0.671  -0.704  -1.307  0.621  0.588  1.386

// Model uses material "vshotgun_albedo.vmt"
// Model uses material "casing01.vmt"
// Model uses material "v_hand_sheet.vmt"

$attachment "muzzle" "ValveBiped.Gun" -0.03 0.51 23.45 rotate -90.00 0.00 0.00
$attachment "1" "ValveBiped.Gun" -0.95 1.14 -5.59 rotate -0.00 180.00 -180.00

$surfaceprop "default"

$illumposition 28.541 -4.919 -16.577

$sequence fire "fire" ACT_RANGE_ATTACK_SHOTGUN 1 fps 5.00 {
  { event AE_MUZZLEFLASH 0 "SHOTGUN MUZZLE" }
  { event 6001 0 "3" }
  { event 3004 0 }
}

$sequence fire01 "fire01" ACT_VM_PRIMARYATTACK 1 fps 30.00 node 2 {
  { event AE_MUZZLEFLASH 0 "SHOTGUN MUZZLE" }
}

$sequence altfire "altfire" ACT_VM_SECONDARYATTACK 1 fps 30.00 node 2 {
  { event AE_MUZZLEFLASH 0 "SHOTGUN MUZZLE" }
}

$sequence pump "pump" ACT_SHOTGUN_PUMP 1 fps 30.00 node 0 {
  { event 6001 5 "2" }
}

$sequence reload1 "reload1" ACT_SHOTGUN_RELOAD_START 1 fps 30.00 node 0
$sequence reload2 "reload2" ACT_VM_RELOAD 1 fps 30.00 node 0
$sequence reload3 "reload3" ACT_SHOTGUN_RELOAD_FINISH 1 fps 30.00 node 0