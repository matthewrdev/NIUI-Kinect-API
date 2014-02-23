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

#ifndef NIUI_CONVERSION_H
#define NIUI_CONVERSION_H

//****************************************************************************
//**
//**    NIUIConversion.h
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 01/2011
//**
//****************************************************************************

#include "NIUI.h"

// Forward Declarations
namespace NIUI
{
}; // End Namespace NIUI


// OpenNI forward declarations.
struct XnSkeletonJointOrientation;
struct XnSkeletonJointPosition;

extern "C"
{
	struct UDKRotator;
	struct UDKVector3;
	struct UDKQuaternion;
};

namespace NIUI
{

	inline int ToUnrealRotation_Rad(float radian)
	{
		return static_cast<int>(radian * NIUI::RadianToURot);
	}

	inline int ToUnrealRotation_Deg(float degree)
	{
		return static_cast<int>(degree * NIUI::DegreeToURot);
	}

	// Converts an OpenNI joint position to a Unreal position.
	void XnPosToUnrealPos(XnSkeletonJointPosition &source, UDKVector3 &out_target);

	// Converts an OpenNI column major matrix to its equvilent side, front and up vectors in Unreal space.
	void ExtractBasisVectors(float* elements, UDKVector3* side, UDKVector3* front, UDKVector3* up);

}; // End Namespace NIUI


#endif // #ifndef NIUI_CONVERSION_H