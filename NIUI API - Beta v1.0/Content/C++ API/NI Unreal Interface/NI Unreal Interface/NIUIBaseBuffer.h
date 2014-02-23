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

#ifndef NIUI_BASE_BUFFER_H
#define NIUI_BASE_BUFFER_H

//****************************************************************************
//**
//**    NIUIBaseBuffer.h
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 04/2011
//**
//****************************************************************************

#include "NIUIDataTypes.h"
#include "NIUIColor.h"
#include "Logger.h"

// Forward Declarations
namespace NIUI
{
}; // End Namespace NIUI

namespace NIUI
{

	// ------------------------------------------------------------------------
	//								BaseBuffer
	// ------------------------------------------------------------------------

	class BaseBuffer
	{
	private:
	protected:
		U8* m_buffer;
		int m_width;
		int m_height;
		int m_pixelSize;

		// Call me in the Initisilsation of the inherited class.
		void Allocate(int width, int height, size_t pixelSize, const char* bufferName)
		{
			m_buffer = new U8[width * height * pixelSize];

			m_width = width;
			m_height = height;
			m_pixelSize = pixelSize;

			static char text[256];
			sprintf(text, "%s has been allocated with X:%i Y:%i", bufferName, width, height);
			LOG(text, LOG_Init);

		}

		void ReleaseBuffer()
		{
			if (m_buffer != NULL)
			{
				delete [] m_buffer;
				m_buffer = NULL;
			}
		}

	public:

		BaseBuffer()
			: m_buffer(NULL)
		{		}

		virtual ~BaseBuffer()
		{		}

		virtual void Update() = 0;

		U8* GetData()
		{
			return m_buffer;
		}

		int GetBufferWidth() const
		{
			return m_width;
		}

		int GetBufferHeight() const
		{
			return m_height;
		}

		size_t GetPixelSize() const
		{
			return m_pixelSize;
		}

		int GetTotalPixels() const
		{
			return m_width * m_height;
		}

		size_t GetBufferSize() const
		{
			return GetTotalPixels() * m_pixelSize;
		}

		void Clear(int color = COLOR_ARGB(0,0,0,0))
		{
			memset(this->m_buffer, color, GetBufferSize());
		}

		inline void SetPixel(BYTE* ptr, BYTE* rgb24)
		{
			ptr[0] = rgb24[2];      // Red channel.
			ptr[1] = rgb24[1];  // Green channel.
			ptr[2] = rgb24[0];      // Blue channel.
		}

	};

}; // End Namespace NIUI


#endif // #ifndef NIUI_BASE_BUFFER_H