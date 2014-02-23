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

#ifndef NIUI_CORE_H
#define NIUI_CORE_H

//****************************************************************************
//**
//**    NIUICore.h
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 01/2011
//**
//****************************************************************************

#include "NIUI.h"

// Forward Declarations
namespace NIUI
{
	class User;
	class Timer;
	class EventManager;
	class CameraInputManager;

}; // End Namespace NIUI

namespace xn
{
	class SkeletonCapability;
}; // End Namespace xn.

extern "C"
{
	struct UDKVector3;
	struct UDKRotator;
};

namespace NIUI
{
	struct OpenNIDeviceCaps
	{
		int depthWidth, depthHeight;
		int vgaWidth, vgaHeight;

		void Invalidate()
		{
			depthWidth = depthHeight = vgaWidth = vgaHeight = 0;
		}
	};

	struct UserHashPair;
	struct OpenNIContextData;

	// ------------------------------------------------------------------------
	//								Core
	// ------------------------------------------------------------------------

	class Core
	{
	private:

		OpenNIDeviceCaps m_xnCaps;
		OpenNIContextData* m_openNIInfo;
		Timer* m_timer;
		EventManager* m_eventManager;
		CameraInputManager* m_cameraInput;

		NIUI::User* m_users;
		int m_maxUsers;

		// If to allow calibration requests on users when a pose is detected.
		bool m_acceptCalibration;
		float m_timeToMarkDirty;

		// Private init calls.
		NIUI_STATUS InitOpenNIContext(const char* xmlPath);
		NIUI_STATUS InitUsers(int maxUsers);

		// Private update calls
		void UpdateOpenNIContext(bool async);
		void UpdateUsers();
		void UpdateBonesOnUser(User& user);

		// User validation functions.
		bool UserValid(int id);
		bool UserExists(int id);

		void MarkUserDirty(int id);

		int FindNextInActiveUser();

		inline int GetUserIndex(int ID);

	protected:
	public:

		Core();
		~Core();

		// Begins initilisation of OpenNI. 
		NIUI_STATUS Initialise(const char* xmlpath, int maxUsers);

		// Closes all openNI devices.
		void Shutdown();

		void RegisterEventManager(EventManager* theEventManager);

		// Starts the OpenNI context to generate events.
		void Start();
		void Stop();

		OpenNIContextData* GetOpenNIData();

		// Sets the maximum amount of time that is allowed for a user to be lost before it is marked as "dirty".
		void SetDirtyMarkTime(float time);

		// Highly recomended that you dont use async updating as it may fall out of sync.
		// Also prevents threading based memory errors. NIUI has not been designed to handle threading.
		void Update(bool async = false);

		const OpenNIDeviceCaps& GetDeviceCapabilies() const;

		// Returns the user of xnID "ID". It is recomended to precache this if you are performing a large amount of operations.
		void GetUserWithID(int ID, NIUI::User* out_user);

		// Methods for OpenNI callbacks to interface with the NIUI core.
		void OnCalibrationStart(int userID);
		void OnCalibrationEnd(int userID, bool succesful, xn::SkeletonCapability& skeleton);
		void OnUserPoseDetected(int userID);
		void OnUserPoseLost(int userID);
		void OnLostUser(int userID);
		void OnNewUser(int userID);

		void StartPoseDetection(int userID);
		void StopPoseDetection(int userID);

		void StartCalibration(int userID);
		void StopCalibration(int userID);

		int GetMaxNumberOfUsers();

		int GetNumberOfDetectedUsers(); // O(n). Linearly counts number of detected users.
		int GetNumberOfTrackingUsers();// O(n). Linearly counts number of tracked users.
		int GetNumberOfCalibratingUsers();// O(n). Linearly counts number of calibrating users.
		bool HasReachedUserCapacity();

		void EnablePoseDetectionOnUser(int userID);
		void DisablePoseDetectionOnUser(int userID);

		void EnableCalibrationOnUser(int userID);
		void DisableCalibrationOnUser(int userID);

		void EnableTrackingOnUser(int userID);
		void DisableTrackingOnUser(int userID);

		NIUI_STATUS GetJointLocation(int userID, int joint, UDKVector3* vec);
		NIUI_STATUS GetLocalJointLocation(int userID, int joint, UDKVector3* vec);
		NIUI_STATUS GetUserCentre(int userID, UDKVector3* vec);
		NIUI_STATUS GetBoneAxes(int userID, int joint, UDKVector3* forward, UDKVector3* side, UDKVector3* up);

		NIUI_STATUS OpenPreviewWindow(int xRes, int yRes);
		NIUI_STATUS ClosePreviewWindow();

		NIUI_STATUS AddCameraInputToWindow(int theType, int* out_handle);
		NIUI_STATUS RemoveFeedFromWindow(int handle);
		
		CameraInputManager* GetCameraInputManager();

		NIUI_STATUS ApplyDepthMapToBuffer(BYTE* udkPixels, int xRes, int yRes);
		NIUI_STATUS ApplyImageMapToBuffer(BYTE* udkPixels, int xRes, int yRes);
		NIUI_STATUS ApplyUserMapToBuffer(BYTE* udkPixels, int xRes, int yRes);
		NIUI_STATUS ApplyUserColorMapToBuffer(BYTE* udkPixels, int xRes, int yRes);
	};

}; // End Namespace NIUI

#endif // #ifndef NIUI_CORE_H