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

#ifndef NIUI_USER_HISTOGRAM_H
#define NIUI_USER_HISTOGRAM_H

//****************************************************************************
//**
//**    NIUIUserColorMap.h
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 04/2011
//**
//****************************************************************************

#include <XnCppWrapper.h>

#include "NIUI.h"
#include "NIUIColor.h"

#include "NIUIBaseBuffer.h"

// Forward Declarations
namespace NIUI
{
}; // End Namespace NIUI

namespace NIUI
{

	// ------------------------------------------------------------------------
	//								UserColorMap
	// ------------------------------------------------------------------------

	class UserColorMap : public BaseBuffer
	{
	private:
		int m_totalUsers;

		// Array of colors that link to each user. Length == TotalUsers + 1
		// 0 = Background. 1...n are assigned to users.
		Color32* m_pallete;

		xn::UserGenerator* m_userGenerator;
		xn::SceneMetaData m_metaData;

	protected:
	public:

		UserColorMap();
		~UserColorMap();

		void Initialise(int maxUsers, xn::UserGenerator* userGenerator, int xRes, int yRes);
		void Release();

		void SetUserColor(int userID, BYTE r, BYTE g, BYTE b);
		void SetBackgroundColor(BYTE r, BYTE g, BYTE b);

		// Pulls the label map from the UserGenerator and samples from the pallete to
		// generate a color coded histogram.
		void Update();
	};

}; // End Namespace NIUI


#endif // #ifndef NIUI_USER_HISTOGRAM_H