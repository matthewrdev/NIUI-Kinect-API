#pragma once

#ifndef VERTEX_H
#define VERTEX_H

//****************************************************************************
//**
//**    Vertex.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include <Windows.h>

#include "../NIUIDataTypes.h"


namespace NIUI
{
	struct Vertex
	{
		Real x,y,z;
		float u,v;

		Vertex(){}
		~Vertex(){}

		Vertex(const Vertex& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			u = v.u;
			this->v = v.v;
		}

		Vertex(float xIn, float yIn, float zIn, float uIn, float vIn)
		{
			x = xIn;
			y = yIn;
			z = zIn;
			u = uIn;
			this->v = vIn;
		}
		
		inline void operator=(const Vertex& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			u = v.u;
			this->v = v.v;
		}

		inline void Copy(const Vertex& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			u = v.u;
			this->v = v.v;
		}
	};

	struct Triangle
	{
		Vertex* verts;
	};

	// ------------------------------------------------------------------------
	//								VertexBuffer
	// ------------------------------------------------------------------------
	// Desc:
	// 
	// ------------------------------------------------------------------------
	class VertexBuffer
	{
	private:

		Vertex* m_vertices; // A byte buffer.
		U16 m_numOfVerts;
		
		friend NIUI_STATUS CreateVertexBuffer(void* verts, U16 totalVerts, VertexBuffer* &out_buffer);
	protected:
	public:

		VertexBuffer()
			: m_vertices(NULL)
			, m_numOfVerts(0)
		{
		}

		~VertexBuffer()
		{
			if (m_vertices != NULL)
			{
				delete [] m_vertices;
				m_vertices = NULL;
			}
		}

		Vertex* GetVertices();

		U16 GetTotalVerts() const;

		// Returns the size in BYTES of the total vertices stored by the buffer.
		size_t Size();
	};

	NIUI_STATUS CreateVertexBuffer(void* verts, U16 totalVerts, VertexBuffer* &out_buffer);
}; // End namespace NIUI.

#endif // #ifndef VERTEX_H