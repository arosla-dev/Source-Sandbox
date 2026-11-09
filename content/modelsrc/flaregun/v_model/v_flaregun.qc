// Created by Crowbar 0.74

$modelname "Weapons/v_flaregun.mdl"

$bodygroup "studio"
{
	studio "Flaregun_reference.smd"
}
$bodygroup "studio"
{
	studio "Shell_reference.smd"
}


$surfaceprop "default"

$contents "solid"

$illumposition 0.998 -17.918 -18.873

$cdmaterials "models\Weapons\V_hand\\"
$cdmaterials "models\Weapons\V_flaregun\\"

$attachment "muzzle" "Bip01 R Hand" 14.5 0.5 4.5 rotate 0 0 90

$cbox 0 0 0 0 0 0

$bbox -4.479 -11.704 -36.403 40.315 13.699 -1.344

$definebone "Bip01" "" 0.134137 2.717642 43.181469 0 -89.999927 0 0 0 0 0 0 0
$definebone "Bip01 Pelvis" "Bip01" 0 0 0 -89.773411 85.014251 -175.018707 0 0 0 0 0 0
$definebone "Bip01 Spine" "Bip01 Pelvis" 4.443588 -0.009634 -0.0175 0.260066 0.045894 -0.000057 0 0 0 0 0 0
$definebone "Bip01 Neck" "Bip01 Spine" 12.111423 -0.011487 0.007272 0 0 0 0 0 0 0 0 0
$definebone "Bip01 R Clavicle" "Bip01 Neck" 0.00082 0.011449 -1.352403 78.575261 179.953041 -0.001031 0 0 0 0 0 0
$definebone "Bip01 R UpperArm" "Bip01 R Clavicle" 6.07808 0.000005 0.000004 -2.571091 -88.15609 72.743468 0 0 0 0 0 0
$definebone "Bip01 R Forearm" "Bip01 R UpperArm" 13.446745 -0.00034 -0.000003 -0.000573 -3.959253 0.000173 0 0 0 0 0 0
$definebone "Bip01 R Hand" "Bip01 R Forearm" 15.997018 -0.000374 -0.000161 0.00086 0.000974 96.454811 0 0 0 0 0 0
$definebone "Dummy17" "Bip01 R Hand" 3.915661 -0.277994 -1.402447 -0.00063 0.000802 0.044974 0 0 0 0 0 0
$definebone "Dummy14" "Bip01 R Hand" -0.56724 7.913957 8.606499 -1.063674 -179.114553 90.656397 0 0 0 0 0 0
$definebone "Dummy15" "Dummy14" -8.090256 1.809784 0.001158 0 0.000458 -0.000061 0 0 0 0 0 0
$definebone "Dummy16" "Dummy14" -13.128956 1.564598 -0.031926 0 0.000458 -0.000061 0 0 0 0 0 0
$definebone "Bip01 L Clavicle" "Bip01 Neck" -0.000057 0.011442 1.352403 -78.537962 179.955732 -0.000801 0 0 0 0 0 0
$definebone "Bip01 L UpperArm" "Bip01 L Clavicle" 6.078087 -0.000004 -0.000011 2.568742 -88.156664 -72.74381 0 0 0 0 0 0
$definebone "Bip01 L Forearm" "Bip01 L UpperArm" 13.446768 0.000145 -0.00001 -0.000286 -3.958738 -0.000001 0 0 0 0 0 0
$definebone "Bip01 L Hand" "Bip01 L Forearm" 15.997007 0.000042 -0.000093 0.000516 -0.000974 -96.454121 0 0 0 0 0 0
$definebone "Bip01 L Finger0" "Bip01 L Hand" 1.885353 0.83326 -2.177845 39.99962 -0.039249 -91.560257 0 0 0 0 0 0
$definebone "Bip01 L Finger01" "Bip01 L Finger0" 1.506958 -0.000011 0.000011 -0.000228 -7.999807 -0.000001 0 0 0 0 0 0
$definebone "Bip01 L Finger02" "Bip01 L Finger01" 0.798344 -0.000008 0.000001 0.000058 -6.500032 0.000056 0 0 0 0 0 0
$definebone "Bip01 L Finger1" "Bip01 L Hand" 3.950314 -0.539905 -2.108791 8.999677 -0.007735 -0.04658 0 0 0 0 0 0
$definebone "Bip01 L Finger11" "Bip01 L Finger1" 1.545799 0.000006 0.000023 0.000172 27.000122 0.000058 0 0 0 0 0 0
$definebone "Bip01 L Finger12" "Bip01 L Finger11" 1.667393 0.000006 0.000027 -0.000115 24.999643 -0.000399 0 0 0 0 0 0
$definebone "Bip01 L Finger2" "Bip01 L Hand" 3.899466 -0.475402 -1.170815 -0.000344 -0.000115 -0.046007 0 0 0 0 0 0
$definebone "Bip01 L Finger21" "Bip01 L Finger2" 1.528215 -0.000006 0.000019 0.000057 18.999918 -0.00017 0 0 0 0 0 0
$definebone "Bip01 L Finger22" "Bip01 L Finger21" 1.528221 -0.000004 0.000023 -0.000116 27.499743 -0.000285 0 0 0 0 0 0
$definebone "Bip01 L Finger3" "Bip01 L Hand" 3.899454 -0.431826 0.168301 -0.000344 -0.000401 -0.04595 0 0 0 0 0 0
$definebone "Bip01 L Finger31" "Bip01 L Finger3" 1.506966 0.000005 0.000019 0.000114 24.500018 -0.00017 0 0 0 0 0 0
$definebone "Bip01 L Finger32" "Bip01 L Finger31" 1.506962 0.000002 0.000023 -0.00023 19.500279 -0.000629 0 0 0 0 0 0
$definebone "Bip01 L Finger4" "Bip01 L Hand" 3.915619 -0.278038 1.402683 -0.000344 -0.000115 -0.046007 0 0 0 0 0 0
$definebone "Bip01 L Finger41" "Bip01 L Finger4" 1.333088 -0.000005 0.000008 -0.000001 36.50022 -0.000457 0 0 0 0 0 0
$definebone "Bip01 L Finger42" "Bip01 L Finger41" 1.159203 -0.000008 0.000023 -0.000058 12.499357 -0.000056 0 0 0 0 0 0
$definebone "Bip01 R Finger0" "Bip01 R Hand" 1.885321 0.833257 2.17791 -40.000071 -0.038449 91.55928 0 0 0 0 0 0
$definebone "Bip01 R Finger01" "Bip01 R Finger0" 1.506977 0.000013 0.000008 -0.000231 -7.999459 0.00006 0 0 0 0 0 0
$definebone "Bip01 R Finger02" "Bip01 R Finger01" 0.798347 0 0.000002 -0.000002 -6.500144 0.000003 0 0 0 0 0 0
$definebone "Bip01 R Finger1" "Bip01 R Hand" 3.950291 -0.539914 2.108936 -8.999676 -0.007507 0.046463 0 0 0 0 0 0
$definebone "Bip01 R Finger11" "Bip01 R Finger1" 1.545799 -0.000008 0.000034 -0.000226 26.999833 -0.000919 0 0 0 0 0 0
$definebone "Bip01 R Finger12" "Bip01 R Finger11" 1.667397 -0.000008 0.000023 -0.000168 25.000502 -0.000173 0 0 0 0 0 0
$definebone "Bip01 R Finger2" "Bip01 R Hand" 3.89945 -0.475414 1.170952 0.000344 -0.000631 0.045948 0 0 0 0 0 0
$definebone "Bip01 R Finger21" "Bip01 R Finger2" 1.528225 0.000003 0.000027 -0.000056 19.000201 -0.000462 0 0 0 0 0 0
$definebone "Bip01 R Finger22" "Bip01 R Finger21" 1.528212 0 0.000031 -0.000112 27.499805 -0.000575 0 0 0 0 0 0
$definebone "Bip01 R Finger3" "Bip01 R Hand" 3.899458 -0.431839 -0.168159 0.000344 -0.00023 0.045948 0 0 0 0 0 0
$definebone "Bip01 R Finger31" "Bip01 R Finger3" 1.506962 -0.000008 0.000027 -0.000113 24.49962 -0.000691 0 0 0 0 0 0
$definebone "Bip01 R Finger32" "Bip01 R Finger31" 1.506968 0.000004 0.000023 0.000003 19.500279 -0.000002 0 0 0 0 0 0
$definebone "Bip01 R Finger4" "Bip01 R Hand" 3.915638 -0.278054 -1.40255 0.000344 -0.00023 0.045948 0 0 0 0 0 0


$sectionframes 30 249

$sequence "Flaregun_draw" {
	"v_flaregun_anims\Flaregun_draw.smd"
	activity "ACT_VM_DRAW" 1
	fadein 0.2
	fadeout 0.2
	node "1"
	snap
	fps 30
}

$sequence "Flaregun_idle" {
	"v_flaregun_anims\Flaregun_idle.smd"
	activity "ACT_VM_IDLE" 1
	fadein 0.2
	fadeout 0.2
	node "1"
	fps 30
	loop
}

$sequence "nothreatidle" {
	"v_flaregun_anims\nothreatidle.smd"
	activity "ACT_VM_IDLE_LOWERED" 1
	fadein 0.2
	fadeout 0.2
	node "Low"
	fps 30
	loop
}

$sequence "nothreattoidle" {
	"v_flaregun_anims\nothreattoidle.smd"
	activity "ACT_VM_LOWERED_TO_IDLE" 1
	fadein 0.2
	fadeout 0.2
	transition "0" "1"
	fps 30
}

$sequence "idletonothreat" {
	"v_flaregun_anims\idletonothreat.smd"
	activity "ACT_VM_IDLE_TO_LOWERED" 1
	fadein 0.2
	fadeout 0.2
	fps 30
}

$sequence "Flaregun_fire" {
	"v_flaregun_anims\Flaregun_fire.smd"
	activity "ACT_VM_PRIMARYATTACK" 1
	{ event AE_MUZZLEFLASH 0 "357 MUZZLE" }
	fadein 0.2
	fadeout 0.2
	node "Fire"
	snap
	fps 30
}

$sequence "Flaregun_dryfire" {
	"v_flaregun_anims\Flaregun_dryfire.smd"
	activity "ACT_VM_DRYFIRE" 1
	fadein 0.2
	fadeout 0.2
	snap
	fps 30
}

$sequence "Flaregun_reload" {
	"v_flaregun_anims\Flaregun_reload.smd"
	activity "ACT_VM_RELOAD" 1
	{ event 5004 57 "Weapon_Flaregun.Reload" }
	fadein 0.2
	fadeout 0.2
	node "1"
	fps 30
}

$sequence "Flaregun_Holster" {
	"v_flaregun_anims\Flaregun_Holster.smd"
	activity "ACT_VM_HOLSTER" 1
	fadein 0.2
	fadeout 0.2
	fps 30
}
