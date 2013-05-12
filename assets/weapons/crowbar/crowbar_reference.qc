$cd "D:\valve-toolkit\weapons\Crowbar"

$origin 25 -12 -12.5

$modelname "Weapons/v_crowbar.mdl"
$model "studio" "crowbar_reference.smd"

$cdmaterials "models\Weapons\V_hand\"
$cdmaterials "models\Weapons\V_crowbar\"

$hboxset "default"
$hbox 0 "ValveBiped.Bip01_Spine4" 0.000  0.000  -1.938  2.033  1.001  0.000
$hbox 0 "ValveBiped.Bip01_R_UpperArm" -1.763  -2.283  -2.700  11.693  4.552  3.363
$hbox 0 "ValveBiped.Bip01_R_Forearm" -1.958  -3.135  -2.804  11.482  2.550  3.047
$hbox 0 "ValveBiped.Bip01_R_Hand" 0.000  -3.141  -19.575  10.937  0.794  14.726
$hbox 0 "ValveBiped.HandControlRotR" 0.000  -1.935  -15.150  11.123  0.000  0.000

// Model uses material "crowbar_cyl.vmt"
// Model uses material "v_hand_sheet.vmt"
// Model uses material "head.vmt"

$attachment "0" "ValveBiped.Tip" 0.00 -0.00 -0.00 rotate -0.00 -0.00 0.00

$surfaceprop "default"

$illumposition 0.00 0.00 0.00

$sequence misscenter1 "misscenter1" ACT_VM_MISSCENTER 1 snap rotate -90 fps 30.00
$sequence misscenter2 "misscenter2" ACT_VM_MISSCENTER 1 snap rotate -90 fps 30.00
$sequence hitcenter1 "hitcenter1" ACT_VM_HITCENTER 1 snap rotate -90 fps 30.00
$sequence hitcenter2 "hitcenter2" ACT_VM_HITCENTER 1 snap rotate -90 fps 30.00
$sequence hitcenter3 "hitcenter3" ACT_VM_HITCENTER 1 snap rotate -90 fps 30.00
$sequence hitkill1 "hitkill1" ACT_VM_HITKILL 1 snap rotate -90 fps 30.00