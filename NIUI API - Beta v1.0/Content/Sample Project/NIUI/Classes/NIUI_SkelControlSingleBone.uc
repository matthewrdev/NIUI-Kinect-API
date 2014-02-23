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
*   NIUI_SkelControlSingleBone.uc
*
*   Copyright (c) 2011 Matthew Robbins
*
*   Author:  Matthew Robbins
*   Created: 01/2011
*
*   Desc: 
*   A bone controller overriden to replicate the rotations of a bone of a user
*   picked up from a Natural Interaction device.
*/

class NIUI_SkelControlSingleBone extends SkelControlSingleBone;

// Precached reference to the NIUI_Core.
var private NIUI_Core niuiCore;

// The OpenNI bone to inherit rotations from.
var(NIUI) NIUI_BoneID TargetBone;

// The name of the bone we are overriding.
var(NIUI) name TargetBoneName;

// The rotation the skeletal control node is to treat as the reference pose.
var private Quat qBoneRefRot;

// The userID for the OpenNI user we are using for overriding the bone rotations.
var private int UserID;

// If the control has been allocated a target NIUI user and thus can override rotations.
var private bool Bound; 

// Precached conversion of the target bone enum to an int to save on conversions.
var private int intTargetBone;

/** Used to cache the rotation to consider as the reference pose from a T-Pose animation. */
function CacheInitialRotation(SkeletalMeshComponent SkelComp, Quat qOwnerRotInv)
{
	local Quat qBone;

	qBone = SkelComp.GetBoneQuaternion(TargetBoneName, 0);

	// Find bones rotation we are going to treat as the reference pose.
	qBoneRefRot = QuatProduct(qOwnerRotInv, qBone);
}

/** 
 *  DO NOT CALL THIS YOURSELF.
 *  It is only for use by the NIUI_Core.
 *  
 *  Presets the rotation to treat as the reference pose. */  
function AssignReferenceRotation(Quat qRefRot)
{
	qBoneRefRot = qRefRot;
}

function Bind(NIUI_Core core, int user)
{
	niuiCore = core;
	UserID = user;
	Bound = true;
	intTargetBone = int(TargetBone);

	// Transformation is done in world space, however we apply the OpenNI bone rotation in local space and
	// then transform into world for the final rotation.
	BoneRotationSpace = BCS_WorldSpace;
}

/** Applies the bone rotations of the linked NIUI user to the specified target bone. */
function UpdateBoneOverride(float DeltaTime,Quat qOwner)
{
	local Quat qRot;

	if (Bound)
	{
		// Apply rotation but don't add it to the animation rotation.
		bApplyRotation = true;
		bAddRotation = false;

		// Get the bones rotation.
		qRot = QuatFromRotator(niuiCore.GetBoneRotation(UserID, intTargetBone));   

		// Transform the cached delta by the OpenNI rotation.
		qROt = QuatProduct(qRot, self.qBoneRefRot);

		// Transform into world space based on the owner's orientation.
		BoneRotation = QuatToRotator(QuatProduct(qOwner, qRot));
	}
}

/** Gets the rotation that is currently being considered as the reference pose. */
function Quat GetReferenceRotation()
{
	return qBoneRefRot;
}

/** Gets the bone that is being used to override the rotations on the mesh. */
function NIUI_BoneID GetTargetBone()
{
	return self.TargetBone;
}

DefaultProperties
{
	Bound = false; // Is not bound by default.
}
