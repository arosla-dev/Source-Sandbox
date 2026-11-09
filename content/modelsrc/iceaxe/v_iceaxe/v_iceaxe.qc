// Created by Crowbar 0.74

$modelname "Weapons/v_iceaxe.mdl"

$bodygroup "studio"
{
	studio "iceaxe_reference.smd"
}


$surfaceprop "default"

$contents "solid"

$illumposition -8.427 -25.531 -14.879

$cdmaterials "models\Weapons\V_hand\"
$cdmaterials "models\Weapons\V_iceaxe\"

$attachment "0" "ValveBiped.Tip" 0 0 0 rotate 0 0 0

$cbox 0 0 0 0 0 0

$bbox 7.416 -16.855 -29.795 43.647 0 0.037

$definebone "ValveBiped.Bip01" "" -0.000005 -28.696043 38.566917 0 0 89.999982 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_Spine" "ValveBiped.Bip01" 0.000005 3.345127 -2.981897 4.944225 89.999982 89.999982 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_Spine1" "ValveBiped.Bip01_Spine" 4.018333 -0.000004 0 0 -1.675443 0 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_Spine2" "ValveBiped.Bip01_Spine1" 3.518562 0.000004 0 0 5.74883 0 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_Spine4" "ValveBiped.Bip01_Spine2" 8.942642 0 0 0 11.120882 0 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Clavicle" "ValveBiped.Bip01_Spine4" 2.033344 1.000772 -1.93761 73.71203 168.618779 -95.596496 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_UpperArm" "ValveBiped.Bip01_R_Clavicle" 6.028147 0.000011 0 78.953592 -88.125723 91.732603 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Forearm" "ValveBiped.Bip01_R_UpperArm" 11.692556 0.000004 0.000001 0 -18.575693 -0.000003 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Hand" "ValveBiped.Bip01_R_Forearm" 11.481709 -0.000004 0 0.513542 -3.12594 -105.465149 0 0 0 0 0 0
$definebone "ValveBiped.HandControlRotGrandmaR" "" -6.855245 -37.470791 55.472576 -2.905985 -90.838736 -73.89305 0 0 0 0 0 0
$definebone "ValveBiped.HandControlRotParentR" "ValveBiped.HandControlRotGrandmaR" 11.481693 0.000011 -0.000002 6.108074 2.532072 -89.66423 0 0 0 0 0 0
$definebone "ValveBiped.HandControlRotR" "ValveBiped.HandControlRotParentR" 0 -0.000001 0 5.625013 -5.625014 -15.749977 0 0 0 0 0 0
$definebone "ValveBiped.Tip" "ValveBiped.HandControlRotR" 11.123104 -1.935456 -15.150154 -0.537146 -92.19487 -39.209626 0 0 0 0 0 0


$sequence "idle01" {
	"v_iceaxe_anims\idle01.smd"
	activity "ACT_VM_IDLE" 1
	fadein 0.2
	fadeout 0.2
	fps 30
	loop
}

$sequence "draw" {
	"v_iceaxe_anims\draw.smd"
	activity "ACT_VM_DRAW" 1
	fadein 0.2
	fadeout 0.2
	snap
	fps 30
}

$sequence "misscenter1" {
	"v_iceaxe_anims\misscenter1.smd"
	activity "ACT_VM_MISSCENTER" 1
	fadein 0.2
	fadeout 0.2
	snap
	fps 30
}

$sequence "misscenter2" {
	"v_iceaxe_anims\misscenter2.smd"
	activity "ACT_VM_MISSCENTER" 1
	fadein 0.2
	fadeout 0.2
	snap
	fps 30
}

$sequence "hitcenter1" {
	"v_iceaxe_anims\hitcenter1.smd"
	activity "ACT_VM_HITCENTER" 1
	fadein 0.2
	fadeout 0.2
	snap
	fps 30
}

$sequence "hitcenter2" {
	"v_iceaxe_anims\hitcenter2.smd"
	activity "ACT_VM_HITCENTER" 1
	fadein 0.2
	fadeout 0.2
	snap
	fps 30
}

$sequence "hitcenter3" {
	"v_iceaxe_anims\hitcenter3.smd"
	activity "ACT_VM_HITCENTER" 1
	fadein 0.2
	fadeout 0.2
	snap
	fps 30
}

$sequence "hitkill1" {
	"v_iceaxe_anims\hitkill1.smd"
	activity "ACT_VM_HITKILL" 1
	fadein 0.2
	fadeout 0.2
	snap
	fps 30
}

$sequence "holster" {
	"v_iceaxe_anims\holster.smd"
	activity "ACT_VM_HOLSTER" 1
	fadein 0.2
	fadeout 0.2
	fps 30
}
