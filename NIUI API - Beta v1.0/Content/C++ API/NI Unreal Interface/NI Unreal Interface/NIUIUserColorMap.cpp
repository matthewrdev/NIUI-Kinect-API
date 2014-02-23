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
//**    NIUIUserColorMap.cpp
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 03/2011
//**
//****************************************************************************

#include "NIUIUserColorMap.h"

namespace NIUI
{

	UserColorMap::UserColorMap()
	{
		// All setup is done in Initialise method.
	}

	UserColorMap::~UserColorMap()
	{
		// All destruction is done in 
	}

	void UserColorMap::Initialise(int maxUsers, xn::UserGenerator* userGenerator, int xRes, int yRes)
	{
		m_userGenerator = userGenerator;

		m_height = yRes;
		m_width = xRes;

		m_totalUsers = maxUsers;

		m_pallete = new Color32[maxUsers + 1]; // Include slot for background. 
		memset(m_pallete, 0, sizeof(Color32) * maxUsers + 1);

		Allocate(xRes, yRes, PIXEL_SIZE, "Color map");
		Clear();

	}

	void UserColorMap::Release()
	{
		m_userGenerator = NULL;

		// Destroy pixel buffer.
		ReleaseBuffer();

		if (m_pallete != NULL)
		{
			delete [] m_pallete;
			m_pallete = NULL;
		}
	}

	// Pulls the label map from the UserGenerator and samples from the pallete to
	// generate a color coded histogram.
	void UserColorMap::Update()
	{
		// Get the label map from the user generator.
		m_userGenerator->GetUserPixels(0, this->m_metaData); // Get user pixels for all users.

		const XnLabel* labelMap = m_metaData.Data();
		BYTE* colorMap = this->GetData();

		size_t pixelSize = GetPixelSize();
		int size = GetTotalPixels();
		int palleteIndex = 0;
		int mod = this->m_totalUsers + 1; // Add 1 to include background for the index hashing.
		bool moreThan1 = false;

		for (int iter = 0; iter < size; iter++)
		{
			palleteIndex = labelMap[iter] % mod; // Users over the mod will wrap.
			memcpy(colorMap, &m_pallete[palleteIndex], sizeof(Color32));
			

			colorMap += pixelSize; // Advance pointer.
		}
	}

	void UserColorMap::SetUserColor(int userID, BYTE r, BYTE g, BYTE b)
	{
		if (userID >= m_totalUsers +1)
			return;

		Color32 color = COLOR_ARGB(0, r,g,b);

		BYTE* col = (BYTE*)&color;
		BYTE* pallete= (BYTE*)&m_pallete[userID];
		pallete[0] = col[0];
		pallete[1] = col[1];
		pallete[2] = col[2];
		pallete[3] = col[3];
	}

	void UserColorMap::SetBackgroundColor(BYTE r, BYTE g, BYTE b)
	{
		m_pallete[0] = COLOR_ARGB(0, r, g, b);
	}

}; // End Namespace NIUI
