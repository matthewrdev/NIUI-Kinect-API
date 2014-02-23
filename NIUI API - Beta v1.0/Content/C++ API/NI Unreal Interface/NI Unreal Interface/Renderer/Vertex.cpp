//****************************************************************************
//**
//**    VertexBuffer.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include <Windows.h>
#include <iostream>

#include "../NIUI.h"
#include "Vertex.h"
#include "../Logger.h"


namespace NIUI
{
	Vertex* VertexBuffer::GetVertices()
	{
		return (Vertex*)m_vertices;
	}

	U16 VertexBuffer::GetTotalVerts() const
	{
		return m_numOfVerts;
	}

	size_t VertexBuffer::Size()
	{
		return m_numOfVerts * sizeof(Vertex);
	}


	NIUI_STATUS CreateVertexBuffer(void* verts, U16 totalVerts, VertexBuffer* &out_buffer)
	{
		out_buffer = new VertexBuffer();
		out_buffer->m_numOfVerts = totalVerts;

		size_t size = totalVerts * sizeof(Vertex);

		// Allocate the new buffer.
		out_buffer->m_vertices = new Vertex[totalVerts];

		if (out_buffer->m_vertices == NULL)
			return NIUI_STATUS_FAIL;

		memcpy(out_buffer->m_vertices, verts, size);
		return NIUI_STATUS_OK;

	}

}; // End namespace NIUI.