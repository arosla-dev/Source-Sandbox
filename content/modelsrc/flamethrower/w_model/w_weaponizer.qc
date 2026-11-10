// Created by Crowbar 0.74

$modelname "weapons\w_weaponizer.mdl"

$bodygroup "w_weaponizer"
{
	studio "w_weaponizer_ref.smd"
}


$surfaceprop "default"

$contents "solid"

$illumposition 2.333 4.073 4.523

$cdmaterials "models\weapons\v_weaponizer\"

$attachment "muzzle" "weapon_bone" 0 0 -21 rotate 90 -180 0

$cbox 0 0 0 0 0 0

$bbox -30.008 -4.154 -1.944 21.861 8.82 10.99

$definebone "weapon_bone" "" 0 0 0 0 0 0 0 0 0 0 0 0


// The 32 below is a guess until further is known about the format.
$animblocksize 32 nostall

$bonesaveframe "weapon_bone" position rotation

$sequence "idle" {
	"w_weaponizer_anims\idle.smd"
	fadein 0.2
	fadeout 0.2
	snap
	fps 30
	loop
}

$collisionmodel "w_weaponizer_physics.smd"
{
	$mass 1
	$inertia 1
	$damping 0
	$rotdamping 0
	$rootbone " "

}
