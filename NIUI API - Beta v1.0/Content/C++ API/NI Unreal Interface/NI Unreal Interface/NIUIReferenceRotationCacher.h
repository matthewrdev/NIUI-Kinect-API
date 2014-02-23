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

#ifndef NIUI_REFERENCE_ROTATION_CACHER_H
#define NIUI_REFERENCE_ROTATION_CACHER_H

//****************************************************************************
//**
//**    NIUIReferenceRotationCacher.h
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 03/2011
//**
//****************************************************************************

#include <string>

#include "NIUI.h"

extern "C"
{
	struct UDKQuaternion;
};

// Forward Declarations
namespace NIUI
{
}; // End Namespace NIUI

namespace NIUI
{
	struct BoneReferenceData
	{
		int boneID;
		int active;
		float x,y,z,w;
	};

	// ------------------------------------------------------------------------
	//							ReferenceRotationCacher
	// ------------------------------------------------------------------------

	class ReferenceRotationCacher
	{
	private:
		BoneReferenceData m_data[NIUI::NIUI_JOINT_COUNT];

		std::wstring m_fileName;

		const wchar_t* GetMeshNameAsRRF(std::wstring name);
		bool IsValidJoint(int jointID);

		// Checks that all quaternions are normalised and are not whacked out values.
		bool ValidateImportedRRFInfo();

		void GenerateRRFDirectory();

		void SwapDataEndianess();
	protected:
	public:

		ReferenceRotationCacher();
		~ReferenceRotationCacher();

		void Initialise();
		void Release();

		NIUI_STATUS ImportReferenceDataFile();

		void SetOutputMeshName(std::wstring name);

		void SetBoneReferencePoint(int jointID, UDKQuaternion* quat);
		void GetBoneReferencePoint(int jointID, UDKQuaternion* out_quat);

		int HasReferencePoint(int jointID);

		// Outputs an inplace file of the bone's reference 
		NIUI_STATUS ExportCurrentReferenceInfo();

		void ClearCurrentReferenceInfo();

	};

}; // End Namespace NIUI


#endif // #ifndef NIUI_REFERENCE_ROTATION_CACHER_H