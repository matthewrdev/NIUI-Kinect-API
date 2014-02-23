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
//**    NIUI Library Export.cpp
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 01/2011
//**
//****************************************************************************

#include "NIUI Library Export.h"

#include <XnOpenNI.h>
#include <XnCppWrapper.h>
#include <XnTypes.h>
#include <wchar.h>

#include "NIUI.h"
#include "NIUICore.h"
#include "NIUIEventManager.h"
#include "NIUIReferenceRotationCacher.h"
#include "NIUICameraInputManager.h"
#include "NIUIUser.h"
#include "NIUIDataTypes.h"
#include "NIUIConversion.h"
#include "NIUIUtil.h"
#include "Logger.h"

using namespace NIUI;

						
static Core g_core;
static EventManager g_eventManager;
static ReferenceRotationCacher g_refRotationCacher;

XnChar g_strPose[20] = "";

// Dummy stuff as far as i am aware.
static XnCallbackHandle		g_hUserCallbacks		= NULL;
static XnCallbackHandle		g_hCalibrationCallbacks = NULL;

XnCallbackHandle hUserCallbacks, hCalibrationCallbacks, hPoseCallbacks;

//-----------------------------------------------------------------------------
// OpenNI event handlers
//-----------------------------------------------------------------------------

static void XN_CALLBACK_TYPE OnNewUser(xn::UserGenerator& generator, const XnUserID nUserId, void* pCookie)
{
	//printf("OnNewUser. %i", nUserId);
	g_core.OnNewUser(nUserId);
}

static void XN_CALLBACK_TYPE OnLostUser(xn::UserGenerator& generator, const XnUserID nUserId, void* pCookie)
{	
	//printf("OnLostUser.");
	g_core.OnLostUser(nUserId);
}

// Callback: Detected a pose
static void XN_CALLBACK_TYPE UserPoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie)
{
	//printf("UserPoseDetected.");
	g_core.OnUserPoseDetected(nId);
}

static void XN_CALLBACK_TYPE UserPoseLost(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie)
{
	//printf("UserPoseLost.");
	g_core.OnUserPoseLost(nId);
}

static void XN_CALLBACK_TYPE OnCalibrationStart(xn::SkeletonCapability& skeleton, const XnUserID nUserId, void* pCookie)
{
	//printf("OnCalibrationStart.");
	g_core.OnCalibrationStart(nUserId);
}

static void XN_CALLBACK_TYPE OnCalibrationEnd(xn::SkeletonCapability& skeleton, const XnUserID nUserId, XnBool bSuccess, void* pCookie)
{
	//printf("OnCalibrationEnd.");
	g_core.OnCalibrationEnd(nUserId, bSuccess == 1, skeleton);
}

// ------------------------------------------------------------
// ------------------------------------------------------------
// Thread for updating the camera output.
// Allow the update to happen async, which is important as it 
// uses a software renderer.
// ------------------------------------------------------------
// ------------------------------------------------------------

extern "C" DWORD CameraUpdateThreadFunction(LPVOID param);
static bool updateCamera = true;
static bool cameraRequiresUpdate = true;
HANDLE cameraThread = 0;

void StartCamerUpdateThread()
{
	LOG("Camera update thread has been triggered to start.", LOG_Standard);

	if (cameraThread != 0)
	{
		// Try close the existing thread.
		if (!CloseHandle(cameraThread))
		{
			LOG("Could not close camera update thread.", LOG_Error);
		}
	}

	updateCamera = true;
	cameraThread = CreateThread( 
		NULL,										// default security attributes
		0,											// use default stack size  
		(LPTHREAD_START_ROUTINE)CameraUpdateThreadFunction,	// thread function name
		NULL,										// argument to thread function 
		0,											// use default creation flags 
		NULL);


}


DWORD CameraUpdateThreadFunction(LPVOID param)
{
	CameraInputManager* cameraManager = g_core.GetCameraInputManager();
	LOG("Camera update thread has been entered.", LOG_Standard);
	while (updateCamera)
	{
		if (cameraRequiresUpdate)
		{
			cameraManager->Update();
		}
		else
		{
			Sleep(1);
		}
	}

	return 0;
}

// ------------------------------------------------------------
// ------------------------------------------------------------
// The core C wrapper functions for accessing the NIUI library.
// ------------------------------------------------------------
// ------------------------------------------------------------

/** Initilises the OpenNI Context.  */  
int InitiliseNIUI(int requestedNumberOfPlayers)
{
	XnStatus result = XN_STATUS_OK;

	NIUI_STATUS status = NIUI_STATUS_OK;

	status = g_core.Initialise("NIUIConfig.xml", requestedNumberOfPlayers);
	if (status != NIUI_STATUS_OK)
		return NIUI_STATUS_FAIL;

	// Bind the event manager.
	g_core.RegisterEventManager(&g_eventManager);
	g_eventManager.Initialise(100);

	// Register callbacks.
	NIUI::OpenNIContextData* openNIInfo = g_core.GetOpenNIData();
	
	LOG("Registering user detection and calibration callbacks.", LOG_Init);
	openNIInfo->userGenerator.RegisterUserCallbacks(OnNewUser, OnLostUser, NULL, g_hUserCallbacks);
	openNIInfo->userGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
	openNIInfo->userGenerator.GetSkeletonCap().RegisterCalibrationCallbacks(&OnCalibrationStart, &OnCalibrationEnd, NULL, g_hCalibrationCallbacks);
	openNIInfo->userGenerator.GetSkeletonCap().SetSmoothing(0.5);

	openNIInfo->userGenerator.GetPoseDetectionCap().RegisterToPoseCallbacks(UserPoseDetected, UserPoseLost, NULL, hPoseCallbacks);
	openNIInfo->userGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
	
	LOG("NIUI.dll initilisation was successful.", LOG_Init);

	g_core.Start();

	LOG("Initilising ReferenceRotationCacher.", LOG_Init);
	g_refRotationCacher.Initialise();

	return NIUI_STATUS_OK;
}

/** Closes the OpenNI Context and deletes references.  */ 
void ReleaseNIUI()
{
	LOG("Event generation is being stopped.", LOG_Shutdown);
	g_core.Stop();


	LOG("Callbacks are being unregistered.", LOG_Shutdown);
	// Unregister all the callbacks.
	if (NULL != hPoseCallbacks)
	{
		g_core.GetOpenNIData()->userGenerator.GetPoseDetectionCap().UnregisterFromPoseCallbacks(hPoseCallbacks);
		hPoseCallbacks = NULL;
	}

	if (NULL != g_hCalibrationCallbacks)
	{
		g_core.GetOpenNIData()->userGenerator.GetSkeletonCap().UnregisterCalibrationCallbacks(g_hCalibrationCallbacks);
		g_hCalibrationCallbacks = NULL;
	}

	if (NULL != g_hUserCallbacks)
	{
		g_core.GetOpenNIData()->userGenerator.GetSkeletonCap().UnregisterCalibrationCallbacks(g_hUserCallbacks);
		g_hUserCallbacks = NULL;
	}

	// Destory core.
	LOG("Releasing NIUICore.", LOG_Shutdown);
	g_core.Shutdown();
	g_eventManager.Destroy();

	//gLogger.DumpLog();
	//gLogger.EmptyLog();
}

/** Updates the NIUI and OpenNI contexts.  */
void UpdateNIUI()
{
	g_core.Update(false); // No async yet, will reserve this for final release.
	g_core.GetCameraInputManager()->Update();
	g_core.GetCameraInputManager()->Draw();
	//cameraRequiresUpdate = true;
}

void GetSkeletalJointLocation(int userID, int joint, UDKVector3* out_location)
{
	g_core.GetJointLocation(userID, joint, out_location);
}


void GetLocalSkeletalJointLocation(int userID, int joint, UDKVector3* out_location)
{
	g_core.GetLocalJointLocation(userID, joint, out_location);
}

void GetUserCentre(int userID, UDKVector3* out_location)
{
	g_core.GetUserCentre(userID, out_location);
}

void GetBoneAxes(int userID, int joint, UDKVector3* out_forward, UDKVector3* out_side, UDKVector3* out_up)
{
	g_core.GetBoneAxes(userID, joint, out_forward, out_side, out_up);
}

bool IsUserDetected(int userID)
{
	User* user = NULL;
	g_core.GetUserWithID(userID, user);
	if (user != NULL)
	{
		return user->GetState() != Lost;
	}
	return false;
}

bool IsUserCalibrating(int userID)
{
	User* user = NULL;
	g_core.GetUserWithID(userID, user);
	if (user != NULL)
	{
		return user->GetState() == Calibrating;
	}

	return false;
}

bool IsUserTracking(int userID)
{
	User* user = NULL;
	g_core.GetUserWithID(userID, user);
	if (user != NULL)
	{
		return user->GetState() == Tracking;
	}

	return false;
}

bool IsUserInPose(int userID)
{
	// Dummy function. Use the event pump.
	return false;
}

/** Starts calibration pose detection.*/
void StartPoseDetection(int userID)
{
	g_core.StartPoseDetection(userID);
}

void StopPoseDetection(int userID)
{
	g_core.StopPoseDetection(userID);
}

void StartCalibration(int userID)
{
	g_core.StartCalibration(userID);
}

void StopCalibration(int userID)
{
	g_core.StopCalibration(userID);
}

/** Configures the NIUI core to either accept or discard pose / calibration tests.*/
void AllowPoseDetection(int enable, int userID)
{
	if (enable)
	{
		g_core.EnablePoseDetectionOnUser(userID);
	}
	else
	{
		g_core.DisablePoseDetectionOnUser(userID);
	}
}

void AllowCalibration(int enable, int userID)
{
	if (enable)
	{
		g_core.EnablePoseDetectionOnUser(userID);
	}
	else
	{
		g_core.DisablePoseDetectionOnUser(userID);
	}
}

void AllowTracking(int enable, int userID)
{
	if (enable)
	{
		g_core.EnablePoseDetectionOnUser(userID);
	}
	else
	{
		g_core.DisablePoseDetectionOnUser(userID);
	}
}

/** Configures the NIUI core to accept pose, calibration and tracking commands on the user.*/
void ActivateUser(int userID)
{
	g_core.EnablePoseDetectionOnUser(userID);
	g_core.EnableCalibrationOnUser(userID);
	g_core.EnableTrackingOnUser(userID);
}

/** Gets the flat data mirror of the users state.*/
void GetUserMirror(int userID, UserMirror* out_mirror)
{
	// Not implemented yet. To come when I fix NIUI's memory fragmentation.
}

/** Determines if the user is "dirty", that is either lost but not marked lost.*/
bool IsUserDirty(int userID)
{
	return false; // Unimplemented.
}

/** Functions to retrieve the total amount of users in a certain state.*/
int GetNumberOfTrackedUsers()
{
	return g_core.GetNumberOfTrackingUsers();
}

int GetNumberOfCalibratingUsers()
{
	return g_core.GetNumberOfCalibratingUsers();
}

int GetNumberOfDetectedUsers()
{
	return g_core.GetNumberOfDetectedUsers();
}

int GetMaxNumberOfUsers()
{
	return g_core.GetMaxNumberOfUsers();
}

/** Changes the behaviour of NIUI's debug dumping.*/
void SetDebugInfoLevel(int level)
{
	gLogger.SetLogLevel((LogLevel)level);
	
}

void SetDebugFileDump(int enable)
{
	gLogger.EnableFileDumping(enable == 1);
}

void SetConsoleDump(int enable)
{
	gLogger.EnableConsoleDumping(enable == 1);
}

/** Event processing.*/
int GetTotalEvents()
{
	return g_eventManager.GetEventCount();
}

void GetEvent(int number, int* out_user, int* out_event)
{
	static Event theEvent;
	if (g_eventManager.GetEvent(number, theEvent))
	{
		*out_user = theEvent.user;
		*out_event = theEvent.theEvent;
	}
}

void ClearEvents()
{
	g_eventManager.ClearEvents();
}

void EnableEventDumping(int enable)
{
	g_eventManager.EnableEventLogging(enable == 1);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// The C wrapper functions for using the reference rotation caching system.
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------

int LoadRotationReferenceFile()
{
	return g_refRotationCacher.ImportReferenceDataFile();
}

void SetRotationReferenceFileName(UDKString* theMeshName)
{
	g_refRotationCacher.SetOutputMeshName(std::wstring(theMeshName->Data));
}

int ExportCurrentRotationReferenceInfo()
{
	return g_refRotationCacher.ExportCurrentReferenceInfo();
}

void ClearRotationReferenceInfo()
{
	g_refRotationCacher.ClearCurrentReferenceInfo();
}

void GetBoneRotationReferenceQuat(int boneID, UDKQuaternion* out_quat)
{
	g_refRotationCacher.GetBoneReferencePoint(boneID, out_quat);
}

void SetBoneRotationReferenceQuat(int boneID, UDKQuaternion* quat)
{
	g_refRotationCacher.SetBoneReferencePoint(boneID, quat);
}

int HasRotationReferenceInfo(int boneID)
{
	return g_refRotationCacher.HasReferencePoint(boneID);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// The C wrapper functions for using the Preview Display Window.
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------

int OpenPreviewWindow(int width, int height)
{
	return g_core.OpenPreviewWindow(width, height);
}

int ClosePreviewWindow()
{
	return g_core.ClosePreviewWindow();
}

int AddCameraInput(int type, int* out_handle)
{
	return g_core.AddCameraInputToWindow(type, out_handle);
}

int RemoveCameraInput(int handle)
{
	return g_core.RemoveFeedFromWindow(handle);
}

void SetCameraInputPosition(int handle, int x, int y)
{
	g_core.GetCameraInputManager()->SetCameraInputPosition(handle, x, y);
}

void SetCameraInputScale(int handle, float x, float y)
{
	g_core.GetCameraInputManager()->SetCameraInputScale(handle, x, y);
}

void GetCameraInputPosition(int handle, int* out_x, int* out_y)
{
	g_core.GetCameraInputManager()->GetCameraInputPosition(handle, out_x, out_y);
}

void GetCameraInputScale(int handle, float* out_x, float* out_y)
{
	g_core.GetCameraInputManager()->GetCameraInputScale(handle, out_x, out_y);
}

void SetUserColor(int userID, unsigned char r, unsigned char g, unsigned char b)
{
	g_core.GetCameraInputManager()->SetUserColor(userID, r,g,b);
} 


// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// Test functions for aiding the development of the API.
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------

void ReadBuffer(int bufferPointer, int pixelsToRead)
{
	BYTE a,r,g,b;

	static char output[64] = {0};

	// Cast the int to a pointer.
	BYTE* buffer = (BYTE*)bufferPointer;
	for (unsigned int i= 0; i <  pixelsToRead; i+=4)
	{
		a = buffer[i];
		r = buffer[i+1];
		g = buffer[i+2];
		b = buffer[i+3];

		memset(output, 0, 64);
		sprintf(output, "A:%i, R:%i, G:%i, B:%i", (int)a, (int)r, (int)g, (int)b);
		puts(output);
	}
}
