$cd "D:\dev\hl2-vr-bkp\assets\weapons\IRifle"

// forward/back right/left down/up
$origin -18 4 -4

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

$sequence IR_idle "IR_idle" loop ACT_VM_IDLE 1 snap rotate 90 fps 30.00 node Ready
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

$sequence IR_draw "IR_draw" ACT_VM_DRAW 1 snap rotate 90 fps 30.00 node Ready
$sequence IR_holster "IR_holster" ACT_VM_HOLSTER 1 snap rotate 90 fps 30.00 node Ready
$sequence idletolow "idletolow" ACT_VM_LOWERED_TO_IDLE 1 snap rotate 90 fps 30.00 transition Ready Low
$sequence lowtoidle "lowtoidle" ACT_VM_IDLE_TO_LOWERED 1 snap rotate 90 fps 30.00 transition Low Ready
$sequence lowidle "lowidle" loop ACT_VM_IDLE_LOWERED 1 snap rotate 90 fps 30.00 node Low
$sequence shake "shake" loop ACT_VM_FIDGET 1 snap rotate 90 fps 30.00 node Fire

$animation IR_delta "IR_delta" snap rotate 90 fps 30
$animation a_vent_open "a_vent_open" snap rotate 90 fps 30.000000 subtract IR_delta 0
$animation a_vent_closed "a_vent_closed" snap rotate 90 fps 30.000000 subtract IR_delta 0

$sequence VentPoses "a_vent_open" snap rotate 90 fps 30.00 {
  blendwidth 2
  blend VentPoses 0.000000 1.000000
  delta
  autoplay
 a_vent_closed
}