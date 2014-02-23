#pragma once

#ifndef RENDER_DATA_H
#define RENDER_DATA_H

//****************************************************************************
//**
//**    RenderData.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 12/2010
//**
//****************************************************************************

#include "../NIUIDataTypes.h"

// Forward Declarations
namespace NIUI
{
	class LightingManager;
	class BackBuffer;
	class ZDepthBuffer;
	struct Vertex;
	class Texture;
};

namespace NIUI
{
	struct ScanlineDataCol
	{
		U32 y;					   // The Y position for the scan line in the back buffer.
		Real xStart, xEnd;		   // The start and end x positions for the scan-line.
		Real zStart, zSlope;	   // The start z position and slope for the scan-line.
		Real rStart, rSlope;	   // The red channel start and slope for the scan-line.
		Real gStart, gSlope;	   // The red channel start and slope for the scan-line.
		Real bStart, bSlope;	   // The red channel start and slope for the scan-line.

		ScanlineDataCol(const ScanlineDataCol& data)
		{
			xStart = data.xStart;
			xEnd = data.xEnd;
			y = data.y;
			zStart = data.zStart;
			zSlope = data.zSlope;
			rStart = data.rStart;
			rSlope = data.rSlope;
			gStart = data.gStart;
			gSlope = data.gSlope;
			bStart = data.bStart;
			bSlope = data.bSlope;
		}

		ScanlineDataCol(){}
	};

	struct ScanlineDataTex
	{
		U32 y;					   // The Y position for the scan line in the back buffer.
		Real xStart, xEnd;		   // The start and end x positions for the scan-line.
		Real zStart, zSlope;	   // The start z position and slope for the scan-line.
		Real uStart, uSlope;	   // The start U position and slope for the texture referenced in the scanline.
		Real vStart, vSlope;	   // The start V position and slope for the texture referenced in the scanline.

		ScanlineDataTex(const ScanlineDataTex &data)
		{
			xStart = data.xStart;
			xEnd = data.xEnd;
			y = data.y;
			zStart = data.zStart;
			zSlope = data.zSlope;
			uStart = data.uStart;
			uSlope = data.uSlope;
			vStart = data.vStart;
			vSlope = data.vSlope;
		}
		ScanlineDataTex(){}
	};


	// ------------------------------------------------------------------------
	//								TriangleEdgeType
	// ------------------------------------------------------------------------
	// Desc:
	// When calculating how to render the scan lines for a triangle, the 
	// rasterizer will determine what type of triangle it is.
	// The type is figured from when the longest edge in the Y axis lies 
	// relative to the two smaller edges and the topmost vertices.
	// A major triangle means the longest edge lies in to the left of the two
	// smaller edges.
	// A minor triangle means the two smaller edges lie to the left of the 
	// longest edge.
	// ------------------------------------------------------------------------
	enum TriangleEdgeType
	{
		TRIANGLE_Minor,			
		TRIANGLE_Major,
	};

	struct SharedRenderResources
	{
	public:
		LightingManager* lightingManager;
		BackBuffer* backBuffer;
		ZDepthBuffer* depthBuffer;
	};

	enum RenderJobType
	{
		JOB_Tex_List_Indice,
		JOB_Tex_List,
		JOB_Tex_Strip_Indice,
		JOB_Tex_Strip,
		JOB_Col_List_Indice,
		JOB_Col_List,
		JOB_Col_Strip_Indice,
		JOB_Col_Strip,
	};

	struct ThreadJob
	{
	public:
		Vertex* triangle;
		Texture* texture;
		RenderJobType type;

		bool complete;
	};

}; // End namespace NIUI.

#endif // #ifndef RENDER_DATA_H