// Created by Crowbar 0.74

$modelname "weapons/W_binoculars.mdl"

$bodygroup "studio"
{
	studio "w_binoculars_reference.smd"
}


$contents "solid"

$illumposition -0.016 -21.691 63.598

$cdmaterials "models\weapons\v_binoculars\"

$cbox 0 0 0 0 0 0

$bbox 16.586 -4.378 61.286 26.796 4.346 65.91

$definebone "Bip01" "" 0.134137 21.745049 43.181469 0 -89.999927 0 0 0 0 0 0 0
$definebone "Bip01 Pelvis" "Bip01" 0 0 0 -89.980215 -89.999927 0 0 0 0 0 0 0
$definebone "Bip01 Spine" "Bip01 Pelvis" 4.443615 -0.00964 0.000006 -0.000057 0.045607 -0.000229 0 0 0 0 0 0
$definebone "Bip01 Neck" "Bip01 Spine" 12.111413 -0.011447 0 0 0 0 0 0 0 0 0 0
$definebone "Bip01 R Clavicle" "Bip01 Neck" 0.000009 0.011449 -1.352403 78.540823 179.953614 -0.000802 0 0 0 0 0 0
$definebone "Bip01 R UpperArm" "Bip01 R Clavicle" 6.078088 0.000001 0.000002 -2.569601 -88.156548 72.743762 0 0 0 0 0 0
$definebone "Bip01 R Forearm" "Bip01 R UpperArm" 13.446755 -0.000001 0 0 -3.959138 -0.000002 0 0 0 0 0 0
$definebone "Bip01 R Hand" "Bip01 R Forearm" 15.997 -0.000001 0 0 0 96.454354 0 0 0 0 0 0
$definebone "v_hand" "Bip01 R Hand" -8.248927 0.312686 0.689153 0 0 0.000002 0 0 0 0 0 0

$animation "a_idle" "w_binoculars_anims\idle.smd" {
	fps 30
	loop
}

$animation "a_attack1" "w_binoculars_anims\attack1.smd" {
	fps 30
}

$sequence "idle" {
	"a_idle"
	activity "ACT_VM_IDLE" 1
	fadein 0.2
	fadeout 0.2
}

$sequence "attack1" {
	"a_attack1"
	fadein 0.2
	fadeout 0.2
}


$collisionmodel "w_binoculars_physics.smd"
{
	$mass 3
	$inertia 1
	$damping 0
	$rotdamping 0
	$rootbone " "

}
