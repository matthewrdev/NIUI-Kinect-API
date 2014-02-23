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

#ifndef NIUI_H
#define NIUI_H

//****************************************************************************
//**
//**    NIUI.h
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 01/2011
//**
//****************************************************************************

namespace NIUI
{
	// Size of 4byte ARGB pixel.
	const size_t PIXEL_SIZE = 4;

	// Maths constants
	const float PI = 3.14159265f;
	const float PI_2 = PI * 2.0f;
	const float PI_OVER_2 = PI * 0.5f;

	// The conversion factor of unreal units per metre.
	const float UNREAL_UNIT_CONVERSION = 52.5f;

	// The conversion from millimetres to unreal units.
	const float NI_TO_UNREAL_UNIT = 1.0f / UNREAL_UNIT_CONVERSION;

	// Keep the maximum number of users to a reasonable number.
	const int MAX_NIUI_USERS = 4;

	// Unreal rotation conversion constants.
	const float DegreeToMOA    = 60.0f;           // 21600/360
	const float DegreeToRadian = 0.017453f;       // pi/180
	const float DegreeToURot   = 182.044449f;     // 65536/360
	const float MOAToDegree    = 0.016667f;       // 360/21600
	const float MOAToRadian    = 0.000291f;       // pi/10800
	const float MOAToURot      = 3.034074f;       // 65536/21600
	const float RadianToDegree = 57.295776f;      // 180/pi
	const float RadianToMOA    = 3437.746582f;    // 10800/pi
	const float RadianToURot   = 10430.377930f;   // 32768/pi
 	const float URotToDegree   = 0.005493f;       // 360/65536
	const float URotToMOA      = 0.329590f;       // 21600/65536
	const float URotToRadian   = 0.000096f;       // pi/32768

	// Status codes.
	enum NIUI_STATUS
	{
		NIUI_STATUS_OK = 1,
		NIUI_STATUS_FAIL = -1,

		NIUI_STATUS_COUNT = 2,
	};

	enum NIUI_BoneID // Maps the same as Xn bones but 1 less to allow array indexing.
	{
		NIUI_JOINT_HEAD,
		NIUI_JOINT_NECK,
		NIUI_JOINT_TORSO,
		NIUI_JOINT_WAIST,
		NIUI_JOINT_LEFT_COLLAR,
		NIUI_JOINT_LEFT_SHOULDER,
		NIUI_JOINT_LEFT_ELBOW,
		NIUI_JOINT_LEFT_WRIST,
		NIUI_JOINT_LEFT_HAND,
		NIUI_JOINT_LEFT_FINGERTIP,
		NIUI_JOINT_RIGHT_COLLAR,
		NIUI_JOINT_RIGHT_SHOULDER,
		NIUI_JOINT_RIGHT_ELBOW,
		NIUI_JOINT_RIGHT_WRIST,
		NIUI_JOINT_RIGHT_HAND,
		NIUI_JOINT_RIGHT_FINGERTIP,
		NIUI_JOINT_LEFT_HIP,
		NIUI_JOINT_LEFT_KNEE,
		NIUI_JOINT_LEFT_ANKLE,
		NIUI_JOINT_LEFT_FOOT,
		NIUI_JOINT_RIGHT_HIP,
		NIUI_JOINT_RIGHT_KNEE,
		NIUI_JOINT_RIGHT_ANKLE,
		NIUI_JOINT_RIGHT_FOOT,

		NIUI_JOINT_COUNT,
	};

	inline int ToXnBone(NIUI_BoneID bone)
	{
		return static_cast<int>(bone) + 1;
	}

	inline NIUI_BoneID ToNIUIBone(int xnBone)
	{
		return static_cast<NIUI_BoneID>(xnBone - 1);
	}


}; // End Namespace NIUI

#define ERR_CHECK(val, errorMsg, successMsg, errorLogType, successLogType )	\
	if (val != XN_STATUS_OK)												\
	{																		\
		LOG(errorMsg, errorLogType);										\
		assert(false);														\
		return NIUI::NIUI_STATUS_FAIL;										\
	}																		\
	else																	\
	{																		\
		LOG(successMsg, successLogType);									\
	}

#endif // #ifndef NIUI_H