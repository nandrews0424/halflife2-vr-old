$cd "D:\dev\hl2-vr-bkp\assets\weapons\RPG"

// right/left forward/back down/up
$origin -22 -32 -12

$modelname "Weapons/v_RPG.mdl"
$model "studio" "RPG_reference.smd"

$cdmaterials "Models\Weapons\W_rocket_launcher\"
$cdmaterials "models\Weapons\V_hand\"

// Model uses material "w_rpg sheet.vmt"
// Model uses material "v_hand_sheet.vmt"

$attachment "0" "muzzle" -0.00 -0.00 -0.00 rotate -0.00 0.00 -0.00
$attachment "laser" "muzzle" -9.00 -5.00 0.50 rotate -0.00 0.00 -0.00
$attachment "laser_end" "muzzle" 100.00 -5.00 0.50 rotate -0.00 0.00 -0.00

$surfaceprop "default"

$illumposition 36.922 -16.708 -8.533

$sequence idle1 "idle1" loop ACT_VM_IDLE 1 fps 30.00 node Idle
$sequence draw "draw" ACT_VM_DRAW 1 fps 30.00 node Idle
$sequence fire "fire" ACT_VM_PRIMARYATTACK 1 fps 30.00 node Fire {
  { event 5001 0 }
}

$sequence reload "reload" ACT_VM_RELOAD 1 fps 30.00 node Idle
$sequence up_to_down "up_to_down" ACT_VM_IDLE_TO_LOWERED 1 fps 30.00 transition Idle Low
$sequence down_to_up "down_to_up" ACT_VM_LOWERED_TO_IDLE 1 fps 30.00 transition Low Idle
$sequence down_idle "down_idle" loop ACT_VM_IDLE_LOWERED 1 fps 30.00 node Low