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
//**    NIUIImageMap.cpp
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 04/2011
//**
//****************************************************************************

#include "NIUIImageBuffer.h"
#include "NIUIColor.h"

namespace NIUI
{
	
		ImageBuffer::ImageBuffer()
		{
		}

		ImageBuffer::~ImageBuffer()
		{
		}

		void ImageBuffer::Initialise(xn::ImageGenerator* imageGenerator)
		{
			m_imageGenerator = imageGenerator;
			m_imageGenerator->SetPixelFormat(XN_PIXEL_FORMAT_RGB24);

			// Assign buffer width and height.
			m_imageGenerator->GetMetaData(m_metaData);
			m_width = m_metaData.XRes();
			m_height = m_metaData.YRes();

			Allocate(m_width, m_height, PIXEL_SIZE, "Image Buffer");
			Clear(COLOR_ARGB(0,0,0,0)); // Pre-clear the buffer.
		}

		void ImageBuffer::Release()
		{
			m_imageGenerator = NULL;

			ReleaseBuffer();

		}

		void ImageBuffer::Update()
		{
			m_imageGenerator->GetMetaData(m_metaData);

			const XnUInt8* data = m_metaData.Data();
			
			U8* buffer = GetData();

			// Copy the buffer accross
			for (int iter = 0; iter < this->GetTotalPixels() * 3 /*byte sizeof the RGB24 format*/; iter+=3)
			{
				
				buffer[0] = data[iter + 2];      // Red channel.
				buffer[1] = data[iter + 1];  // Green channel.
				buffer[2] = data[iter];      // Blue channel.
				buffer += 4; // Shift pointer along 4 bytes to skip the A channel.
			}
		}

}; // End Namespace NIUI
