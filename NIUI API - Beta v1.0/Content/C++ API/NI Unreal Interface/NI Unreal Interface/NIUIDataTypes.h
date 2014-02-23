/*****************************************************************************
*                                                                            *
*  Natural Interaction Unreal Interface (NIUI)                               *
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

#ifndef NIUI_DATA_TYPES_H
#define NIUI_DATA_TYPES_H

//****************************************************************************
//**
//**    NIUIDataTypes.h
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 01/2011
//**
//****************************************************************************

#include "NIUI.h"

#include <XnOpenNI.h>
#include <XnCppWrapper.h>

// Forward Declarations
namespace NIUI
{
}; // End Namespace NIUI


// Data types are defined in "C" code to prevent errors when converting from UnrealScript to C++.
extern "C"
{
	struct UDKRotator
	{
		int pitch;
		int yaw;
		int roll;
	};

	struct UDKVector3
	{
		float x;
		float y;
		float z;
	};

	struct UDKQuaternion
	{
		float x;
		float y;
		float z;
		float w;
	};

	// Used to reflect the state of a user to unreal. Allows for a fast memcpy and minimal interfacing calls b/w Unreal and the dll.
	struct UserMirror
	{
		UDKRotator JointRotations[NIUI::NIUI_JOINT_COUNT];
		UDKVector3 WorldJointLocations[NIUI::NIUI_JOINT_COUNT];
		UDKVector3 LocalJointLocations[NIUI::NIUI_JOINT_COUNT];
		UDKVector3 UserCentre;
		int detected;
		int lost;
		int calibrated;
		int tracking;
	};

	struct UDKString
	{
	   wchar_t* Data;
	   int ArrayNum;
	   int ArrayMax;
	};
}

namespace NIUI
{
	struct Orientation
	{
		float yaw, pitch, roll;
	};

	struct Location
	{
		float x, y, z;
	};
	
	// Define the OpenNIContexts.
	struct OpenNIContextData
	{
		xn::Context context;
		xn::DepthGenerator depthGenerator;
		xn::ImageGenerator imageGenerator;
		xn::UserGenerator userGenerator;
	};

	// Typedef for more explicit data types
	typedef unsigned char U8;
	typedef signed char S8;
	typedef unsigned short U16;
	typedef signed short S16;
	typedef unsigned int U32;
	typedef signed int S32;
	typedef float Real;

}; // End Namespace NIUI 


#endif // #ifndef NIUI_DATA_TYPES_H