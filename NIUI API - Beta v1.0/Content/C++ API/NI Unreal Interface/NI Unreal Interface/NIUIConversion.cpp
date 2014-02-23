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

//****************************************************************************
//**
//**    NIUIConversion.cpp
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 01/2011
//**
//****************************************************************************

#include <cmath>

#include <XnOpenNI.h>

#include "NIUIConversion.h"
#include "NIUIDataTypes.h"


namespace NIUI
{

	void AsQuaternion(UDKQuaternion& quat, float x, float y, float z, float w)
	{
		quat.w = w;
		quat.x = x;
		quat.y = y;
		quat.z = z;
	}

	void XnPosToUnrealPos(XnSkeletonJointPosition& source, UDKVector3& out_target)
	{
		out_target.x = source.position.Z * NI_TO_UNREAL_UNIT;
		out_target.y = source.position.X * NI_TO_UNREAL_UNIT;
		out_target.z = source.position.Y * NI_TO_UNREAL_UNIT;
	}
	
	void ExtractBasisVectors(float* elements, UDKVector3* side, UDKVector3* front, UDKVector3* up)
	{
		// Copy side axis.
		side->x = elements[6];
		side->z = elements[3];
		side->y = elements[0];

		// Copy up axis.
		up->x = elements[7];
		up->z = elements[4];
		up->y = elements[1];

		// Copy forward axis.
		front->x = elements[8];
		front->z = elements[5];
		front->y = elements[2];
	}

}; // End Namespace NIUI