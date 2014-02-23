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

#ifndef NIUI_CAMERA_INPUT_MANAGER_H
#define NIUI_CAMERA_INPUT_MANAGER_H

//****************************************************************************
//**
//**    NIUICameraInputManager.h
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 03/2011
//**
//****************************************************************************

#include "NIUI.h"

#include <Windows.h>

// Forward Declarations
namespace NIUI
{
	class Window;
	class RenderDevice;
	class DepthBuffer;
	class ImageBuffer;
	class UserColorMap;

	struct OpenNIContextData;
}; // End Namespace NIUI

namespace NIUI
{
	const int MAX_CAMERA_FEED_DISPLAYS = 10;

	enum CameraFeedTypeSet
	{
		CFeed_Invalid, // As zero so that memset(0) disable the Instance.

		CFeed_ImageMap,
		CFeed_DepthMap,
		CFeed_UserColorMap,
	};

	struct InternalCameraFeed
	{
		int type;
		int x,y;
		float xScale, yScale;
		int active;

		void Clear()
		{
			type = (int)CFeed_Invalid;
			x = 0;
			y = 0;
			xScale = 1.0f;
			yScale = 1.0f;
			active = 0;
		}
	};

	// ------------------------------------------------------------------------
	//								CameraInputManager
	// ------------------------------------------------------------------------

	class CameraInputManager
	{
	private:
		Window* m_window;
		RenderDevice* m_renderer;
		
		DepthBuffer* m_depthMap;
		ImageBuffer* m_imageMap;
		UserColorMap* m_userColorMap;

		bool m_useClear;

		InternalCameraFeed m_cameraFeeds[MAX_CAMERA_FEED_DISPLAYS];

		BYTE* GetAssociatedBuffer(CameraFeedTypeSet type, int* out_width, int* out_height);

		NIUI_STATUS FindActiveHandle(int* out_handle);

		bool m_isUpdating;

		void RenderCameraInput(int index);
	protected:
	public:

		CameraInputManager();
		~CameraInputManager();

		void Initialise(OpenNIContextData* context, int maxUsers);
		void Release();

		NIUI_STATUS OpenPreviewWindow(int xRes, int yRes);
		NIUI_STATUS ClosePreviewWindow();

		void Update();
		void Draw();

		void SetUserColor(int userID, BYTE r, BYTE g, BYTE b);

		NIUI_STATUS AddCameraInput(int type, int* out_handle);
		NIUI_STATUS RemoveCameraInput(int handle);

		void SetCameraInputPosition(int handle, int x, int y);
		void SetCameraInputScale(int handle, float x, float y);

		void GetCameraInputPosition(int handle, int* out_x, int* out_y);
		void GetCameraInputScale(int handle, float* out_x, float* out_y);

		bool IsWindowOpen();

		NIUI_STATUS ApplyToUDKTexture(BYTE* buffer, int xRes, int yRes, size_t pixelSize, CameraFeedTypeSet type);

		void EnableBackBufferClearing(bool enable);

		void GetCameraViewPortSize(int* out_xRes, int* out_yRes);

		void DiscardAllCameraFeeds();

		bool IsUpdating() const;
	};

	const char* CameraFeedTypeToString(CameraFeedTypeSet type);

}; // End Namespace NIUI


#endif // #ifndef NIUI_CAMERA_INPUT_MANAGER_H