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
//**    NIUIDepthMap.cpp
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 04/2011
//**
//****************************************************************************

#include "NIUI.h"
#include "NIUIDepthBuffer.h"

namespace NIUI
{
	
		DepthBuffer::DepthBuffer()
		{
		}

		DepthBuffer::~DepthBuffer()
		{
		}

		void DepthBuffer::Initialise(xn::DepthGenerator* depthGenerator)
		{
			m_depthGenerator = depthGenerator;
			depthGenerator->GetMetaData(m_metaData);

			// Allocate the pixel buffer.
			this->Allocate(m_metaData.XRes(), m_metaData.YRes(), PIXEL_SIZE, "Depth Buffer");

		}

		void DepthBuffer::Release()
		{
			ReleaseBuffer();
		}

		void DepthBuffer::Update()
		{
			m_depthGenerator->GetMetaData(m_metaData);
			
			const XnDepthPixel* data = m_metaData.Data();
			BYTE* buffer = GetData();



			BYTE grayscale = 0;
			float conversion = 0.0f;
			int value = 0;
			for (int iter = 0; iter < this->GetTotalPixels(); iter++)
			{
				buffer[0] = buffer[1] = buffer[2] = data[iter] >> 4;

				buffer += 4; // Advance pointer.
			}
		}

}; // End Namespace NIUI
