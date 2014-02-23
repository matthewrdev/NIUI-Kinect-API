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
*   NIUI_User.uc
*
*   Copyright (c) 2011 Matthew Robbins
*
*   Author:  Matthew Robbins
*   Created: 01/2011
*
*   Desc: 
*   NIUI_User is used to reflect the user state.
*   This is an upcoming feature to remove the need for so many interface calls to the NIUI.dll
*/

class NIUI_User extends Actor implements (NIUI_DependencyInterface);

struct NIUI_InternalUserMirror
{
	var Rotator JointRotations[24];
	var Vector WorldJointLocations[24];
	var Vector LocalJointLocations[24]; // Joint locations relative to the user centre.
	var Vector UserCentre;
	var int detected;
	var int lost;
	var int calibrated;
	var int tracking;
};

var int userID;

var private NIUI_InternalUserMirror mirror;

function Reflect(NIUI_Core core)
{

}

function OnNIUICoreStart(NIUI_Core TheCore)
{
}

function OnNIUICoreShutdown()
{
}

DefaultProperties
{
}
