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
//**    NIUIReferenceRotationCacher.cpp
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 03/2011
//**
//****************************************************************************

#include <stdio.h>

#include "NIUIReferenceRotationCacher.h"

#include "NIUIConversion.h"
#include "NIUIDataTypes.h"
#include "NIUIUtil.h"
#include "Logger.h"

namespace NIUI
{
	
		ReferenceRotationCacher::ReferenceRotationCacher()
		{
			// Does nothing, initilise method sets everthing up.
		}

		ReferenceRotationCacher::~ReferenceRotationCacher()
		{
			// Does nothing, release method discards everything.
		}

		void ReferenceRotationCacher::Initialise()
		{
			ClearCurrentReferenceInfo();
			GenerateRRFDirectory();
		}

		void ReferenceRotationCacher::Release()
		{
		}

		void ReferenceRotationCacher::GenerateRRFDirectory()
		{
			LOG("Attempting to create Reference Rotation Directory.", LOG_Init);

			// Attempt to create the directory.
			BOOL result = CreateDirectory(L"NIUI_Reference_Rotations", NULL);
			if (result == ERROR_ALREADY_EXISTS)
			{
				LOG("Reference Rotation Directory already exists.", LOG_Init);
			}
			else if (result == ERROR_PATH_NOT_FOUND)
			{
				// Error message here.
			}
			else
			{
				LOG("Created successfully.", LOG_Init);
			}
		}

		NIUI_STATUS ReferenceRotationCacher::ImportReferenceDataFile()
		{
			// Assume the worst.
			NIUI_STATUS status = NIUI_STATUS_FAIL;

			// Generate the mesh name as a .rrf file.
			const wchar_t* outputName = this->GetMeshNameAsRRF(m_fileName);

			printf("%s", outputName);

			// Open file as readable binary.
			FILE* theFile = _wfopen(outputName, L"rb");



			// Validate file opened.
			if (theFile != NULL)
			{
				// Read endian encoding byte.
				unsigned char endianess = GetPlatformEndianess();
				fread(&endianess, sizeof(unsigned char), 1, theFile);

				// Read inplace format.
				int size = fread(this->m_data, sizeof(BoneReferenceData), NIUI_JOINT_COUNT, theFile);
				if (size <= 0)
				{
					LOG("Failed to import .rrf file. Rotation referencing may fail.", LOG_Warning);
				}
				else
				{
					LOG(".rrf file was succesfully imported.", LOG_Standard);
					status = NIUI_STATUS_OK;
				}

				fclose(theFile);

				// Check endianness and convert if necessary
				if (endianess != GetPlatformEndianess())
				{
					this->SwapDataEndianess();
				}
			}

			return status;
		}

		void ReferenceRotationCacher::SetOutputMeshName(std::wstring name)
		{
			m_fileName = name;
		}

		void ReferenceRotationCacher::SetBoneReferencePoint(int jointID, UDKQuaternion* quat)
		{
			if (IsValidJoint(jointID))
			{
				m_data[jointID].w = quat->w;
				m_data[jointID].x = quat->x;
				m_data[jointID].y = quat->y;
				m_data[jointID].z = quat->z;

				m_data[jointID].active = 1;
			}
		}

		void ReferenceRotationCacher::GetBoneReferencePoint(int jointID, UDKQuaternion* out_quat)
		{
			if (IsValidJoint(jointID))
			{
				out_quat->w = m_data[jointID].w;
				out_quat->x = m_data[jointID].x;
				out_quat->y = m_data[jointID].y;
				out_quat->z = m_data[jointID].z;
			}
		}

		int ReferenceRotationCacher::HasReferencePoint(int jointID)
		{
			if (IsValidJoint(jointID))
			{
				return m_data[jointID].active;
			}

			return 0;
		}

		NIUI_STATUS ReferenceRotationCacher::ExportCurrentReferenceInfo()
		{
			// Assume the worst.
			NIUI_STATUS status = NIUI_STATUS_FAIL;

			// Generate the mesh name as a .rrf file.
			const wchar_t* outputName = this->GetMeshNameAsRRF(m_fileName);

			// Open file as writable binary.
			FILE* theFile = _wfopen(outputName, L"wb");

			// Validate file opened.
			if (theFile != NULL)
			{
				// Write endian encoding byte.
				unsigned char endianess = GetPlatformEndianess();
				fwrite(&endianess, sizeof(unsigned char), 1, theFile);

				// Dump inplace format.
				int size = fwrite(this->m_data, sizeof(BoneReferenceData), NIUI_JOINT_COUNT, theFile);
				if (size <= 0)
				{
					LOG("Failed to export .rrf file.", LOG_Warning);
				}
				else
				{
					LOG(".rrf file was succesfully exported.", LOG_Standard);
					status = NIUI_STATUS_OK;
				}

				fclose(theFile);
			}

			return status;
		}

		void ReferenceRotationCacher::ClearCurrentReferenceInfo()
		{
			this->m_fileName.clear();
			memset(this->m_data, 0, sizeof(NIUI::BoneReferenceData) * NIUI_JOINT_COUNT);
		}

		const wchar_t* ReferenceRotationCacher::GetMeshNameAsRRF(std::wstring name)
		{
			static wchar_t theName[1024] = {0};
			memset(theName, 0, sizeof(theName));

			// Append the _NIUI.rrf file format to the end of the mesh name.
			swprintf(theName, L"NIUI_Reference_Rotations\\NIUI_%s.rrf", name.c_str());

			return theName;
		}
		
		bool ReferenceRotationCacher::IsValidJoint(int jointID)
		{
			return jointID >= 0 && jointID < NIUI_JOINT_COUNT;
		}

		void ReferenceRotationCacher::SwapDataEndianess()
		{
			LOG("Converting endianess of loaded input data.", LOG_Standard);
			for (int iter = 0; iter < NIUI_JOINT_COUNT; iter++)
			{
				// Endian swap the data in the struct.
				IntSwap(m_data[iter].active);
				IntSwap(m_data[iter].boneID);
				FloatSwap(m_data[iter].x);
				FloatSwap(m_data[iter].y);
				FloatSwap(m_data[iter].z);
				FloatSwap(m_data[iter].w);
			}
		}

}; // End Namespace NIUI

