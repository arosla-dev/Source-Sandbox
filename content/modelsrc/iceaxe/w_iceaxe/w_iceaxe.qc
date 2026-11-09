// Created by Crowbar 0.74

$modelname "weapons/W_iceaxe.mdl"

$bodygroup "studio"
{
	studio "iceaxe_w_ref.smd"
}


$surfaceprop "plastic"

$contents "solid"

$illumposition 0 -0.51 0

$cdmaterials "models\weapons\v_iceaxe\"

$cbox 0 0 0 0 0 0

$bbox -16.875 -7.183 -25.369 11.592 6.26 8.97

$hboxset "default"
$hbox 0 "Bone01" 0 -3.636 -25.478001 7.125 0 8.861

$definebone "Bone01" "" -0.0062 0.888138 0.108722 0 89.631971 0 0 0 0 0 0 0


$sequence "idle" {
	"w_iceaxe_anims\idle.smd"
	activity "ACT_VM_IDLE" 1
	fadein 0.2
	fadeout 0.2
	fps 30
	loop
}

$sequence "attack1" {
	"w_iceaxe_anims\attack1.smd"
	fadein 0.2
	fadeout 0.2
	fps 30
}

$collisionmodel "w_iceaxe_physics.smd"
{
	$mass 8
	$inertia 1
	$damping 0
	$rotdamping 0
	$rootbone " "

}
