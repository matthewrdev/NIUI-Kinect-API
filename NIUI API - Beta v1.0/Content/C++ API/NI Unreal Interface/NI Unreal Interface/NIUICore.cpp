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
//**    NIUICore.cpp
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 01/2011
//**
//****************************************************************************

#include <XnOpenNI.h>
#include <XnCppWrapper.h>

#include "NIUI.h"
#include "NIUICore.h"
#include "NIUIEventManager.h"
#include "NIUICameraInputManager.h"

#include "NIUIUser.h"
#include "NIUIConversion.h"
#include "NIUIDataTypes.h"

#include "Timer.h"
#include "Logger.h"


namespace NIUI
{
	Core::Core()
		: m_users(NULL)
		, m_openNIInfo(NULL)
		, m_timer(NULL)
		, m_cameraInput(NULL)
	{
		m_xnCaps.Invalidate();
	}

	Core::~Core()
	{
		// Should never be used.
	}

	NIUI_STATUS Core::Initialise(const char* xmlPath, int maxUsers)
	{
		LOG("NIUI Core Initilisation has begun.", LOG_Init);

		// Create the timer.
		m_timer = new NIUI::Timer();
		m_timer->Tick();

		m_openNIInfo = new NIUI::OpenNIContextData();
		if (InitOpenNIContext(xmlPath) != NIUI_STATUS_OK)
		{
			LOG("Core has failed to initilise.", LOG_Error);
			return NIUI_STATUS_FAIL;
		}
		if (InitUsers(maxUsers) != NIUI_STATUS_OK)
		{
			LOG("NIUI users have failed to initilise.", LOG_Error);
			return NIUI_STATUS_FAIL;
		}

		m_cameraInput = new NIUI::CameraInputManager();
		m_cameraInput->Initialise(m_openNIInfo, maxUsers);

		return NIUI_STATUS_OK;
	}

	NIUI_STATUS Core::InitOpenNIContext(const char* xmlPath)
	{
		xn::EnumerationErrors errors;
		XnStatus result = XN_STATUS_OK;

		LOG("Core is initilising through an xml file.", LOG_Init);
		m_openNIInfo->context.Init();
		result = m_openNIInfo->context.InitFromXmlFile(xmlPath);
		
		if (result != XN_STATUS_OK)
		{
			XnChar strError[1024];
			errors.ToString(strError, 1024);
			printf("OpenNI init failed. Reason:");
			printf("%s\n", strError);
		}
		
		ERR_CHECK(result, "OpenNI Context failed to initilise.\n Is the device OpenNI capable and is it plugged in?", "OpenNI context creation was successful.", LOG_Error, LOG_Init);
	
		result = m_openNIInfo->context.FindExistingNode(XN_NODE_TYPE_DEPTH, m_openNIInfo->depthGenerator);
		if (result != XN_STATUS_OK)
		{
			LOG("No Depth-Map production node was declared in the NIUIConfig.xml.\nAttempting to create a Depth-Map production node from the OpenNI context.", LOG_Init);
			// Create the depth generator.
			result = m_openNIInfo->depthGenerator.Create(m_openNIInfo->context);
			ERR_CHECK(result, "Depth-Map production node failed to create.", "Depth-Map production node creation was successful.", LOG_Error, LOG_Init);
		}	
		else
		{
			LOG("Depth-Map production node has been successfully built from the NIUIConfig.xml file.", LOG_Init);
		}

		result = m_openNIInfo->context.FindExistingNode(XN_NODE_TYPE_USER, m_openNIInfo->userGenerator);
		if (result != XN_STATUS_OK)
		{
			LOG("No User Generator production node was declared in the NIUIConfig.xml.\n Attempting to create a User Generator production node from the OpenNI context.", LOG_Init);
			// Create the user generator.
			result = m_openNIInfo->userGenerator.Create(m_openNIInfo->context);
			ERR_CHECK(result, "User Generator production node failed to create.", "User Generator production node creation was successful.", LOG_Error, LOG_Init);
		}	
		else
		{
			LOG("User Generator production node has been successfully built from the NIUIConfig.xml file.", LOG_Init);
		}

		result = m_openNIInfo->context.FindExistingNode(XN_NODE_TYPE_IMAGE, m_openNIInfo->imageGenerator);
		if (result != XN_STATUS_OK)
		{
			LOG("No Image Generator production node was declared in the NIUIConfig.xml.\nAttempting to create a Image Generator production node from the OpenNI context.", LOG_Init);
			// Create the image generator.
			result = m_openNIInfo->imageGenerator.Create(m_openNIInfo->context);
			ERR_CHECK(result, "Image Generator production node failed to create.", "Image Generator production node creation was successful.", LOG_Error, LOG_Init);
		}	
		else
		{
			LOG("Image Generator production node has been successfully built from the NIUIConfig.xml file.", LOG_Init);
		}

		return NIUI_STATUS_OK;
	}

	NIUI_STATUS Core::InitUsers(int maxUsers)
	{
		if (maxUsers <= MAX_NIUI_USERS)
		{
			this->m_maxUsers = maxUsers;
		}
		else
		{
			LOG("THe requested number of users exceeds the user limit. Restricting to 4 users.", LOG_Warning);
			this->m_maxUsers = MAX_NIUI_USERS;
		}

		this->m_users = new User[maxUsers];
		for (int iter = 0; iter < maxUsers; iter++)
		{
			m_users[iter].Initialise();
		}
		return NIUI_STATUS_OK;
	}

	void Core::Start()
	{
		// Start generating events.
		m_openNIInfo->context.StartGeneratingAll();

		LOG("OpenNI context event generation has begun.", LOG_Init);
	}

	void Core::RegisterEventManager(EventManager* eventManager)
	{
		this->m_eventManager = eventManager;
	}

	void Core::Update(bool async)
	{
		m_timer->Tick();
		UpdateOpenNIContext(async);
		UpdateUsers();
	}

	void Core::UpdateOpenNIContext(bool async)
	{
		XnStatus result = XN_STATUS_OK;

		if (async) // Recomended you dont use async, might give you a purring 60fps but its likely the shit will hit the fan.
		{
			result = this->m_openNIInfo->context.WaitNoneUpdateAll();
		}
		else 
		{
			result = this->m_openNIInfo->context.WaitAndUpdateAll();
		}
	}

	void Core::UpdateUsers()
	{
		// Retrieve the bone information for each user.
		for (int i = 0; i < m_maxUsers; i++)
		{
			// Update the 
			if (m_users[i].GetState() == Tracking)
			{
				UpdateBonesOnUser(m_users[i]);
			}
		}
	}

	inline int Core::GetUserIndex(int ID)
	{
		for (int i = 0; i < m_maxUsers; i++)
		{
			if (m_users[i].m_xnID == ID)
				return i;
		}

		return -1;
	}

	void Core::UpdateBonesOnUser(User &user)
	{
		static XnSkeletonJointTransformation trans;
		static XnSkeletonJointPosition bones[3];
		static XnSkeletonJointPosition jointPos;
		static UDKVector3 centre;

		//return;
		XnUserID id = user.m_xnID;
		xn::SkeletonCapability skeletonCap = this->m_openNIInfo->userGenerator.GetSkeletonCap();

		// Pre-Cache the bone data array for the user.
		BoneState* boneBuffer = user.m_bones;

		skeletonCap.GetSkeletonJointPosition(id, XN_SKEL_TORSO, bones[0]);
		skeletonCap.GetSkeletonJointPosition(id, XN_SKEL_LEFT_HIP, bones[1]);
		skeletonCap.GetSkeletonJointPosition(id, XN_SKEL_RIGHT_HIP, bones[2]);

		jointPos.position.X = (bones[0].position.X + bones[1].position.X + bones[2].position.X) / 3.0f;
		jointPos.position.Y = (bones[0].position.Y + bones[1].position.Y + bones[2].position.Y) / 3.0f;
		jointPos.position.Z = (bones[0].position.Z + bones[1].position.Z + bones[2].position.Z) / 3.0f;

		XnPosToUnrealPos(jointPos, centre);

		// Firstly update the centre of mass.
		user.SetCentreOfMass(centre);
		
		// Get all bone transforms.
		for (int i = 0; i < NIUI::NIUI_JOINT_COUNT; i++)
		{
			skeletonCap.GetSkeletonJoint(user.m_xnID, (XnSkeletonJoint)(i + 1), trans);
			user.m_bones[i].confidence = trans.orientation.fConfidence;
			XnPosToUnrealPos(trans.position, user.m_bones[i].worldPos);
			XnPosToUnrealPos(trans.position, user.m_bones[i].localPos);
			user.m_bones[i].localPos.x -= centre.x;
			user.m_bones[i].localPos.y -= centre.y;
			user.m_bones[i].localPos.z -= centre.z;

			user.SetAxisFromXnMatrice(trans.orientation.orientation.elements, i);
		}
	}

	void Core::Shutdown()
	{
		if (m_timer != NULL)
		{
			delete m_timer;
			m_timer = NULL;
		}

		m_cameraInput->Release();
		if (m_cameraInput != NULL)
		{
			delete m_cameraInput;
			m_cameraInput = NULL;
		}

		// Release buffer allocations.
		if (m_users != NULL)
		{
			for (int i  = 0; i < m_maxUsers; i++)
			{
				// Destory the user.
				m_users[i].Destroy();
			}

			delete [] m_users;
		}

		// Close OpenNI context.
		m_openNIInfo->context.Shutdown();

		// Kill the openNI info.
		if (m_openNIInfo != NULL)
		{
			delete m_openNIInfo;
			m_openNIInfo = NULL;
		}
	}
	
	OpenNIContextData* Core::GetOpenNIData()
	{
		return this->m_openNIInfo;
	}

	void Core::Stop()
	{
		m_openNIInfo->context.StopGeneratingAll();
	}

	const OpenNIDeviceCaps& Core::GetDeviceCapabilies() const
	{
		return this->m_xnCaps;
	}

	// Determines if the users is both valid (IE not outside specified number of users) and is detected.
	bool Core::UserExists(int id)
	{
		for (int i = 0; i < m_maxUsers; i++)
		{
			if (m_users[i].m_xnID == id)
			{
				return m_users[i].GetState() != Lost;
			}
		}

		return false;
	}
	
	void Core::MarkUserDirty(int id)
	{
		for (int i = 0; i < m_maxUsers; i++)
		{
			if (m_users[i].m_xnID == id)
			{
				m_users[i].m_dirty = true;
				return;
			}
		}
	}

	bool Core::UserValid(int id)
	{
		return (id > 0 && id <= m_maxUsers);
	}

	
	int Core::FindNextInActiveUser()
	{
		for (int i = 0; i < m_maxUsers; i++)
		{
			if (m_users[i].m_xnID == INVALID_USER_ID)
			{
				return i;
			}
		}

		return INVALID_USER_ID;
	}

	void Core::GetUserWithID(int ID, NIUI::User* out_user)
	{
		// Validate the user ID.
		if (ID > 0 && ID <= this->m_maxUsers)
		{
			for (int i = 0; i < m_maxUsers; i++)
			{
				if (m_users[i].m_xnID == ID)
				{
					out_user = &m_users[i];
					return;
				}
			}
		}

		out_user = NULL;
	}

	void Core::StartPoseDetection(int userID)
	{
		if (UserValid(userID))
		{
			m_openNIInfo->userGenerator.GetPoseDetectionCap().StartPoseDetection("Psi", userID);
		}
	}

	void Core::StopPoseDetection(int userID)
	{
		if (UserValid(userID))
		{
			m_openNIInfo->userGenerator.GetPoseDetectionCap().StopPoseDetection(userID);
		}
	}

	void Core::StartCalibration(int userID)
	{
		if (UserValid(userID))
		{
			int index = GetUserIndex(userID);
			if (index != INVALID_USER_ID)
			{
				m_openNIInfo->userGenerator.GetPoseDetectionCap().StopPoseDetection(userID);
				m_openNIInfo->userGenerator.GetSkeletonCap().RequestCalibration(userID, TRUE);		
			}
		}
	}

	void Core::StopCalibration(int userID)
	{
		if (UserValid(userID))
		{
			// Precahce the UserGenerator node.
			xn::UserGenerator* userGen = &m_openNIInfo->userGenerator;

			int index = GetUserIndex(userID);
			if (index != INVALID_USER_ID)
			{
				userGen->GetSkeletonCap().AbortCalibration(userID);
			}
		}
	}	

	// 
	void Core::OnCalibrationStart(int userID)
	{
		if (UserExists(userID))
		{
			int index = GetUserIndex(userID);

			if (index != INVALID_USER_ID)
			{
				m_eventManager->AddEvent("CalibrationStart", userID);
				m_users[index].OnCalibrationStarted();
			}

		}
	}

	void Core::OnUserPoseDetected(int userID)
	{
		if (UserExists(userID))
		{
			m_eventManager->AddEvent("PoseDetected", userID);
		}
	}
	
	void Core::OnUserPoseLost(int userID)
	{
		if (UserValid(userID))
		{
			m_eventManager->AddEvent("PoseLost", userID);
		}

	}
	
	void Core::OnCalibrationEnd(int userID, bool successful, xn::SkeletonCapability& skeleton)
	{
		if (UserExists(userID))
		{
			int index = GetUserIndex(userID);
			if (successful && index != INVALID_USER_ID)
			{
				m_eventManager->AddEvent("CalibrationSuccess", userID);

				if (m_users[index].m_startTracking)
				{
					// Start tracking.
					skeleton.StartTracking(userID);
					m_users[index].OnCalibrationEnded(true);
					m_eventManager->AddEvent("TrackingStarted", userID);
				}
			}
			else if (index != INVALID_USER_ID)
			{
				m_eventManager->AddEvent("CalibrationFailed", userID);
				m_users[index].OnCalibrationEnded(false);
			}

		}
	}

	void Core::OnLostUser(int userID)
	{
		if (UserValid(userID))
		{
			int index = GetUserIndex(userID);

			if (index != INVALID_USER_ID)
			{
				m_eventManager->AddEvent("UserLost", userID);
				m_users[index].OnUserLost();
			}
		}
	}

	void Core::OnNewUser(int userID)
	{
		if (UserValid(userID) && !HasReachedUserCapacity())
		{
			if (!UserExists(userID))
			{
				int index = FindNextInActiveUser();

				if (index == INVALID_USER_ID)
				{
					LOG("User failed to be added to user list.", LOG_Warning);
					return;
				}
				m_eventManager->AddEvent("NewUser", userID);

				m_users[index].OnUserDetected(userID);
			}
		}
	}

	
	int Core::GetMaxNumberOfUsers()
	{
		return m_maxUsers;
	}

	int Core::GetNumberOfDetectedUsers()
	{
		int count = 0;
		for (int i = 0; i < m_maxUsers; i++)
		{
			if (m_users[i].GetState() != Lost)
				count++;
		}

		return count;
	}

	int Core::GetNumberOfTrackingUsers()
	{
		int count = 0;
		for (int i = 0; i < m_maxUsers; i++)
		{
			if (m_users[i].GetState() == Tracking)
				count++;
		}

		return count;
	}

	int Core::GetNumberOfCalibratingUsers()
	{
		int count = 0;
		for (int i = 0; i < m_maxUsers; i++)
		{
			if (m_users[i].GetState() == Calibrating)
				count++;
		}

		return count;
	}

	bool Core::HasReachedUserCapacity()
	{
		return GetNumberOfDetectedUsers() == m_maxUsers;
	}

	
	void Core::EnablePoseDetectionOnUser(int userID)
	{
		if (UserValid(userID))
		{
			int index = GetUserIndex(userID);

			if (index == INVALID_USER_ID)
				return;
			m_users[index].m_acceptPose = true;
			StartPoseDetection(userID);
		}
	}

	void Core::DisablePoseDetectionOnUser(int userID)
	{
		if (UserValid(userID))
		{
			int index = GetUserIndex(userID);

			if (index == INVALID_USER_ID)
				return;

			m_users[index].m_acceptPose = false;
			this->StopPoseDetection(userID);
		}
	}

	void Core::EnableCalibrationOnUser(int userID)
	{
		if (UserValid(userID))
		{
			int index = GetUserIndex(userID);

			if (index == INVALID_USER_ID)
				return;
			m_users[index].m_acceptCalibration = true;
		}
	}

	void Core::DisableCalibrationOnUser(int userID)
	{
		if (UserValid(userID))
		{
			int index = GetUserIndex(userID);

			if (index == INVALID_USER_ID)
				return;
			m_users[index].m_acceptCalibration = false;
		}
	}

	void Core::EnableTrackingOnUser(int userID)
	{
		if (UserValid(userID))
		{
			int index = GetUserIndex(userID);

			if (index == INVALID_USER_ID)
				return;
			m_users[index].m_startTracking = true;
		}
	}

	void Core::DisableTrackingOnUser(int userID)
	{
		if (UserValid(userID))
		{
			int index = GetUserIndex(userID);

			if (index == INVALID_USER_ID)
				return;
			m_users[index].m_startTracking = false;
		}
	}

	NIUI_STATUS Core::GetJointLocation(int userID, int joint, UDKVector3* vec)
	{
		if (joint >= NIUI_JOINT_COUNT)
			return NIUI_STATUS_FAIL;

		int index = GetUserIndex(userID);

		if (index == INVALID_USER_ID)
			return NIUI_STATUS_FAIL;

		vec->x = m_users[index].m_bones[joint].worldPos.x;
		vec->y = m_users[index].m_bones[joint].worldPos.y;
		vec->z = m_users[index].m_bones[joint].worldPos.z;

		return NIUI_STATUS_OK;
	}

	NIUI_STATUS Core::GetUserCentre(int userID, UDKVector3* vec)
	{
		int index = GetUserIndex(userID);

		if (index == INVALID_USER_ID)
			return NIUI_STATUS_FAIL;
		
		vec->x = m_users[index].m_centre.x;
		vec->y = m_users[index].m_centre.y;
		vec->z = m_users[index].m_centre.z;

		return NIUI_STATUS_OK;
	}

	NIUI_STATUS Core::GetLocalJointLocation(int userID, int joint, UDKVector3* vec)
	{

		if (joint >= NIUI_JOINT_COUNT)
		{
			return NIUI_STATUS_FAIL;
		}

		int index = GetUserIndex(userID);

		if (index == INVALID_USER_ID)
		{
			return NIUI_STATUS_FAIL;
		}
		
		vec->x = m_users[index].m_bones[joint].localPos.x;
		vec->y = m_users[index].m_bones[joint].localPos.y;
		vec->z = m_users[index].m_bones[joint].localPos.z;

		return NIUI_STATUS_OK;
	}

	
	NIUI_STATUS Core::GetBoneAxes(int userID, int joint, UDKVector3* forward, UDKVector3* side, UDKVector3* up)
	{
		if (joint >= NIUI_JOINT_COUNT)
		{
			return NIUI_STATUS_FAIL;
		}


		int index = GetUserIndex(userID);

		if (index == INVALID_USER_ID)
		{
			return NIUI_STATUS_FAIL;
		}

		m_users[index].GetBoneAxis(joint, forward, side, up);
		return NIUI_STATUS_OK;
	}

	

	NIUI_STATUS Core::OpenPreviewWindow(int xRes, int yRes)
	{
		return m_cameraInput->OpenPreviewWindow(xRes, yRes);
	}

	NIUI_STATUS Core::ClosePreviewWindow()
	{
		return m_cameraInput->ClosePreviewWindow();
	}

	NIUI_STATUS Core::AddCameraInputToWindow(int theType, int* out_handle)
	{
		return m_cameraInput->AddCameraInput(theType, out_handle);
	}

	NIUI_STATUS Core::RemoveFeedFromWindow(int handle)
	{
		return m_cameraInput->RemoveCameraInput(handle);
	}
		
	CameraInputManager* Core::GetCameraInputManager()
	{
		return m_cameraInput;
	}

	NIUI_STATUS Core::ApplyDepthMapToBuffer(BYTE* udkPixels, int xRes, int yRes)
	{
		return m_cameraInput->ApplyToUDKTexture(udkPixels, xRes, yRes, 3, CFeed_DepthMap);
	}

	NIUI_STATUS Core::ApplyImageMapToBuffer(BYTE* udkPixels, int xRes, int yRes)
	{
		return m_cameraInput->ApplyToUDKTexture(udkPixels, xRes, yRes, 3, CFeed_ImageMap);
	}

	NIUI_STATUS Core::ApplyUserColorMapToBuffer(BYTE* udkPixels, int xRes, int yRes)
	{
		return m_cameraInput->ApplyToUDKTexture(udkPixels, xRes, yRes, 3, CFeed_UserColorMap);
	}

}; // End Namespace NIUI
