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

#ifndef NIUI_LIBRARY_EXPORT_H
#define NIUI_LIBRARY_EXPORT_H

//****************************************************************************
//**
//**    NI Unreal Interface.h
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 01/2011
//**
//****************************************************************************

//---------------------------------------------------------------------------
// API Export/Import Macros
//---------------------------------------------------------------------------
/** Indicates an exported shared library function. */ 
#define NIUI_API_EXPORT __declspec(dllexport)

/** Indicates an imported shared library function. */ 
#define NIUI_API_IMPORT __declspec(dllimport)

// Declare an API export.
#define NIUI_EXPORT

// API Exporting/ Importing defines.
#ifdef __cplusplus
	#define NIUI_C_API_EXPORT extern "C" NIUI_API_EXPORT
	#define NIUI_C_API_IMPORT extern "C" NIUI_API_IMPORT
	#define NIUI_CPP_API_EXPORT NIUI_API_EXPORT
	#define NIUI_CPP_API_IMPORT NIUI_API_IMPORT
#else
	#define NIUI_C_API_EXPORT NIUI_API_EXPORT
	#define NIUI_C_API_IMPORT NIUI_API_IMPORT
#endif

#ifdef NIUI_EXPORT
	#define NIUI_C_API NIUI_C_API_EXPORT
	#define NIUI_CPP_API NIUI_CPP_API_EXPORT
#else
	#define NIUI_C_API NIUI_C_API_IMPORT
	#define NIUI_CPP_API NIUI_CPP_API_IMPORT
#endif

typedef void (__stdcall * pfn_user_callback)(int nUserId);
typedef void (__stdcall * pfn_focus_callback)(bool focused);
typedef void (__stdcall * pfn_handpoint_callback)(float x, float y, float z);
typedef void (__stdcall * pfn_item_callback)(int item_index, int direction);
typedef void (__stdcall * pfn_value_callback)(float value);

// Data type Forward declarations for the C wrapper.
extern "C"
{
	struct UDKRotator;
	struct UDKVector3;
	struct UDKQuaternion;
	struct UserMirror;
	struct UDKString;
}

// ------------------------------------------------------------
// ------------------------------------------------------------
// The core C wrapper functions for accessing the NIUI library.
// ------------------------------------------------------------
// ------------------------------------------------------------

/** Initilises the OpenNI Context.  */  
NIUI_C_API int InitiliseNIUI(int requestedNumberOfPlayers);

/** Closes the OpenNI Context and deletes references.  */ 
NIUI_C_API void ReleaseNIUI();

/** Updates the NIUI and OpenNI context */
NIUI_C_API void UpdateNIUI();

/** User skelton joint information buffer information */
NIUI_C_API void GetSkeletalJointLocation(int userID, int joint, UDKVector3* out_location);
NIUI_C_API void GetLocalSkeletalJointLocation(int userID, int joint, UDKVector3* out_location);
NIUI_C_API void GetUserCentre(int userID, UDKVector3* out_location);
NIUI_C_API void GetBoneAxes(int userID, int joint, UDKVector3* out_forward, UDKVector3* out_side, UDKVector3* out_up);

/** User state polling. Recomended that you use the event processing instead to determine this.*/
NIUI_C_API bool IsUserDetected(int userID);
NIUI_C_API bool IsUserCalibrating(int userID);
NIUI_C_API bool IsUserTracking(int userID);
NIUI_C_API bool IsUserInPose(int userID);

/** Starts calibration pose detection.*/
NIUI_C_API void StartPoseDetection(int userID);
NIUI_C_API void StopPoseDetection(int userID);

/** Starts calibration on users.*/
NIUI_C_API void StartCalibration(int userID);
NIUI_C_API void StopCalibration(int userID);

/** Configures the NIUI core to either accept or discard pose / calibration tests.
*   Use this to flag calibration and user tracking to automatically begin when a
*   user calibrates or a pose is detected.*/
NIUI_C_API void AllowPoseDetection(int enable, int userID);
NIUI_C_API void AllowCalibration(int enable, int userID);
NIUI_C_API void AllowTracking(int enable, int userID);

/** Configures the NIUI core to accept pose, calibration and tracking commands on the user
*   Flags pose detection, calibration and user tracking to begin automatically when calbacks
*   are activated.*/
NIUI_C_API void ActivateUser(int userID);

/** Gets the flat data mirror of the users state.*/
NIUI_C_API void GetUserMirror(int userID, UserMirror* out_mirror);

/** Determines if the user is "dirty", that is either lost but not marked lost.*/
NIUI_C_API bool IsUserDirty(int userID);

/** Functions to retrieve the total amount of users in a certain state.*/
NIUI_C_API int GetNumberOfTrackedUsers();
NIUI_C_API int GetNumberOfCalibratingUsers();
NIUI_C_API int GetNumberOfDetectedUsers();
NIUI_C_API int GetMaxNumberOfUsers();

/** Changes the behaviour of NIUI's debug dumping.*/
NIUI_C_API void SetDebugInfoLevel(int level);
NIUI_C_API void SetDebugFileDump(int enable);
NIUI_C_API void SetConsoleDump(int enable);

/** Event processing.*/
NIUI_C_API int GetTotalEvents();
NIUI_C_API void GetEvent(int number, int* out_user, int* out_event);
NIUI_C_API void ClearEvents();

/** Activates/Deactivates console dumping of events as they are recieved. Recommended that you have this off.*/
NIUI_C_API void EnableEventDumping(int enable);

/** Starts a seperate thread that will continue to track users. Use this when a new level is loading to hold onto user calibration*/
NIUI_C_API void StartIdleMode();

/** Stops the seperate thread used to track users while loading a level. */
NIUI_C_API void StopIdleMode();

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// The C wrapper functions for using the reference rotation caching system.
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------

/** Tries to load the rotation reference point file of the inputted mesh. 
	Return 1 if succesful, 0 if not. */
NIUI_C_API int LoadRotationReferenceFile();

/** Sets the name of the mesh that will be outputted as a rotation reference file. */
NIUI_C_API void SetRotationReferenceFileName(UDKString* theMeshName);

/** Exports the current information in the ReferenceRotationCacher into a .rrf file. */
NIUI_C_API int ExportCurrentRotationReferenceInfo();

/** Invalidates all the information currently held in the ReferenceRotationCacher. */
NIUI_C_API void ClearRotationReferenceInfo();

/** Tries to access the reference rotation information for the inputted bone of the last loaded .rrf file for a mesh. */
NIUI_C_API void GetBoneRotationReferenceQuat(int boneID, UDKQuaternion* out_quat);

/** Sets the reference rotation information for the inputted bone. */
NIUI_C_API void SetBoneRotationReferenceQuat(int boneID, UDKQuaternion* quat);

/** Query's to see if the inputted bone has a reference rotation. */
NIUI_C_API int HasRotationReferenceInfo(int boneID);

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// The C wrapper functions for using the Preview Display Window.
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------

/** Opens the external preview window. All camera feeds will appear here.*/
NIUI_C_API int OpenPreviewWindow(int width, int height);

/** Shuts the external preview window. */
NIUI_C_API int ClosePreviewWindow();

/** Adds a camera output feed of the inputted type. */
NIUI_C_API int AddCameraInput(int type, int* out_handle);

/** Finds and removes the camera output from being displayed on the preview window. */
NIUI_C_API int RemoveCameraInput(int handle);

/** Sets the inputted camera input handle position in screen space.*/
NIUI_C_API void	SetCameraInputPosition(int handle, int x, int y);

/** Sets the inputted camera input handle scale.*/
NIUI_C_API void SetCameraInputScale(int handle, float x, float y);

/** Gets the inputed camera input handle's position. */
NIUI_C_API void	GetCameraInputPosition(int handle, int* out_x, int* out_y);

/** Gets the inputed camera input handle's scale. */
NIUI_C_API void GetCameraInputScale(int handle, float* out_x, float* out_y);

/** Assigns a color to the user for the histogram. */
NIUI_C_API void SetUserColor(int userID, unsigned char r, unsigned char g, unsigned char b); 

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// Test functions for aiding the development of the API.
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------

/** Sample Buffer reading. Assumes ARGB 8 bit per channel format. */
NIUI_C_API void ReadBuffer(int bufferPointer, int pixelsToRead);


#endif // #ifndef NIUI_LIBRARY_EXPORT_H