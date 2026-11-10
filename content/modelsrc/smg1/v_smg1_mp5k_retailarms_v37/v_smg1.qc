/*
==============================================================================

SMG1 - Sub Machine Gun 1
MP5k with 'retail arms'
Set up for Leak (Version 37)
Modeled and compiled by Jasper
1/11/2017

==============================================================================
*/

/*
$origin 0 0 68
*/

$origin 0 0 0 90
 
$modelname "weapons/v_smg1.mdl"
$cdmaterials "models/Weapons/V_smg1/"

$body "studio" "v_smg1_reference.smd"

$illumposition 19.66 -2.06 -11.12

$surfaceprop "default"

$contents solid

$bbox 5.15 -10.29 -20.15 34.17 6.17 -2.08
$hboxset "default"
$hbox 0 "ValveBiped.Bip01_Spine2"    0.00   -0.00   -8.40    11.64   -0.00    0.00
$hbox 0 "ValveBiped.base"   -3.79   -6.41  -15.49     1.69   -6.41   13.89
/*
StudioMDL reports this bone is missing for some reason?
$hbox 0 "ValveBiped.handle"   -0.57   -5.08   -0.80     0.47   -5.08    0.83
*/

$attachment "0" "ValveBiped.muzzle" -8.00 0.00 0.00 rotate 0.00 -0.00 0.00

$attachment "1" "ValveBiped.eject" -0.00 -0.00 0.00 rotate 0.00 0.00 0.00


$sequence "idle01" "idle01.smd" fps 30.00 loop ACT_VM_IDLE 1 {

}

$sequence "fire01" "fire01.smd" fps 30.00 snap ACT_VM_PRIMARYATTACK 1 {
	{ event 5001 0.00 "2" }
	{ event 6001 0.00 "0" }
}

/*
Randomized fire anims

$sequence "fire02" "fire02.smd" fps 30.00 snap ACT_VM_PRIMARYATTACK 2  {
	{ event 5001 0.00 "2" }
	{ event 6001 0.00 "0" }

}

$sequence "fire03" "fire03.smd" fps 30.00 snap ACT_VM_PRIMARYATTACK 3  {
	{ event 5001 0.00 "2" }
	{ event 6001 0.00 "0" }

}

$sequence "fire04" "fire04.smd" fps 30.00 snap ACT_VM_PRIMARYATTACK 4  {
	{ event 5001 0.00 "2" }
	{ event 6001 0.00 "0" }

}
*/

$sequence "altfire" "altfire.smd" fps 30.00 snap ACT_VM_SECONDARYATTACK 1  {

}

$sequence "draw" "draw.smd" fps 30.00 snap ACT_VM_DRAW 1  {

}

$sequence "reload" "reload.smd" fps 30.00 ACT_VM_RELOAD 1  {
  { event AE_CL_PLAYSOUND 1 "Weapon_Smg1.Reload" }
  { event 5004 1 "Weapon_Smg1.Reload" }
}

$sequence nothreatidle "lowidle" loop fps 30 
$sequence nothreattoidle "lowtoidle" fps 30
$sequence idletonothreat "idletolow" fps 30

$sequence "dryfire" "dryfire.smd" fps 30.00 snap ACT_VM_DRYFIRE 1  {

}