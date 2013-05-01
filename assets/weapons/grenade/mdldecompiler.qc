$cd "D:\valve-toolkit\weapons\grenade"
$origin -4 -22 -5
$modelname "weapons/v_Grenade.mdl"
$model "studio" "Grenade_reference.smd"
$cdmaterials "models\Weapons\V_hand\"
$cdmaterials "models\Weapons\V_Grenade\"
$hboxset "default"
$hbox 0 "ValveBiped.Bip01_Spine4" 0.000  0.000  -1.938  2.033  1.001  1.938
$hbox 0 "ValveBiped.Bip01_L_Forearm" 0.000  -2.345  -2.627  11.482  1.912  2.222
$hbox 0 "ValveBiped.Bip01_L_Hand" -0.940  -1.938  -1.864  4.304  0.992  1.887
$hbox 0 "ValveBiped.Bip01_L_Finger3" 0.000  -0.778  -0.623  1.826  0.400  0.439
$hbox 0 "ValveBiped.Bip01_L_Finger0" 0.000  -0.455  -1.279  2.198  1.123  0.471
$hbox 0 "ValveBiped.Bip01_L_Finger01" 0.000  -0.333  -0.647  1.275  0.712  0.361
$hbox 0 "ValveBiped.Bip01_R_UpperArm" -1.320  -3.572  -2.929  11.693  2.830  3.010
$hbox 0 "ValveBiped.Bip01_R_Forearm" -1.959  -3.135  -2.804  11.482  2.507  3.047
$hbox 0 "ValveBiped.Bip01_R_Hand" -0.847  -2.623  -1.876  4.377  1.043  1.997
$hbox 0 "ValveBiped.Bip01_R_Finger0" 0.000  -0.469  -0.897  1.857  1.116  1.245
$hbox 0 "ValveBiped.Bip01_R_Finger01" 0.000  -0.398  -0.295  1.263  0.678  0.738
$hbox 0 "ValveBiped.Grenade_body" -1.649  -1.779  -5.018  1.840  2.067  3.726
// Model uses material "v_hand_sheet.vmt"
// Model uses material "grenadetop.vmt"
// Model uses material "handle.vmt"
// Model uses material "blackseal.vmt"
// Model uses material "grenade body.vmt"
// Model uses material "rim.vmt"
$surfaceprop "default"
$illumposition 15.495 -1.044 -17.007
$sequence idle01 "idle01" loop ACT_VM_IDLE 1 fps 30.00
$sequence drawbackhigh "drawbackhigh" ACT_VM_PULLBACK_HIGH 1 fps 20.00 {
  { event 3900 4 }
}

$sequence drawbacklow "drawbacklow" ACT_VM_PULLBACK_LOW 1 fps 20.00 {
  { event 3900 4 }
}

$sequence throw "throw" ACT_VM_THROW 1 fps 20.00 {
  { event 3005 1 }
}

$sequence roll "roll" ACT_VM_SECONDARYATTACK 1 fps 20.00 {
  { event 3013 2 }
}

$sequence lob "lob" ACT_VM_HAULBACK 1 fps 20.00 {
  { event 3016 2 }
}

$sequence draw "draw" ACT_VM_DRAW 1 fps 30.00
