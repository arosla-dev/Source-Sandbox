//
// .qc file version 1.0


$modelname gaspump.mdl
$CDMaterials models\gaspump\

$scale 1.0
$body "Body" "gaspump001a_reference.smd" 
$staticprop
$surfaceprop "metal"


// --------- Animation sequences -------
$sequence  "idle" "gaspump001a_reference.smd" fps 30

$attachment "playercable" "polymsh" -18 0 55.829

$lod 20
{
	replacemodel "gaspump001a_reference.smd" "gaspump001a_lod1"
}


$collisionmodel "gaspump001a_physbox.smd" {
	// Mass in kilograms
	$automass
	$concave

}



