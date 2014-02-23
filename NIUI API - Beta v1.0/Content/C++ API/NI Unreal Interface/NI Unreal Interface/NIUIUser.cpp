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
//**    NIUIUser.cpp
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 01/2011
//**
//****************************************************************************

#include "NIUIUser.h"

#include <XnOpenNI.h>
#include <XnCppWrapper.h>
#include "NIUIConversion.h"
#include "Logger.h"

namespace NIUI
{
	
	User::User()
		: m_bones(NULL)
		, m_state(Lost)
		, m_startTracking(false)
		, m_acceptPose(false)
		, m_acceptCalibration(false)
		, m_xnID(INVALID_USER_ID)
	{
	}

	User::~User()
	{
	}

	void User::Initialise()
	{
		m_bones = new BoneState[NIUI_JOINT_COUNT];
		m_xnID = INVALID_USER_ID;
		m_state = Lost;
	}

	void User::Destroy()
	{
		if (m_bones != NULL)
		{
			delete [] m_bones;
			m_bones = NULL;
		}
	}

	void User::OnUserDetected(int newUserID)
	{
		printf("NIUI: User %i was detected.\n", newUserID);
		this->m_xnID = newUserID;
		this->m_state = Detected;
		m_dirty = false;
	}

	void User::OnUserLost()
	{
		printf("NIUI: User %i was lost.\n", m_xnID);
		this->m_xnID = NIUI::INVALID_USER_ID;
		this->m_state = Lost;
		m_dirty = true;
	}

	void User::OnCalibrationStarted()
	{
		printf("NIUI: User %i calibration has begun.\n", m_xnID);
		this->m_state = Calibrating;
		m_dirty = false;
	}

	void User::OnCalibrationEnded(bool succesful)
	{
		if (succesful)
		{
			printf("User %i has been successfull calibrated.\n", m_xnID);
			this->m_state = Tracking;
		}
		else
		{
			printf("User %i calibration has failed.");
			this->m_state = Detected;
		}
	}

	void User::SetCentreOfMass(const UDKVector3 &com)
	{
		m_centre.x = com.x;
		m_centre.y = com.y;
		m_centre.z = com.z;
	}


	UserState User::GetState() const
	{
		return m_state;
	}

	BoneState User::GetBoneState(int bone)
	{
		return m_bones[bone];
	}
	
	UDKVector3 User::GetCentre()
	{
		return this->m_centre;
	}
	
	void User::AllowPoseDetection(bool allow)
	{
		this->m_acceptPose = allow;
	}

	void User::AllowCalibration(bool allow)
	{
		this->m_acceptCalibration = allow;
	}

	void User::AllowTracking(bool allow)
	{
		m_startTracking = allow;
	}

	void User::SetAxisFromXnMatrice(float* elements, int bone)
	{
		// OpenNI is column major. 
		// Unreal's co-ordinate system is:
		// Y = left to right is positive
		// X = origin to forwards is positive
		// Z = origin to up is positive
		if (bone < NIUI::NIUI_JOINT_COUNT)
		{
			ExtractBasisVectors(elements, &m_bones[bone].sideAxis, &m_bones[bone].forwardAxis, &m_bones[bone].upAxis);
		}
	}

	
	void User::GetBoneAxis(int bone, UDKVector3* forward, UDKVector3* side, UDKVector3* up)
	{
		if (bone < NIUI::NIUI_JOINT_COUNT)
		{
			// Copy forward axis.
			forward->x = m_bones[bone].forwardAxis.x;
			forward->y = m_bones[bone].forwardAxis.y;
			forward->z = m_bones[bone].forwardAxis.z;

			// Copy side axis.
			side->x = m_bones[bone].sideAxis.x;
			side->y = m_bones[bone].sideAxis.y;
			side->z = m_bones[bone].sideAxis.z;

			// Copy up axis.
			up->x = m_bones[bone].upAxis.x;
			up->y = m_bones[bone].upAxis.y;
			up->z = m_bones[bone].upAxis.z;
		}
	}

}; // End Namespace NIUI
