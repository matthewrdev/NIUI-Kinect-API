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
*   NIUI_SkeletalController.uc
*
*   Copyright (c) 2011 Matthew Robbins
*
*   Author:  Matthew Robbins
*   Created: 01/2011
*
*   Desc: 
*   Overrides a skeleton to replicate the rotations of a user picked up from a 
*   Natural Interaction device.
*   Spawn one of these within your pawn class and use the Bind() function to 
*   make the bones access the rotations of the target NIUI user.
*   You also need to have a T-Pose animation so that the rotation overiding will 
*   work.
*/

class NIUI_SkeletalController extends Actor;

// The user that the skeletal controller is to target.
var() int NIUI_UserID;

// Cached reference's to the bone controllers targeting the owner pawns skeletal mesh.
// These shouldn't need to be accessed within your code.
var array<NIUI_SkelControlSingleBone> BoneControllers;

// If the skeletal controller should track its linked user and inherit bone rotations.
var private bool TrackSkeleton;

// Precached reference to the NIUI_Core.
var NIUI_Core theCore;

// If the linked skeletal mesh has a .rrf file associated with it.
var private bool HasReferenceRotationFile;

/** 
 *  Caches all NIUI bone controllers from the target anim tree. 
 *  */
function CacheSkeletonControls(SkeletalMeshComponent SkelComp, array<name> TargetNodeNames)
{
	local int iter;
	local NIUI_SkelControlSingleBone BoneControl;

	// Cache all NIUI_SkelControlSingleBone
	for (iter = 0; iter < TargetNodeNames.Length; iter++)
	{
		BoneControl = NIUI_SkelControlSingleBone(SkelComp.FindSkelControl(TargetNodeNames[iter]));
		if (BoneControl != none)
		{
			// Add the bone controller.
			BoneControllers.AddItem(BoneControl);
			`log(BoneControl @"targeting joint:" @BoneControl.GetTargetBone());
		}
	}
}

/** 
 *  Takes the userID and makes all the cached bones start inheriting the rotations
 *  of the linked bones. 
 *  */
function Bind(NIUI_Core core, int userID)
{
	local int iter;

	theCore = core;

	// Set the target ID and bind the bones to the user.
	NIUI_UserID = userID;

	for (iter = 0; iter < BoneControllers.Length; iter++)
	{
		BoneControllers[iter].Bind(core, userID);
	}
}

/**
 * Takes owners skeleton in T-Pose and then calculates the quaternions to treat as 
 * the reference pose. 
 *  */
function CacheRotations(SkeletalMeshComponent SkelComp, Rotator OwnerRotation)
{
	local int iter;
	local Quat qOwnerInv;

	qOwnerInv = QuatFromRotator(OwnerRotation);
	qOwnerInv = QuatInvert(qOwnerInv);

	for (iter = 0; iter < BoneControllers.Length; iter++)
	{
		BoneControllers[iter].CacheInitialRotation(SkelComp, qOwnerInv);
	}
}

/** 
 *  DO NOT CALL THIS YOURSELF.
 *  It is only for use by the NIUI_Core.
 *  
 *  Assigns the reference rotation to the bone.  
 *  */
function AssignReferenceRotation(NIUI_BoneID bone, Quat qRefRot)
{
	local int iter;

	// Find the linked bone from the precached bone controllers.
	for (iter = 0; iter < self.BoneControllers.Length; iter++)
	{
		if (BoneControllers[iter].GetTargetBone() == bone)
		{
			BoneControllers[iter].AssignReferenceRotation(qRefRot);
			break;
		}
	}
}


/** 
 *  DO NOT CALL THIS YOURSELF.
 *  It is only for use by the NIUI_Core. 
 *  
 *  Sets the Skeleton controller flagged as pre-initilised by an .rrf file. 
 *  This means all quaternion reference rotations for each bone is preloaded 
 *  and removes the need to force T-Pose.
 *  */
function SetInitilisedFromRRF()
{
	HasReferenceRotationFile = true;
}

/** 
 *  Enforces the bone rotation tracking. 
 *  */
function UpdateBoneOverides(float DeltaTime, Rotator OwnerRot)
{
	local int iter;
	local Quat qOwner;

	qOwner = QuatFromROtator(OwnerRot);

	for (iter = 0; iter < BoneControllers.Length; iter++)
	{
		BoneControllers[iter].UpdateBoneOverride(DeltaTime, qOwner);
	}
}

/** 
 *  Returns if the skeletal controller has already been initilised by a .rrf file
 *  and therefore does not need the T-Pose animation. 
 *  */
function bool IsInitilisedFromRRF()
{
	return HasReferenceRotationFile;
}

DefaultProperties
{
	HasReferenceRotationFile = false;
}
