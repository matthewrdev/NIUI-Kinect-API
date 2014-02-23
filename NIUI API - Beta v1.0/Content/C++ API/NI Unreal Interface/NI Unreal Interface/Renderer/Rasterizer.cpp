//****************************************************************************
//**
//**    Rasterizer.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

//
//
//
//			OPTIMISE WITH INTRINSICS!!!!
//
//
//


#include <assert.h>
#include <cmath>

#include "Rasterizer.h"

#include "BackBuffer.h"

#include "../Logger.h"
#include "../NIUIUtil.h"

#define FIXED_INTEGER_SHIFT 12

namespace NIUI
{
	
	const float EPSILON = 1e-4;

	Rasterizer::Rasterizer()
	{
		Reset();
	}

	Rasterizer::~Rasterizer()
	{
		// Just unassign the buffers, as the deletion is done in the render-device.
		m_targetBackBuffer = NULL;
		m_targetZBuffer = NULL;
	}

	void Rasterizer::SetTargetBuffers(U8* backBuffer, U32 width, U32 height)
	{
		m_targetBackBuffer = backBuffer;
		m_bufferHeight = height;
		m_bufferWidth = width;
	}

	void Rasterizer::Reset()
	{

		// Clear the references to the back-buffer and z-buffer.
		m_targetBackBuffer = NULL;
		m_targetTexture = NULL;
		m_bufferWidth = 0;
		m_bufferHeight = 0;
	}

	void Rasterizer::SetTargetTexture(U8* tex, U32 width, U32 height)
	{
		m_targetTexture = tex;
		m_texWidth = width;
		m_texHeight = height;
	}
	
	void Rasterizer::ScanLineTexAffine(ScanlineDataTex* scanline, bool filterAlpha, U32 alphaFilter)
	{
		if (scanline->y >= m_bufferHeight)
			return;

		// Scanline end will be < 0 and cause a wrap-around.
		if (scanline->xEnd <= 1.0f - EPSILON)
			return;

		static U32 xStart, xEnd, texelIndex;
		static S32 uVal, vVal, uSlope, vSlope, texWidth, texHeight;

		xStart = ceil(scanline->xStart);
		xEnd = ceil(scanline->xEnd);
		
		// Load the back buffer at the start point for our render.
		U32* buffer = (U32*)(m_targetBackBuffer + ((xStart + (scanline->y * m_bufferWidth)) << 2));

		// Load the texels we are going to be referencing into a buffer.
		U32* texels = (U32*)m_targetTexture;

		// Get the width and height for our texture so we can use this to get the appropriate texel
		// in the texture by scaling our U / V.
		texWidth = m_texWidth;
		texHeight = m_texHeight;

		texelIndex = 0;

		uVal = (S32)(scanline->uStart * (1 << FIXED_INTEGER_SHIFT));
		vVal = (S32)(scanline->vStart * (1 << FIXED_INTEGER_SHIFT));
		uSlope = (S32)(scanline->uSlope * (1 << FIXED_INTEGER_SHIFT));
		vSlope = (S32)(scanline->vSlope * (1 << FIXED_INTEGER_SHIFT));

		if (filterAlpha)
		{
			U32 color;

			for (int index = xStart; index < xEnd; index++)
			{
				if (index >= 0 && index < this->m_bufferWidth)
				{
					texelIndex = (uVal >> FIXED_INTEGER_SHIFT) + ((vVal >> FIXED_INTEGER_SHIFT) * texWidth);
					color = texels[texelIndex];
					// Calculate our texel index.
					if (color != alphaFilter)
					{
						*buffer++ = color;
					}
					else
					{
						*buffer++;
					}
				}
				uVal += uSlope;
				vVal += vSlope;
			}
		}
		else
		{

			for (int index = xStart; index < xEnd; index++)
			{
				if (index >= 0 && index < this->m_bufferWidth)
				{
					texelIndex = (uVal >> FIXED_INTEGER_SHIFT) + ((vVal >> FIXED_INTEGER_SHIFT) * texWidth);
					// Calculate our texel index.
					*buffer++ = texels[texelIndex];
				}
				uVal += uSlope;
				vVal += vSlope;
			}
		}
	}

	void Rasterizer::SortByY(Vertex* target, Vertex* source)
	{
		bool usedIndices[3] = {false, false, false};
		
		Real bestY = this->m_bufferHeight;
		int bestVert = -1;

		// Selection sort the triangle for the lowest Y value (the top, middle and bottom of the triangle).
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (source[j].y < bestY && usedIndices[j] == false)
				{
					bestY = source[j].y;
					bestVert = j;
				}
			}
			// Copy the data into the vertices buffer.
			target[i] = source[bestVert];
			usedIndices[bestVert] = true;
			bestVert = -1;
			bestY = this->m_bufferHeight;
		}
	}

	// The texture based triangle rasterization is the same as the Gourad shaded filled triangle procedure.
	// We use the same steps and procedures of deducing the type of triangle (Major or minor) however here
	// we are interpolating UV co-ordinates based on the slopes between each vertex.
	// Each UV co-ordinate is also scaled up by the source texture width and height to avoid getting visaul artifacts.
	void Rasterizer::RasterizeTriTex(Vertex* tri, U32 alphaFilter, bool useAlphaFilter)
	{
		enum VertexLocation
		{
			TOP,
			MIDDLE,
			BOTTOM,
		};
		// The type of triangle that is being drawn.
		// Major means that edge with greatest Y delta is on left, minor means it is on right.
		TriangleEdgeType triType;

		// Sort vertices up to down.
		Vertex verts[3];
		
		SortByY(verts, tri);

		// Determine if the triangle is major/minor.
		if (verts[BOTTOM].x > verts[MIDDLE].x)
		{
			triType = TRIANGLE_Minor;
		}
		else
		{
			triType = TRIANGLE_Major;
		}

		// Set up Y deltas and inverse multipliers for edge traversal.
		float invDeltaYTB = 1.0f / (verts[BOTTOM].y - verts[TOP].y);
		float invDeltaYTM = 1.0f / (verts[MIDDLE].y - verts[TOP].y);
		float invDeltaYMB = 1.0f / (verts[BOTTOM].y - verts[MIDDLE].y);

		// Scale out the vertices UV's based on the current texture.
		for (int i = 0; i < 3; i++)
		{
			verts[i].u *= m_texWidth;
			verts[i].v *= m_texHeight;
		}
	
		// Slope values for screen pixels
		float x1, x2;
		float z1, z2;
		int yStart;
		int yEnd;
		float xSlopeLeft, xSlopeRight;
		float zSlopeLeft, zSlopeRight;
		float spanXInv = 0.0f;

		// Slope values for texel values.
		float u1, u2;
		float v1, v2;
		float uSlopeLeft, vSlopeLeft;
		float uSlopeRight, vSlopeRight;

		// To update the interpolants.
		float sub = 0.0f;

		static ScanlineDataTex scanline;
		memset(&scanline, 0, sizeof(ScanlineDataTex));
		
		// Decide on how to setup the scanline rendering.
		if (triType == TRIANGLE_Minor)
		{
			// The two smaller triangle edges are to the left so we are basing our scan-line
			// rendering from this.

			// Calculate our values for the scan line interpolation.
			xSlopeLeft = (verts[MIDDLE].x - verts[TOP].x) * invDeltaYTM;
			xSlopeRight = (verts[BOTTOM].x - verts[TOP].x) * invDeltaYTB;
			x1 = verts[TOP].x;
			x2 = verts[TOP].x;
			yStart = ceil(verts[TOP].y);
			yEnd = ceil(verts[MIDDLE].y) - 1;

			// Set up the colour interpolation values.
			u1 = verts[TOP].u;
			u2 = verts[TOP].u;
			v1 = verts[TOP].v;
			v2 = verts[TOP].v;

			uSlopeLeft = (verts[MIDDLE].u - verts[TOP].u) * invDeltaYTM;
			uSlopeRight = (verts[BOTTOM].u - verts[TOP].u) * invDeltaYTB;
			vSlopeLeft = (verts[MIDDLE].v - verts[TOP].v) * invDeltaYTM;
			vSlopeRight = (verts[BOTTOM].v - verts[TOP].v) * invDeltaYTB;
		
			// The incidental offset that may be generated by using the ceil function.
			// We may get 'graphical artifacts' / visaul errors if we dont correct starting values
			// by this.
			sub = (float)yStart - verts[TOP].y;
			// If our right slope is less than our left slope then we are going to get an incomplete triangle.
			// Swap the values to fix this.
			bool hasSwaped = false;
			if (xSlopeRight < xSlopeLeft)
			{
				// Swap x axis interpolation.
				Swap<float>(x2,x1);
				Swap<float>(xSlopeLeft,xSlopeRight);

				// Swap texel interpolation.
				Swap<float>(u1, u2);
				Swap<float>(uSlopeLeft, uSlopeRight);

				Swap<float>(v1, v2);
				Swap<float>(vSlopeLeft, vSlopeRight);

				hasSwaped = true;
			}


			// Draw the upper triangle section.
			if (invDeltaYTM > EPSILON)
			{
				// Run down from the top to the middle of the triangle.
				for (int y = yStart; y <= yEnd; y++)
				{
					// Clamp the UVs
					u1 = Clamp<double>(0.0, (double)m_texWidth, u1);
					u2 = Clamp<double>(0.0, (double)m_texWidth, u2);
					v1 = Clamp<double>(0.0, (double)m_texHeight, v1);
					v2 = Clamp<double>(0.0, (double)m_texHeight, v2);

					scanline.y = y;
					scanline.xStart = x1;
					scanline.xEnd = x2;

					spanXInv = 1.0f /  fabs(x1 - x2);
					
					scanline.uStart = u1;
					scanline.uSlope = (u2 - u1) * spanXInv;
					scanline.vStart = v1;
					scanline.vSlope = (v2 - v1) * spanXInv;

					ScanLineTexAffine(&scanline, useAlphaFilter, alphaFilter);

					// Update the slopes.
					x1 += xSlopeLeft;
					x2 += xSlopeRight;
					u1 += uSlopeLeft;
					u2 += uSlopeRight;
					v1 += vSlopeLeft;
					v2 += vSlopeRight;
				}
			}
			
			// Swap back if we swapped.
			if (hasSwaped)
			{
				Swap<float>(x2,x1);
				Swap<float>(xSlopeLeft,xSlopeRight);
				
				// Swap texel interpolation.
				Swap<float>(u1, u2);
				Swap<float>(uSlopeLeft, uSlopeRight);

				Swap<float>(v1, v2);
				Swap<float>(vSlopeLeft, vSlopeRight);
			}

			// Setup the new slope values for the lower left edge.
			xSlopeLeft = (verts[BOTTOM].x - verts[MIDDLE].x) * invDeltaYMB;
			x1 = verts[MIDDLE].x;
			yStart = ceil(verts[MIDDLE].y);
			yEnd = ceil(verts[BOTTOM].y) - 1;

			u1 = verts[MIDDLE].u;
			v1 = verts[MIDDLE].v;

			// Calculate the new colour interpolations.
			uSlopeLeft = (verts[BOTTOM].u - verts[MIDDLE].u) * invDeltaYMB;
			vSlopeLeft = (verts[BOTTOM].v - verts[MIDDLE].v) * invDeltaYMB;
		
			if (x2 < x1)
			{
				Swap<float>(x2, x1);
				Swap<float>(xSlopeLeft, xSlopeRight);
				
				// Swap texel interpolation.
				Swap<float>(u1, u2);
				Swap<float>(uSlopeLeft, uSlopeRight);

				Swap<float>(v1, v2);
				Swap<float>(vSlopeLeft, vSlopeRight);
			}

			// The incidental offset that may be generated by using the ceil function.
			// We may get 'graphical artifacts' / visual errors if we dont correct starting values
			// by this.
			sub = (float)yStart - verts[MIDDLE].y;
			

			if (invDeltaYMB > EPSILON)
			{				
				// Run down from the middle to the bottom of the triangle.
				for (int y = yStart; y <= yEnd; y++)
				{
					// Clamp the UVs
					u1 = Clamp<double>(0.0, (double)m_texWidth, u1);
					u2 = Clamp<double>(0.0, (double)m_texWidth, u2);
					v1 = Clamp<double>(0.0, (double)m_texHeight, v1);
					v2 = Clamp<double>(0.0, (double)m_texHeight, v2);

					scanline.y = y;
					scanline.xStart = x1;
					scanline.xEnd = x2;

					spanXInv = 1.0f /  fabs(x1 - x2);
					
					scanline.uStart = u1;
					scanline.uSlope = (u2 - u1) * spanXInv;
					scanline.vStart = v1;
					scanline.vSlope = (v2 - v1) * spanXInv;
					
					ScanLineTexAffine(&scanline, useAlphaFilter, alphaFilter);

					// Update the slopes.
					x1 += xSlopeLeft;
					x2 += xSlopeRight;
					u1 += uSlopeLeft;
					u2 += uSlopeRight;
					v1 += vSlopeLeft;
					v2 += vSlopeRight;

				}
			}
		}
		else if (triType == TRIANGLE_Major)
		{
			// Calculate our values for the scan line interpolation.
			xSlopeLeft = (verts[BOTTOM].x - verts[TOP].x) * invDeltaYTB;
			xSlopeRight = (verts[MIDDLE].x - verts[TOP].x) * invDeltaYTM;
			x1 = verts[TOP].x;
			x2 = verts[TOP].x;
			yStart = ceil(verts[TOP].y);
			yEnd = ceil(verts[MIDDLE].y) - 1;

			u1 = verts[TOP].u;
			u2 = verts[TOP].u;
			v1 = verts[TOP].v;
			v2 = verts[TOP].v;

			uSlopeLeft = (verts[BOTTOM].u- verts[TOP].u) * invDeltaYTB;
			uSlopeRight = (verts[MIDDLE].u - verts[TOP].u) * invDeltaYTM;
			vSlopeLeft = (verts[BOTTOM].v - verts[TOP].v) * invDeltaYTB;
			vSlopeRight = (verts[MIDDLE].v - verts[TOP].v) * invDeltaYTM;

			bool hasSwaped = false;
			if (xSlopeRight < xSlopeLeft)
			{
				Swap<float>(x2,x1);
				Swap<float>(xSlopeLeft,xSlopeRight);
				
				// Swap texel interpolation.
				Swap<float>(u1, u2);
				Swap<float>(uSlopeLeft, uSlopeRight);

				Swap<float>(v1, v2);
				Swap<float>(vSlopeLeft, vSlopeRight);

				hasSwaped = true;
			}
			
			// The incidental offset that may be generated by using the ceil function.
			// We may get 'graphical artifacts' / visaul errors if we dont correct starting values
			// by this.
			sub = (float)yStart - verts[TOP].y;

			// Draw the upper triangle section.
			if (invDeltaYTM > EPSILON)
			{
				for (int y = yStart; y <= yEnd; y++)
				{
					// Clamp the UVs
					u1 = Clamp<double>(0.0, (double)m_texWidth, u1);
					u2 = Clamp<double>(0.0, (double)m_texWidth, u2);
					v1 = Clamp<double>(0.0, (double)m_texHeight, v1);
					v2 = Clamp<double>(0.0, (double)m_texHeight, v2);

					scanline.y = y;
					scanline.xStart = x1;
					scanline.xEnd = x2;

					spanXInv = 1.0f /  fabs(x1 - x2);
					
					scanline.uStart = u1;
					scanline.uSlope = (u2 - u1) * spanXInv;
					scanline.vStart = v1;
					scanline.vSlope = (v2 - v1) * spanXInv;
					
					ScanLineTexAffine(&scanline, useAlphaFilter, alphaFilter);

					// Update the slopes.
					x1 += xSlopeLeft;
					x2 += xSlopeRight;
					u1 += uSlopeLeft;
					u2 += uSlopeRight;
					v1 += vSlopeLeft;
					v2 += vSlopeRight;
				}
			}

			// Swap back if we swapped.
			if (hasSwaped)
			{
				Swap<float>(x2,x1);
				Swap<float>(xSlopeLeft,xSlopeRight);
				
				// Swap texel interpolation.
				Swap<float>(u1, u2);
				Swap<float>(uSlopeLeft, uSlopeRight);

				Swap<float>(v1, v2);
				Swap<float>(vSlopeLeft, vSlopeRight);
			}

			// Setup the new slope values for the lower left edge.
			xSlopeRight = (verts[BOTTOM].x - verts[MIDDLE].x) * invDeltaYMB;
			x2 = verts[MIDDLE].x;
			yStart = ceil(verts[MIDDLE].y);
			yEnd = ceil(verts[BOTTOM].y) - 1;
			
			u2 = verts[MIDDLE].u;
			v2 = verts[MIDDLE].v;

			// Calculate the new colour interpolations.
			uSlopeRight = (verts[BOTTOM].u - verts[MIDDLE].u) * invDeltaYMB;
			vSlopeRight = (verts[BOTTOM].v - verts[MIDDLE].v) * invDeltaYMB;

			
			if (x2 < x1)
			{
				Swap<float>(x2, x1);
				Swap<float>(xSlopeLeft, xSlopeRight);
				
				// Swap texel interpolation.
				Swap<float>(u1, u2);
				Swap<float>(uSlopeLeft, uSlopeRight);

				Swap<float>(v1, v2);
				Swap<float>(vSlopeLeft, vSlopeRight);
			}
			
			// The incidental offset that may be generated by using the ceil function.
			// We may get 'graphical artifacts' / visaul errors if we dont correct starting values
			// by this.
			sub = (float)yStart - verts[MIDDLE].y;

			if (invDeltaYMB > EPSILON)
			{				
				// Run down from the middle to the bottom of the triangle.
				for (int y = yStart; y <= yEnd; y++)
				{
					// Clamp the UVs
					u1 = Clamp<double>(0.0, (double)m_texWidth, u1);
					u2 = Clamp<double>(0.0, (double)m_texWidth, u2);
					v1 = Clamp<double>(0.0, (double)m_texHeight, v1);
					v2 = Clamp<double>(0.0, (double)m_texHeight, v2);

					scanline.y = y;
					scanline.xStart = x1;
					scanline.xEnd = x2;

					spanXInv = 1.0f /  fabs(x1 - x2);
					
					scanline.uStart = u1;
					scanline.uSlope = (u2 - u1) * spanXInv;
					scanline.vStart = v1;
					scanline.vSlope = (v2 - v1) * spanXInv;
					
					ScanLineTexAffine(&scanline, useAlphaFilter, alphaFilter);

					// Update the slopes.
					x1 += xSlopeLeft;
					x2 += xSlopeRight;
					u1 += uSlopeLeft;
					u2 += uSlopeRight;
					v1 += vSlopeLeft;
					v2 += vSlopeRight;
				}
			}
		}
		else
		{
			LOG("Triangle failed to be classified as a valid render type", LOG_Error);
			assert(false);
		}
	}
		
}; // End namespace NIUI.
