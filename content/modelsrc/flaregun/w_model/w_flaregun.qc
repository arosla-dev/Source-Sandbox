// Created by Crowbar 0.74

$modelname "weapons/W_flaregun.mdl"

$bodygroup "studio"
{
	studio "w_flaregun_reference.smd"
}


$surfaceprop "weapon"

$contents "solid"

$illumposition -1.183 0.603 -0.353

$cdmaterials "models\weapons\W_flaregun\"

$cbox 0 0 0 0 0 0

$bbox -6.259 -5.385 -1.818 5.054 3.017 1.131

$definebone "Line03" "" -1.183835 0.612185 -0.36463 -0.20002 0 0 0 0 0 0 0 0


$sequence "idle" {
	"w_flaregun_anims\idle.smd"
	activity "ACT_VM_IDLE" 1
	fadein 0.2
	fadeout 0.2
	fps 30
	loop
}

$sequence "attack1" {
	"w_flaregun_anims\attack1.smd"
	fadein 0.2
	fadeout 0.2
	fps 30
}

$collisionmodel "w_flaregun_physics.smd"
{
	$mass 3
	$inertia 1
	$damping 0
	$rotdamping 0
	$rootbone " "

}
