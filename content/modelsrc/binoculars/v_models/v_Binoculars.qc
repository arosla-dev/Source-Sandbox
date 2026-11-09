// Created by Crowbar 0.74

$modelname "weapons/v_Binoculars.mdl"

$bodygroup "studio"
{
	studio "Binoculars_reference.smd"
}


$contents "solid"

$illumposition -4.655 -18.527 -12.162
$cdmaterials "models\Weapons\V_binoculars\"

$origin -3.5 0 0

$cbox 0 0 0 0 0 0

$bbox 10.461 -10.67 -21.4 26.594 1.359 -2.924

$definebone "Bip01" "" 0.134137 21.745049 43.181469 0 -89.999927 0 0 0 0 0 0 0
$definebone "Bip01 Pelvis" "Bip01" 0 0 0 -89.980215 -89.999927 0 0 0 0 0 0 0
$definebone "Bip01 Spine" "Bip01 Pelvis" 4.443615 -0.00964 0.000006 -0.000057 0.045607 -0.000229 0 0 0 0 0 0
$definebone "Bip01 Neck" "Bip01 Spine" 12.111413 -0.011447 0 0 0 0 0 0 0 0 0 0
$definebone "Bip01 R Clavicle" "Bip01 Neck" 0.000009 0.011449 -1.352403 78.540823 179.953614 -0.000802 0 0 0 0 0 0
$definebone "Bip01 R UpperArm" "Bip01 R Clavicle" 6.078088 0.000001 0.000002 -2.569601 -88.156548 72.743762 0 0 0 0 0 0
$definebone "Bip01 R Forearm" "Bip01 R UpperArm" 13.446755 -0.000001 0 0 -3.959138 -0.000002 0 0 0 0 0 0
$definebone "Bip01 R Hand" "Bip01 R Forearm" 15.997 -0.000001 0 0 0 96.454354 0 0 0 0 0 0
$definebone "Bip01 R Finger0" "Bip01 R Hand" 1.885327 0.833277 2.177879 -40.000002 -0.038272 91.559587 0 0 0 0 0 0
$definebone "Bip01 R Finger01" "Bip01 R Finger0" 1.506962 0 0 0.000001 -7.999994 -0.000001 0 0 0 0 0 0
$definebone "Bip01 R Finger02" "Bip01 R Finger01" 0.798349 -0.000006 0 0.000001 -6.49999 -0.000001 0 0 0 0 0 0
$definebone "Bip01 R Finger1" "Bip01 R Hand" 3.950304 -0.539875 2.108873 -9.000016 -0.007219 0.046182 0 0 0 0 0 0
$definebone "Bip01 R Finger11" "Bip01 R Finger1" 1.545798 0 0 0.000003 26.999999 0.000003 0 0 0 0 0 0
$definebone "Bip01 R Finger12" "Bip01 R Finger11" 1.667398 -0.000002 0.000001 0.000001 24.999973 0.000005 0 0 0 0 0 0
$definebone "Bip01 R Finger2" "Bip01 R Hand" 3.899461 -0.475376 1.170894 0 0 0.045609 0 0 0 0 0 0
$definebone "Bip01 R Finger21" "Bip01 R Finger2" 1.528215 0 -0.000001 0 19.000024 0.000001 0 0 0 0 0 0
$definebone "Bip01 R Finger22" "Bip01 R Finger21" 1.528216 0.000001 -0.000005 -0.000001 27.499965 0.000001 0 0 0 0 0 0
$definebone "Bip01 R Finger3" "Bip01 R Hand" 3.899461 -0.431804 -0.168222 0 0 0.045609 0 0 0 0 0 0
$definebone "Bip01 R Finger31" "Bip01 R Finger3" 1.506964 0 0 0 24.500015 0.000001 0 0 0 0 0 0
$definebone "Bip01 R Finger32" "Bip01 R Finger31" 1.506962 0 0.000006 -0.000001 19.499985 0.000001 0 0 0 0 0 0
$definebone "Bip01 R Finger4" "Bip01 R Hand" 3.915634 -0.278019 -1.402606 0 0 0.045609 0 0 0 0 0 0
$definebone "Bip01 R Finger41" "Bip01 R Finger4" 1.333083 0 0.000003 -0.000001 36.499981 0.000001 0 0 0 0 0 0
$definebone "Bip01 R Finger42" "Bip01 R Finger41" 1.159203 0 0.000002 -0.000001 12.499986 0.000001 0 0 0 0 0 0
$definebone "Dummy14" "Bip01 R Hand" 14.354309 8.018459 5.505768 -1.064753 -179.115414 90.655885 0 0 0 0 0 0

$animation "a_Binoculars_idle01" "v_Binoculars_anims\Binoculars_idle01.smd" {
	fps 30
	loop
}

$animation "a_Binoculars_draw" "v_Binoculars_anims\Binoculars_draw.smd" {
	fps 30
}

$sequence "Binoculars_idle01" {
	"a_Binoculars_idle01"
	activity "ACT_VM_IDLE" 1
	fadein 0.2
	fadeout 0.2
}

$sequence "Binoculars_draw" {
	"a_Binoculars_draw"
	activity "ACT_VM_DRAW" 1
	fadein 0.2
	fadeout 0.2
}

