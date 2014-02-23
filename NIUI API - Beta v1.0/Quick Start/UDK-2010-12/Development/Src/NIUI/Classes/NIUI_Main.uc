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
*   NIUI_Main.uc
*
*   Copyright (c) 2011 Matthew Robbins
*
*   Author:  Matthew Robbins
*   Created: 01/2011
*
*   Desc: 
*   Sample file demonstrating how to use the NIUI_Core.
*/

class NIUI_Main extends FrameworkGame dependson(NIUI_Core);

// This is a test class to show NIUI working.
var NIUI_Core niui;
var string PlayerPawnArchetypeName;
var int imageMapHandle;
var int depthMapHandle;
var int colorMapHandle;

event PostBeginPlay()
{
	local Vector pos;

	// Create the core.
	niui = Spawn(class'NIUI_Core',,, pos);
	niui.Initilise();

	niui.SetDefaultUserColorMapPallete();
	niui.OpenPreviewWindow(640, 480);

	//niui.AddCameraInput(NIUI_CAMERA_FEED_ImageMap, imageMapHandle);
	//niui.SetCameraInputPosition(imageMapHandle, -150, 0);
	//niui.SetCameraInputScale(imageMapHandle, 1.0, 1.0);

	niui.AddCameraInput(NIUI_CAMERA_FEED_DepthMap, depthMapHandle);
	niui.SetCameraInputPosition(depthMapHandle, 0, 0);
	niui.SetCameraInputScale(depthMapHandle, 1.0f, 1.0f);

	niui.AddCameraInput(NIUI_CAMERA_FEED_UserColorMap, colorMapHandle);
	niui.SetCameraInputPosition(colorMapHandle, 0, 100);
	niui.SetCameraInputScale(colorMapHandle, 1.0f, 1.0f);

	niui.SetUserColor(1, 255, 0, 0);
	
	super.PostBeginPlay();
}

event Tick(float DeltaTime)
{
	super.Tick(DeltaTime);

	// Update the NIUI core.
	if (niui.ShouldNotifyDepencies())
	{
		niui.NotifyDependenciesOfCreation();
	}
	niui.UpdateNIUICore(DeltaTime);
}

event PostLogin(PlayerController NewPlayer)
{
	super.PostLogin(NewPlayer);
}

function RestartPlayer(Controller NewPlayer)
{
	super.RestartPlayer(NewPlayer);
}

function Pawn SpawnDefaultPawnFor(Controller NewPlayer, NavigationPoint StartSpot)
{
    local Rotator StartRotation;
    local Pawn ResultPawn;
    local Pawn PawnArchetypeToUse;

    // don't allow pawn to be spawned with any pitch or roll
    StartRotation.Yaw = StartSpot.Rotation.Yaw;
    StartRotation.Roll = 0;
	StartRotation.Pitch = 0;

    PawnArchetypeToUse = Pawn(DynamicLoadObject(PlayerPawnArchetypeName,class'NIUI_SamplePawn'));

    ResultPawn = Spawn(PawnArchetypeToUse.Class,,,StartSpot.Location,StartRotation,PawnArchetypeToUse,true);
    if ( ResultPawn == None )
    {
            `warn("n\n\n\nCouldn't spawn player of type "$PawnArchetypeToUse.Class$" at "$StartSpot$"n\n\n\n");
    }

    return ResultPawn;
}

event Destroyed()
{
}

DefaultProperties
{
	PlayerControllerClass=class'NIUI_SamplePlayerController'
	DefaultPawnClass=class'NIUI_SamplePawn'
	PlayerPawnArchetypeName="NIUI_Demo_Content.NIUI_Demo_Pawn"
	bDelayedStart = false;
}
