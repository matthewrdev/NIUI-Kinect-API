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

#ifndef NIUI_USER_H
#define NIUI_USER_H

//****************************************************************************
//**
//**    NAME
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 01/2011
//**
//****************************************************************************

#include "NIUIDataTypes.h"

// Forward Declarations
namespace NIUI
{
	class Core;
}; // End Namespace NIUI


namespace NIUI
{
	const int INVALID_USER_ID = -1;

	struct BoneState
	{
		// Rotation of bone in Unreal Units.
		UDKVector3 localPos;
		UDKVector3 worldPos;
		UDKVector3 forwardAxis;
		UDKVector3 sideAxis;
		UDKVector3 upAxis;
		float confidence;
	};

	enum UserState
	{
		Detected,
		Calibrating,
		Lost,
		Tracking,
	};

	// ------------------------------------------------------------------------
	//								User
	// ------------------------------------------------------------------------

	class User
	{
	private:

		UserState m_state;
		BoneState* m_bones;
		int m_xnID;

		UDKVector3 m_centre;
		
		// If to allow pose detection on the user.
		bool m_acceptPose;

		// If to allow calibration on the user.
		bool m_acceptCalibration;

		// If to allow user tracking to commence when a calibration ends.
		bool m_startTracking;

		void Initialise();
		void Destroy();

		void OnUserDetected(int newUserID);
		void OnUserLost();
		void OnCalibrationStarted();
		void OnCalibrationEnded(bool succesful);

		void SetCentreOfMass( const UDKVector3 &com);
		void SetAxisFromXnMatrice(float* elements, int bone);

		float m_timeSinceLastUpdate;
		bool m_dirty;

		friend class NIUI::Core;
	protected:
	public:

		User();
		~User();

		UserState GetState() const;
		BoneState GetBoneState(int bone);
		float GetBoneConfidence(int bone);

		UDKVector3 GetCentre();

		void AllowPoseDetection(bool allow);
		void AllowCalibration(bool allow);
		void AllowTracking(bool allow);

		void GetBoneAxis(int bone, UDKVector3* forward, UDKVector3* side, UDKVector3* up);
	};

}; // End Namespace NIUI


#endif // #ifndef NIUI_USER_H