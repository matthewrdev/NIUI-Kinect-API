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
*   NIUI_SamplePlayerController.uc
*
*   Copyright (c) 2011 Matthew Robbins
*
*   Author:  Matthew Robbins
*   Created: 01/2011
*
*   Desc: 
*   Sample player controller showing how to listen for NIUI events, respond to thos events
*   and update the bone rotations.
*/

class NIUI_SamplePlayerController extends GamePlayerController 
	implements (NIUI_CallbackInterface, NIUI_DependencyInterface) 
	dependson (NIUI_Core);

var int NIUI_UserID;
var int NIUICallbackHandle;
var NIUI_Core niuiCOre;
var bool coreStarted;
var bool isTracking;
var NIUI_SamplePawn MyPawn;
var SimpleHUD theHud;

simulated event PostBeginPlay()
{
	theHud = new class'SimpleHud';
}

function OnNIUIEvent(int TheEvent, int userID)
{
	local NIUI_Core core;
	core = GetCore();

	if (TheEvent == NIUI_EVENT_UserDetected)
	{
		if (NIUI_UserID == -1)
		{
			NIUI_UserID = userID;
			core.ActivateUser(NIUI_UserID);
			core.StartPoseDetection(NIUI_UserID);
			theHud.UserState = "User Detected";
		}
	}
	else if (TheEvent == NIUI_EVENT_CalibrationSuccess && userID == NIUI_UserID)
	{
		NIUI_SamplePawn(Pawn).InitiliseNIUISkeletalControls(NIUI_UserID, core);
	}
	else if (TheEvent == NIUI_EVENT_UserPoseDetected && userID == NIUI_UserID)
	{
		core.StartCalibration(NIUI_UserID);
		theHud.UserState = "Pose Detected";
	}
	else if (TheEvent == NIUI_EVENT_CalibrationStart && userID == NIUI_UserID)
	{
		MyPawn.OnCalibrationStart(core);
		theHud.UserState = "Calibrating";
	}
	else if (TheEvent == NIUI_EVENT_CalibrationFail && userID == NIUI_UserID)
	{
		niuiCore.StartPoseDetection(NIUI_UserID);
		theHud.UserState = "Pose Lost";
	}
	else if (TheEvent == NIUI_EVENT_TrackingStart && userID == NIUI_UserID)
	{
		isTracking = true;
		MyPawn.InitiliseNIUISkeletalControls(NIUI_UserID, core);
		theHud.UserState = "Tracking";
	}
}

function OnNIUICoreStart(NIUI_Core THeCore)
{
	coreStarted = true;
	niuiCOre = THeCore;

	MyPawn = NIUI_SamplePawn(Pawn);
	
	THeCore.RegisterEventListener(self);
}

/** Used when the interface is registered to recieve events. 
 *  This must be defined as it will allow the NIUI_Core to release it properly. */
function SetNIUIHandle(int handle)
{
	NIUICallbackHandle = handle;
}

/** This must be defined as it will allow the NIUI_Core to release it properly. */
function int GetNIUIHandle()
{
	return NIUICallbackHandle;
}

function NIUI_Core GetCore()
{
	if (niuiCOre == none)
	{
		niuiCOre = NIUI_Main(self.WorldInfo.Game).niui;
	}

	return niuiCOre;
}

function PlayerTick(float DeltaTime)
{
	super.PlayerTick(DeltaTime);

	DrawNIUISkeleton();

	if (niuiCOre != none && isTracking)
	{
		MyPawn.UpdateSkeletalControls(DeltaTime);
	}
}

function DrawNIUISkeleton()
{
	DrawInputSkeleton(NIUI_UserID);
}

function DrawHUD(HUD H)
{
	super.DrawHUD(H);

	if (theHud != none)
	{
		theHud.DrawMyHud(H);
	}
}

function DrawInputSkeleton(int userID)
{
	local Vector position;

	if (coreStarted == false)
		return;
	
	if (isTracking)
	{
		position = MyPawn.Location + (vector(MyPawn.Rotation) * 100.0f);
		position.Z += 130.0f;
		DrawBone(userID, NIUI_JOINT_HEAD, NIUI_JOINT_NECK, position);
		DrawBone(userID, NIUI_JOINT_NECK, NIUI_JOINT_RIGHT_SHOULDER, position);
		DrawBone(userID, NIUI_JOINT_NECK, NIUI_JOINT_LEFT_SHOULDER, position);
		DrawBone(userID, NIUI_JOINT_TORSO, NIUI_JOINT_LEFT_HIP,position);
		DrawBone(userID, NIUI_JOINT_TORSO, NIUI_JOINT_RIGHT_HIP, position);
		DrawBone(userID, NIUI_JOINT_LEFT_HIP, NIUI_JOINT_LEFT_KNEE, position);
		DrawBone(userID, NIUI_JOINT_LEFT_KNEE, NIUI_JOINT_LEFT_FOOT, position);
		DrawBone(userID, NIUI_JOINT_RIGHT_HIP, NIUI_JOINT_RIGHT_KNEE, position);
		DrawBone(userID, NIUI_JOINT_RIGHT_KNEE, NIUI_JOINT_RIGHT_FOOT, position);
		DrawBone(userID, NIUI_JOINT_TORSO, NIUI_JOINT_LEFT_SHOULDER, position);
		DrawBone(userID, NIUI_JOINT_RIGHT_SHOULDER, NIUI_JOINT_TORSO, position);
		DrawBone(userID, NIUI_JOINT_LEFT_SHOULDER, NIUI_JOINT_LEFT_ELBOW, position);
		DrawBone(userID, NIUI_JOINT_LEFT_ELBOW, NIUI_JOINT_LEFT_HAND, position);
		DrawBone(userID, NIUI_JOINT_RIGHT_SHOULDER, NIUI_JOINT_RIGHT_ELBOW, position);
		DrawBone(userID, NIUI_JOINT_RIGHT_ELBOW, NIUI_JOINT_RIGHT_HAND, position);
	}
}

function DrawBone( int userID,int startBone, int endBone,Vector position)
{
	local Vector start, end;
	local Rotator startRot, endRot;

	GetCore().GetLocalSkeletalJointLocation(userID, startBone, start);
	GetCore().GetLocalSkeletalJointLocation(userID, endBone, end);
	startRot = GetCore().GetBoneRotation(userID, startBone);
	endRot = GetCore().GetBoneRotation(userID, endBone);

	// Multiply by 3 as other wise the transformed positions will be too small.
	DrawDebugLine(position + start * 3, position + end* 3, 255, 255, 255, false);
	DrawDebugSphere(position + start* 3, 2.0f, 5, 122, 122, 122, false);
	DrawDebugSphere(position + end* 3, 2.0f, 5, 122, 122, 122, false);

	
	DrawDebugCoordinateSystem(position + start* 3, startRot, 15.0f, false);
	DrawDebugCoordinateSystem(position + end* 3, endRot, 15.0f, false);
}

function OnNIUICoreShutdown()
{
	// Do something to inform class the NIUI_Core isnt safe to use.
}

DefaultProperties
{
	NIUI_UserID = -1; // Invalid user.
	coreStarted = false;
	isTracking = false;
	CameraClass=class'NIUI.SimpleCamera'
}
