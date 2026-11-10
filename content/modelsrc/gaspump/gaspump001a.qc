// Created by Crowbar 0.74

$modelname "gaspump.mdl"

$staticprop

$bodygroup "Body"
{
	studio "gaspump001a_reference.smd"
}

$lod 20
{
	replacemodel "gaspump001a_reference.smd" "gaspump001a_reference_lod1.smd"
	nofacial
}

$surfaceprop "metal"

$contents "solid"

$illumposition 0 0 31.95

$cdmaterials "models\gaspump\"

$cbox 0 0 0 0 0 0

$bbox -20.78 -22.202 -0.243 20.78 22.202 64.324

$definebone "static_prop" "" 0 0 0 0 0 0 0 0 0 0 0 0


$sequence "idle" {
	"gaspump001a_anims\idle.smd"
	fadein 0.2
	fadeout 0.2
	fps 30
}

$collisionmodel "gaspump001a_physics.smd"
{
	$mass 57.91835
	$inertia 1
	$damping 0
	$rotdamping 0
	$rootbone " "
	$concave
	$maxconvexpieces 5

}
