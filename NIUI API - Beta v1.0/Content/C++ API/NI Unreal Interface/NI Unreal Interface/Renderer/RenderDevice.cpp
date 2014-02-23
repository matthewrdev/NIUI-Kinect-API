//****************************************************************************
//**
//**    Renderer.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include <Windows.h>
#include <assert.h>
#include <iostream>

#include "RenderDevice.h"

#include "Rasterizer.h"

#include "BackBuffer.h"

#include "Vertex.h"

#include "../Logger.h"

namespace NIUI
{
	RenderDevice::RenderDevice()
		: winDevContext(NULL)
		, m_backBuffer(NULL)
		, m_rasterizer(NULL)
	{
		// Default initilises the renderer.
		// All construction should be done through initilise method.
	}

	RenderDevice::~RenderDevice()
	{
	}
	
	NIUI_STATUS RenderDevice::Initialise(int width, int height, HWND hWnd)
	{
		// Validate back-buffer dimensions.
		if (height < 1 || width < 1)
		{	
			char buffer[256] = {0};
			sprintf(buffer, "Invalid back-buffer dimensions for render-device. W=%i H=%i", width, height);
			LOG(buffer, LOG_Error);
			return NIUI_STATUS_FAIL;
		}

		// Create the device context for the window.
		winDevContext = GetDC(hWnd);
		if (winDevContext == NULL)
		{
			LOG("Windows device context retrieval has failed.", LOG_Error);
			return NIUI_STATUS_FAIL;
		}

		// Create the back buffer we are drawing to.
		this->m_backBuffer = new BackBuffer();
		NIUI_STATUS result = m_backBuffer->CreateBuffer(width, height, winDevContext);
		if (result != NIUI_STATUS_OK)
		{
			LOG("Back-buffer creation has failed.", LOG_Error);
			return NIUI_STATUS_FAIL;
		}

				// Create the quat.
		Vertex verts[4] = 
		{
			Vertex( -0.5f, -0.5, 0.0f, 0.0f, 0.0f),    // side 1
			Vertex( 0.5, -0.05, 0.0f, 1.0f, 0.0f),
			Vertex( -0.5, 0.5, 0.0f, 0.0f, 1.0f),
			Vertex( 0.5, 0.5, 0.0f, 1.0f, 1.0f),
		};

		m_rasterizer = new Rasterizer();
		m_rasterizer->SetTargetBuffers(m_backBuffer->GetByteBuffer(), width, height);
		LOG("Render Device startup sucessful.", LOG_Init);

		CreateVertexBuffer(&verts, 4, this->m_quad);

		return NIUI_STATUS_OK;
	}

	NIUI_STATUS RenderDevice::Release(HWND hWnd)
	{
		if (m_quad != NULL)
		{
			delete m_quad;
			m_quad = NULL;
		}

		if (m_backBuffer != NULL)
		{
			m_backBuffer->ReleaseBuffer();
			delete m_backBuffer;
			m_backBuffer = NULL;
		}

		if (m_rasterizer != NULL)
		{
			delete m_rasterizer;
			m_rasterizer = NULL;
		}

		// Destroy the device context.
		if(winDevContext)
		{
			ReleaseDC(hWnd,winDevContext);
			winDevContext = NULL;
		}

		LOG("Render Device shutdown sucessful.", LOG_Shutdown);
		return NIUI_STATUS_OK;
	}

	void RenderDevice::ClearBackBuffer(UINT32 value)
	{
		m_backBuffer->Clear(value);
	}

	void RenderDevice::Present(HWND hWnd)
	{
		// Blit the back-buffer onto the screen.	
		assert(winDevContext != NULL);
		assert(m_backBuffer->GetDeviceContext() != NULL);

		RECT clRect;
		GetClientRect(hWnd,&clRect);

		// Blit to buffer to the window
		if(BitBlt(winDevContext,
				  clRect.left,
				  clRect.top,
				  (clRect.right - clRect.left) + 1, 
				  (clRect.bottom - clRect.top) + 1,
				  m_backBuffer->GetDeviceContext(),
				  0,
				  0,
				  SRCCOPY) == FALSE)
		{
			LOG("BackBuffer presentation failed.", LOG_Warning);
			return;
		}
	
		return;
	}
	
	void RenderDevice::SetPixelColour(U16 x, U16 y, U32 colour)
	{
		m_backBuffer->PlotPixel(x,y, colour);
	}
	
	void RenderDevice::RebuildVertexBuffer(int x, int y, float xScale, float yScale, int width, int height)
	{
		Vertex* verts = m_quad->GetVertices();

		int left, right, up, down;
		int newWidth, newHeight;

		newWidth = (int)((float)width * xScale);
		newHeight = (int)((float)height * yScale);

		left = x - (newWidth >> 1) + this->m_backBuffer->GetWidth() >> 1;
		right = x + (newWidth >> 1) + this->m_backBuffer->GetWidth() >> 1;
		up = y - (newHeight >> 1) + this->m_backBuffer->GetHeight() >> 1;
		down = y + (newHeight >> 1) + this->m_backBuffer->GetHeight() >> 1;

		verts[0].x = left;
		verts[0].y = up;
		verts[1].x = right;
		verts[1].y = up;
		verts[2].x = left;
		verts[2].y = down;
		verts[3].x = right;
		verts[3].y = down;
	}

	
	void RenderDevice::DrawQuad(BYTE* buffer,
								int posX,
								int posY,
								int width,
								int height,
								float xScale,
								float yScale,
								U32 alphaFilter,
								bool useAlphaFilter)
	{
		RebuildVertexBuffer(posX, posY, xScale, yScale, width, height);
		m_rasterizer->SetTargetTexture(buffer, width, height);

		Vertex* vBuffer = m_quad->GetVertices();
		Vertex verts[3];

		// Draw first tri.
		verts[0].Copy(vBuffer[0]);
		verts[1].Copy(vBuffer[1]);
		verts[2].Copy(vBuffer[2]);
		m_rasterizer->RasterizeTriTex(verts, alphaFilter, useAlphaFilter);

		// Draw 2nd tri.
		verts[0].Copy(vBuffer[3]);
		m_rasterizer->RasterizeTriTex(verts, alphaFilter, useAlphaFilter);


	}
		
}; // End namespace NIUI.