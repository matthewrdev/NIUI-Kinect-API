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

#ifndef NIUI_COLOR_H
#define NIUI_COLOR_H

//****************************************************************************
//**
//**    NIUIColor.h
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

#define ALPHA_MASK	0xFF000000
#define RED_MASK	0x00FF0000
#define GREEN_MASK	0x0000FF00
#define BLUE_MASK   0x000000FF

#define ALPHA_BIT_SHIFT 24
#define RED_BIT_SHIFT 16
#define GREEN_BIT_SHIFT 8
#define BLUE_BIT_SHIFT 0

#define ARGB_TO_INT(a,r,g,b, value)		\
	value = (a << ALPHA_BIT_SHIFT) | (r << RED_BIT_SHIFT) | (g << GREEN_BIT_SHIFT) | b;

#define COLOR_ARGB(a,r,g,b)			\
	(a << ALPHA_BIT_SHIFT) | (r << RED_BIT_SHIFT) | (g << GREEN_BIT_SHIFT) | b


namespace NIUI
{
	typedef int Color32;

	inline U8 RedChannel(Color32 c)
	{
		return (U8)((c & RED_MASK) >> RED_BIT_SHIFT);
	}

	inline U8 GreenChannel(Color32 c)
	{
		return (U8)((c & GREEN_MASK) >> GREEN_BIT_SHIFT);
	}

	inline U8 BlueChannel(Color32 c)
	{
		return (U8)(c & BLUE_MASK);
	}

	inline U8 AlphaChannel(Color32 c)
	{
		return (U8)((c & ALPHA_MASK) >> ALPHA_BIT_SHIFT);
	}

	
	// COLOUR...                               A   R    G    B 
	const Color32 BLACK		    =  COLOR_ARGB(0,  0,   0,   0);
	const Color32 WHITE		    =  COLOR_ARGB(0,  255, 255, 255);	
	const Color32 DARK_GREY	    =  COLOR_ARGB(0,  96,  96,  96);      
	const Color32 LIGHT_GREY	=  COLOR_ARGB(0,  192, 192, 192);    
	const Color32 RED		    =  COLOR_ARGB(0,  255, 0,   0);        
	const Color32 BLUE		    =  COLOR_ARGB(0,  0,   0,   255);      
	const Color32 GREEN		    =  COLOR_ARGB(0,  0,   255, 0);        
	const Color32 CYAN		    =  COLOR_ARGB(0,  0,   255, 255);         
	const Color32 MAGENTA	    =  COLOR_ARGB(0,  255, 0,   255);       
	const Color32 YELLOW		=  COLOR_ARGB(0,  255, 255, 0);      
	const Color32 DIRTY         =  COLOR_ARGB(0,  219, 0,   201);

}; // End Namespace NIUI


#endif // #ifndef NIUI_COLOR_H