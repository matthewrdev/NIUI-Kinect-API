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

#ifndef NIUI_UTIL_H
#define NIUI_UTIL_H

//****************************************************************************
//**
//**    NAME
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 03/2011
//**
//****************************************************************************

#include "NIUIDataTypes.h"

// Forward Declarations
namespace NIUI
{
}; // End Namespace NIUI

namespace NIUI
{
	// ID's for the system endianess.
	const unsigned char BIG_ENDIAN_ID = 1;
	const unsigned char LITTLE_ENDIAN_ID = 0;

	// Calculates the endianness of the current computer 0 = little. Non-Zero = Big.
	inline unsigned char GetPlatformEndianess()
	{
		unsigned char test[2] = {1, 0};
		return (*(short*)test) == 1 ? LITTLE_ENDIAN_ID : BIG_ENDIAN_ID;
	}

	inline short ShortSwap(short s)
	{
		return ((s & 0xFF00) >> 8) |
			   ((s & 0x00FF) << 8);
	}

	inline unsigned short UShortSwap(unsigned short s)
	{
		return ((s & 0xFF00) >> 8) |
			   ((s & 0x00FF) << 8);
	}

	inline int IntSwap(int i)
	{
		return ((i & 0xFF000000) >> 24) |
			    ((i & 0x00FF0000) >> 8) |
				((i & 0x0000FF00) << 8) |
				((i & 0x000000FF) << 24);
	}

	inline unsigned int UIntSwap(unsigned int i)
	{
		return ((i & 0xFF000000) >> 24) |
			    ((i & 0x00FF0000) >> 8) |
				((i & 0x0000FF00) << 8) |
				((i & 0x000000FF) << 24);
	}

	inline float FloatSwap(float f)
	{
		union 
		{
			unsigned char bytes[4];
			float f;
		} temp, result;

		temp.f = f;

		// Swap the bytes.
		result.bytes[0] = temp.bytes[3];
		result.bytes[1] = temp.bytes[2];
		result.bytes[2] = temp.bytes[1];
		result.bytes[3] = temp.bytes[0];

		return result.f;
	}

	
	template<typename T>
	inline void Swap(T& lhs, T& rhs)
	{
		T temp = rhs;
		rhs = lhs;
		lhs = temp;
	}

	template<typename T>
	inline T Clamp(T min, T max, T value)
	{
		if (value < min)
			return min;

		if (value > max)
			return max;

		return value;
	}

	void XnPixelsToUDKPixels(U8* xnBuffer, U8* udkBuffer, int xRes, int yRes);
	void XnPixelsToNIUIPixels(U8* xnBuffer, U8* niuiBuffer, int xRes, int yRes);


}; // End Namespace NIUI


#endif // #ifndef NIUI_UTIL_H