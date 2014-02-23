/*****************************************************************************
*                                                                            *
*  Natural Interaction Unreal Interface (NIUI) Alpha                         *
*  Copyright (C) 2011 Matthew Robbins                                        *
*                                                                            *
*  This file is part of NIUI.                                                *
*                                                                            *
*  NIUI is free software: you can redistribute it and/or modify              *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  NIUI is distributed in the hope that it will be useful,                   *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with NIUI. If not, see <http://www.gnu.org/licenses/>.              *
*                                                                            *
*****************************************************************************/

/**
*
*   NIUI_SamplePawn.uc
*
*   Copyright (c) 2011 Matthew Robbins
*
*   Author:  Matthew Robbins
*   Created: 01/2011
*
*   Desc: 
*   Demonstrates how to create and configure the NIUI_SkeletalController.
*   
*/

class NIUI_SamplePawn extends UDKPawn implements (NIUI_DependencyInterface);

var private NIUI_SkeletalController NIUISkelControl;
var NIUI_Core niuiCOre;

// The bones that are going to be applying the OpenNI bone rotations to. 
// This should be a list of names matching the names of the NIUI_SkelControlSingleBone anim nodes in your anim-tree.
var(NIUI) array<name> TargetBoneNames;

// Our current userID.
var int userID;

// ------------------------------------------------------------------------------------
// CUSTOM ANIMATION PLAYING

// The blendlist used for swapping between standard blending and crossfade blending.
var private AnimNodeBlendList AnimBlendList; 

// Our animation blending node for blending custom animations.
var private AnimNodeBlend AnimationBlender;

// THe physics branch of the animation tree.
var private AnimNodeBlendByPhysics PhysicsNode;

// OUr 2 custom animation nodes.
var private array<AnimNodePlayCustomAnim> CustomAnimNodes;
var private int CustomAnimationBlendlistIndex;
var private int LastCustomAnimNodePlayIndex;
// ------------------------------------------------------------------------------------


function OnNIUICoreStart(NIUI_Core TheCOre)
{
}

function InitiliseNIUISkeletalControls(int NIUI_UserID, NIUI_Core core)
{
	niuiCOre = core;
	userID = NIUI_UserID;

	if (NIUISkelControl.IsInitilisedFromRRF() == false)
	{
		NIUISkelControl.CacheRotations(Mesh, self.Rotation);

		// Export rotations.
		core.ExportReferenceRotations(Mesh, NIUISkelControl);

		StopTPose();
	}

	NIUISkelControl.Bind(core, NIUI_UserID);
}

simulated function PostInitAnimTree(SkeletalMeshComponent SkelComp)
{
	super.PostInitAnimTree(SkelComp);

	NIUISkelControl = Spawn(class'NIUI_SkeletalController',,, Location);
	NIUISkelControl.CacheSkeletonControls(SkelComp, TargetBoneNames);

	`log(SkelComp.Name);
	`log(string(SkelComp.SkeletalMesh.Name));

		//assign our animation nodes so we can access them when we want to play an animation
	//requires them being set in an animation tree that is assigned to the pawn
	AnimBlendList = AnimNodeBlendList(Mesh.FindAnimNode('BlendList'));
	PhysicsNode = AnimNodeBlendByPhysics(Mesh.FindAnimNode('PhysicsNode'));

	// FInd the base animation blender and the crossfade blender.
	AnimationBlender = AnimNodeBlend(AnimBlendList.FindAnimNode('CustomAnimationBlender'));

	// Find the custom animation players for the base animation blender.
	CustomAnimNodes[0] = AnimNodePlayCustomAnim(AnimationBlender.FindAnimNode('BlendNode_1'));
	CustomAnimNodes[1] = AnimNodePlayCustomAnim(AnimationBlender.FindAnimNode('BlendNode_2'));
}

function UpdateSkeletalControls(float DeltaTime)
{
	niuiSkelCOntrol.UpdateBoneOverides(DeltaTIme, self.Rotation);
}

function OnNIUICoreShutdown()
{
}

/** 
 *  Custom animation playing function derived from Dungeon Defense source code.
 */
function float PlayAnim(name AnimName, optional float blendTimeIn=0.2, optional bool looping=false, optional float speed = 1.0f, optional float BlendOutTime=0.2, optional bool bOverride=false)
{
	local float value;

	LastCustomAnimNodePlayIndex = (LastCustomAnimNodePlayIndex + 1)%CustomAnimNodes.Length;
	AnimBlendList.SetActiveChild(1, blendTimeIn);
	AnimationBlender.SetBlendTarget(LastCustomAnimNodePlayIndex,blendTimeIn);
	CustomAnimNodes[LastCustomAnimNodePlayIndex].PlayCustomAnim(AnimName,speed,blendTimeIn,BlendOutTime,looping,false);
	value = CustomAnimNodes[LastCustomAnimNodePlayIndex].GetCustomAnimNodeSeq().GetAnimPlaybackLength();

	return value;
}

function OnCalibrationStart(NIUI_Core core)
{
	// Try load a reference rotation file to remove the need for t-pose.
	if (core.PreloadReferenceRotations(Mesh, NIUISkelControl) == false)
	{
		ForceTPose();
	}
}

function ForceTPose()
{
	PlayAnim('TPose', 0.0f, true);
}

// Restore from tpose.
function StopTPose()
{
	AnimBlendList.SetActiveChild(0, 0.3f);
}

DefaultProperties
{
        Components.Remove(Sprite)
        //Setting up the light environment

    Begin Object Class=DynamicLightEnvironmentComponent Name=MyLightEnvironment

                //amount of time before a shadow will fade after being cast from a light source
        ModShadowFadeoutTime=0.25

                //minimum amount of time before a full update of the enivornment is done
        MinTimeBetweenFullUpdates=0.2

                //ambient glow added to the leve lighting
        AmbientGlow=(R=.01,G=.01,B=.01,A=1)

                //ambient colour of the shadow
        AmbientShadowColor=(R=0.15,G=0.15,B=0.15)
                //Type of shadows used

        LightShadowMode=LightShadow_ModulateBetter
                //Quality of the shadow

                ShadowFilterQuality=SFQ_High

                //not entierly sure, looks like a different method for generating shadows, higher quality but more performace required
        bSynthesizeSHLight=TRUE

    End Object
     //add our light environment to the components list
    Components.Add(MyLightEnvironment)

    //Set up the mesh and animset components
    Begin Object Class=SkeletalMeshComponent Name=InitialSkeletalMesh

            //wether to cast a shadow or not
            CastShadow = true;

            //wether to cast a dynamic shadow or not
            bCastDynamicShadow = true;

            //if the view actor is the same as the owner of this component it won't be visible
            bOwnerNoSee=false;

            //the light environment this pawn will use (using the environment we setup before)
            LightEnvironment = MyLightEnvironment;

            //wether collide against ridged bodies
            BlockRigidBody=true;

            //wether we collide with other actors
            CollideActors=true;

            //unsure - look this up
            BlockZeroExtent=true;
                
		//what to change if you want to use own meshes and animation sets

		//the physics we will be using for this pawn
		PhysicsAsset=PhysicsAsset'UDK_ProtoRig_03.UDK_Proto_03_Physics'
				//the animation sets we will use for this pawn

		AnimSets(0)=AnimSet'UDK_ProtoRig_03.UDK_Anim_03'
                
		AnimTreeTemplate=AnimTree'UDK_ProtoRig_03.Proto_AnimTree_02'

				//finally the mesh we are using for the pawn
		SkeletalMesh=SkeletalMesh'UDK_ProtoRig_03.UDK_Proto_03'

    End Object

	//Set our mesh to the one we just created...
	Mesh=InitialSkeletalMesh;
	//...and add it to our component list
	Components.Add(InitialSkeletalMesh); 

	//then setup our collision for this pawn (pretty self explanitory this next section i think
	//as is most of this stuff with helpful tool tips on mouse hover)
	CollisionType=COLLIDE_BlockAll
	Begin Object Name=CollisionCylinder
		CollisionRadius=+035
		CollisionHeight=+057.000000
	End Object
	CylinderComponent=CollisionCylinder             

	userID = 1
}
