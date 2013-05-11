$cd "D:\valve-toolkit\weapons\IRifle"
$origin -18 8 -8

$modelname "weapons/v_IRifle.mdl"
$model "studio" "IRifle_reference.smd"
$poseparameter VentPoses 0.00 1.00
$cdmaterials "models\Weapons\V_hand\"
$cdmaterials "models\Weapons\V_irifle\"
$hboxset "default"
$hbox 0 "Base" -6.133  -9.432  -8.556  5.653  5.246  26.734
$hbox 0 "Shell1" -0.614  -0.613  -1.121  0.613  0.615  0.727
$hbox 0 "Shell2" -0.605  -0.624  -1.123  0.622  0.604  0.725
// Model uses material "V_IRifle.vmt"
// Model uses material "v_hand_sheet.vmt"
// Model uses material "V_IRifle_core.vmt"
$attachment "muzzle" "Base" 0.00 0.00 20.00 rotate -90.00 0.00 0.00
$surfaceprop "default"
$illumposition 20.728 -6.084 -7.257

$sequence IR_fire "IR_fire" ACT_VM_PRIMARYATTACK 1 snap rotate 90 fps 30.00 node Fire {
  { event AE_MUZZLEFLASH 0 "COMBINE MUZZLE" }
}

$sequence fire2 "fire2" ACT_VM_RECOIL1 1 snap rotate 90 fps 30.00 node Fire {
  { event AE_MUZZLEFLASH 0 "COMBINE MUZZLE" }
}

$sequence fire3 "fire3" ACT_VM_RECOIL2 1 snap rotate 90 fps 30.00 node Fire {
  { event AE_MUZZLEFLASH 0 "COMBINE MUZZLE" }
}

$sequence fire4 "fire4" ACT_VM_RECOIL3 1 snap rotate 90 fps 30.00 node Fire {
  { event AE_MUZZLEFLASH 0 "COMBINE MUZZLE" }
}

$sequence IR_fire2 "IR_fire2" ACT_VM_SECONDARYATTACK 1 snap rotate 90 fps 30.00 {
  { event AE_MUZZLEFLASH 0 "COMBINE MUZZLE" }
}

$sequence IR_reload "IR_reload" ACT_VM_RELOAD 1 snap rotate 90 fps 30.00 node Ready {
  { event AE_CL_PLAYSOUND 1 "Weapon_AR2.Reload_Rotate" }
  { event AE_CL_PLAYSOUND 19 "Weapon_AR2.Reload_Push" }
}