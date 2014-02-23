#pragma once

#ifndef RENDER_DEVICE_H
#define RENDER_DEVICE_H

//****************************************************************************
//**
//**    RenderDevice.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include "../NIUIDataTypes.h"
#include "Vertex.h"

// Forward Declarations
namespace NIUI
{
	class BackBuffer;
	class VertexBuffer;
	class Rasterizer;
	class LineClipper;
}

namespace NIUI
{
	// ------------------------------------------------------------------------
	//								RenderDevice
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	class RenderDevice
	{
	private:
		// The windows device context.
		HDC winDevContext;

		// The buffers for the render-device.
		BackBuffer* m_backBuffer;
		Rasterizer* m_rasterizer;

		VertexBuffer* m_quad;

		void RebuildVertexBuffer(int x, int y, float xScale, float yScale, int width, int height);
	protected:
	public:
		RenderDevice();
		~RenderDevice();

		NIUI_STATUS Initialise(int width, int height, HWND hWnd);
		NIUI_STATUS Release(HWND hWnd);

		//NIUI_STATUS Reset(

		void ClearBackBuffer(UINT32 value);

		void Present(HWND hWnd);

		void SetPixelColour(U16 x, U16 y, U32 colour);

		// Renders a 2D quad.
		void DrawQuad(BYTE* buffer, int posX, int posY, int width, int height, float xScale, float yScale, U32 alphaFilter, bool useAlphaFilter);
	};
	
}; // End namespace NIUI.

#endif // #ifndef RENDER_DEVICE_H