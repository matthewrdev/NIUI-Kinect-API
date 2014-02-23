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
*   NIUI_DependencyInterface.uc
*
*   Copyright (c) 2011 Matthew Robbins
*
*   Author:  Matthew Robbins
*   Created: 01/2011
*
*   Desc: 
*   Provides an easy way for objects that depend on the NIUI_Core to cache a
*   reference to the core without having to get it through the game info class. 
*   See interface function descriptions below for an idea of how the interface works.
*/

interface NIUI_DependencyInterface;

/**
 *  This is called on the first tick of the NIUI_Core object. If you have anything that is dependent on 
 *  the NIUI_Core existing, it will only be safe to use the core after this has been called.
 *  */
function OnNIUICoreStart(NIUI_Core TheCore);

/** 
 *  This is called just before the NIUI_Core object is destroyed. After this function is called it is
 *  no longer safe to use the NIUI_Core (doing so will probably crash your program).
 *  */
function OnNIUICoreShutdown();

DefaultProperties
{
}
