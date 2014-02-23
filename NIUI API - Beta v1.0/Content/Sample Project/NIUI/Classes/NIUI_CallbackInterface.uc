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
*   NIUI_CallbackInterface.uc
*
*   Copyright (c) 2011 Matthew Robbins
*
*   Author:  Matthew Robbins
*   Created: 01/2011
*
*   Desc: 
*   Implement this interface and register it with the NIUI core to enable this class
*   to recieve event notifications.
*   IMPORTANT: You must implement an int variable of some sort to enable the NIUI_Core
*   to identify it through the Set and Get NIUIHandle methods. I will find another method 
*   of doing this in future.
*/

interface NIUI_CallbackInterface;

/** 
 *  Event processing callback. Must be defined.
 *  */
function OnNIUIEvent(int TheEvent, int userID);

/** 
 *  Used when the interface is registered to recieve events. 
 *  This must be defined as it will allow the NIUI_Core to release it properly. 
 *  */
function SetNIUIHandle(int handle);

/** 
 *  This must be defined as it will allow the NIUI_Core to release it properly. 
 *  */
function int GetNIUIHandle();

DefaultProperties
{
}
