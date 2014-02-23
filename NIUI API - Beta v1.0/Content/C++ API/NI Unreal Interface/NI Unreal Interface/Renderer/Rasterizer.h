#pragma once

#ifndef RASTERIZER_H
#define RASTERIZER_H

//****************************************************************************
//**
//**    Rasterizer.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include "../NIUIDataTypes.h"

#include "Vertex.h"
#include "RenderData.h"

// Forward Declarations
namespace NIUI
{
	class BackBuffer;
};

namespace NIUI
{
	// ------------------------------------------------------------------------
	//								Rasterizer
	// ------------------------------------------------------------------------
	// Desc:
	// The class used to plot pixels, lines and circles to the back-buffer.
	// It has lots of functions that do similar things however they are used
	// to try avoid if statements and help create linear code.
	// Uses a top-left fill convention by ceiling floating point numbers.
	// ------------------------------------------------------------------------
	class Rasterizer
	{
	private:
		U8* m_targetBackBuffer;
		U32 m_bufferWidth;
		U32 m_bufferHeight;
		ZDepthBuffer* m_targetZBuffer;

		U8* m_targetTexture;
		U32 m_texWidth, m_texHeight;

		void ScanLineTexAffine(ScanlineDataTex* scanline, bool filterAlpha, U32 alphaFilter);

		// Helper function to sort the triangle by Y.
		void SortByY(Vertex* target, Vertex* source);

		enum VertexLocation
		{
			TOP,
			MIDDLE,
			BOTTOM,
		};

	protected:
	public:
		Rasterizer();
		~Rasterizer();

		void SetTargetBuffers(U8* backBuffer, U32 width, U32 height);
		void Release();
		void Reset();

		void SetTargetTexture(U8* texture, U32 width, U32 height);

		// Renders the triangle with texture mapping.
		void RasterizeTriTex(Vertex* tri, U32 alphaFilter, bool useAlphaFilter);

	};
	
}; // End namespace NIUI.

#endif // #ifndef RASTERIZER_H