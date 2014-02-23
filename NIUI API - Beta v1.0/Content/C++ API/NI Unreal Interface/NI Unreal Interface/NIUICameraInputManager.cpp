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
//**    CameraInputManager.h
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 03/2011
//**
//****************************************************************************

#include "NIUICameraInputManager.h"

#include "Renderer/Window.h"
#include "Renderer/RenderDevice.h"
#include "NIUIDepthBuffer.h"
#include "NIUIUserColorMap.h"
#include "NIUIImageBuffer.h"

#include "Logger.h"

namespace NIUI
{

		CameraInputManager::CameraInputManager()
			: m_window(NULL),
			m_renderer(NULL),
			m_depthMap(NULL),
			m_imageMap(NULL),
			m_userColorMap(NULL)
		{
		}

		CameraInputManager::~CameraInputManager()
		{
		}

		void CameraInputManager::Initialise(OpenNIContextData* context, int maxUsers)
		{
			LOG("Camera Input Manager is being initilised.", LOG_Init);

			// Create and assign everything.
			m_window = new Window();
			m_renderer = new RenderDevice();

			m_userColorMap = new UserColorMap();
			m_depthMap = new DepthBuffer();
			m_imageMap = new ImageBuffer();

			m_depthMap->Initialise(&(context->depthGenerator));
			m_imageMap->Initialise(&(context->imageGenerator));

			// Generate histogram size relative to the camera input resolution
			m_userColorMap->Initialise(maxUsers, &(context->userGenerator), m_imageMap->GetBufferWidth(), m_imageMap->GetBufferHeight());
			
			LOG("Camera Input Manager was succesfully initilised.", LOG_Init);

			DiscardAllCameraFeeds();
		}

		void CameraInputManager::Release()
		{
			LOG("Camera Input Manager is being shutdown.", LOG_Shutdown);

			m_window->ReleaseAttachedDevice();
			m_window->Release();
			// Detroy all subsystems.
			if (m_window != NULL)
			{
				delete m_window;
				m_window = NULL;
			}

			if (m_renderer != NULL)
			{
				delete m_renderer;
				m_renderer = NULL;
			}

			m_imageMap->Release();
			m_depthMap->Release();
			m_userColorMap->Release();

			if (m_imageMap != NULL)
			{
				delete m_imageMap;
				m_imageMap = NULL;
			}

			if (m_depthMap != NULL)
			{
				delete m_depthMap;
				m_depthMap = NULL;
			}

			if (m_userColorMap != NULL)
			{
				delete m_userColorMap;
				m_userColorMap = NULL;
			}
		}

		void CameraInputManager::Draw()
		{
			if (m_window->IsOpen())
			{
				static int count = 0;
				count++;
				if (m_useClear)
				{
					m_renderer->ClearBackBuffer(COLOR_ARGB(0,0,0,0));
				}


				// Render instances.
				for (int iter = 0; iter < NIUI::MAX_CAMERA_FEED_DISPLAYS; iter++)
				{
					RenderCameraInput(iter);
				}

				if (count  >= 60)
					count = 0;

				m_renderer->Present(m_window->GetWindowHandle());
			}
		}
		
		void CameraInputManager::RenderCameraInput(int index)
		{
			if (this->m_cameraFeeds[index].type != CFeed_Invalid)
			{
				// Update the source texture.
				int width, height;
				BYTE* tex = GetAssociatedBuffer((CameraFeedTypeSet)this->m_cameraFeeds[index].type, &width, &height);

				switch (m_cameraFeeds[index].type)
				{
				case CFeed_DepthMap:
				case CFeed_ImageMap:
					// Draw from the display instance position, scale and rotation.
					m_renderer->DrawQuad(tex,
								m_cameraFeeds[index].x,
								m_cameraFeeds[index].y,
								width, 
								height,
								m_cameraFeeds[index].xScale,
								m_cameraFeeds[index].yScale,
								COLOR_ARGB(0,0,0,0),
								false);		
					break;
				case CFeed_UserColorMap:
					m_renderer->DrawQuad(tex,
										m_cameraFeeds[index].x,
										m_cameraFeeds[index].y,
										width, 
										height,
										m_cameraFeeds[index].xScale,
										m_cameraFeeds[index].yScale,
										COLOR_ARGB(0,0,0,0),
										true); // Filter out background.
					break;
				}

			}
		}
		
		BYTE* CameraInputManager::GetAssociatedBuffer(CameraFeedTypeSet type, int* out_width, int* out_height)
		{
			switch (type)
			{
			case CFeed_ImageMap:
				*out_width = m_imageMap->GetBufferWidth();
				*out_height = m_imageMap->GetBufferHeight();
				return m_imageMap->GetData();
				break;
			case CFeed_DepthMap:
				*out_width = m_depthMap->GetBufferWidth();
				*out_height = m_depthMap->GetBufferHeight();
				return m_depthMap->GetData();
				break;
			case CFeed_UserColorMap:
				*out_width = m_userColorMap->GetBufferWidth();
				*out_height = m_userColorMap->GetBufferHeight();
				return m_userColorMap->GetData();
				break;
			}
			return NULL;
		}

		NIUI_STATUS CameraInputManager::FindActiveHandle(int* out_handle)
		{
			// Do a linear search over the DispalyInstance's
			for (int iter = 0; iter < NIUI::MAX_CAMERA_FEED_DISPLAYS; iter++)
			{
				if (this->m_cameraFeeds[iter].active == 0)
				{
					*out_handle = iter;
					return NIUI_STATUS_OK; 
				}
			}

			*out_handle = -1;

			return NIUI_STATUS_FAIL;
		}

		NIUI_STATUS CameraInputManager::OpenPreviewWindow(int xRes, int yRes)
		{
			LOG("Opening preview window.", LOG_Standard);
			WindowSettings ws;
			ws.height = yRes;
			ws.width = xRes;
			ws.name = L"NIUI Preview Window";
			
			if (!m_window->Create(ws, NULL))
			{
				LOG("Could not create preview window.", LOG_Error);
				return NIUI_STATUS_FAIL;
			}
			else
			{
				LOG("Preview window creation was succesfull.", LOG_Standard);
			}

			LOG("Generating preview window render device.", LOG_Standard);

			if (!m_window->AttachRenderer(this->m_renderer))
			{
				LOG("Could not attach renderer to preview window.", LOG_Error);
				return NIUI_STATUS_FAIL;
			}
			else
			{
				LOG("Renderer was succesfully attached to the preview window.", LOG_Standard);
			}


			return NIUI_STATUS_OK;
		}

		NIUI_STATUS CameraInputManager::ClosePreviewWindow()
		{
			// Release window and render device.
			m_renderer->Release(m_window->GetWindowHandle());
			m_window->Release();

			return NIUI_STATUS_OK;
		}

		void CameraInputManager::Update()
		{
			m_isUpdating = true;
			if (m_window->IsOpen())
			{
				// Update input streams.
				m_depthMap->Update();
				m_imageMap->Update();
				m_userColorMap->Update();

				m_window->ProcessMessages();
			}
			m_isUpdating = false;
		}

		void CameraInputManager::SetUserColor(int userID, BYTE r, BYTE g, BYTE b)
		{
			this->m_userColorMap->SetUserColor(userID, r, g, b);
		}

		NIUI_STATUS CameraInputManager::AddCameraInput(int type, int* out_handle)
		{
			if (FindActiveHandle(out_handle) != NIUI_STATUS_FAIL)
			{
				m_cameraFeeds[*out_handle].active = 1;
				m_cameraFeeds[*out_handle].type = type;

				return NIUI_STATUS_OK;
			}
			return NIUI_STATUS_FAIL;
		}

		NIUI_STATUS CameraInputManager::RemoveCameraInput(int handle)
		{
			// Validate handle
			if (handle < 0 || handle >= MAX_CAMERA_FEED_DISPLAYS)
				return NIUI_STATUS_OK;

			m_cameraFeeds[handle].Clear();

			return NIUI_STATUS_OK;
		}

		void CameraInputManager::SetCameraInputPosition(int handle, int x, int y)
		{
			if (handle >= 0 && handle < MAX_CAMERA_FEED_DISPLAYS)
			{
				m_cameraFeeds[handle].x = x;
				m_cameraFeeds[handle].y = y;
			}
		}

		void CameraInputManager::SetCameraInputScale(int handle, float x, float y)
		{
			if (handle >= 0 && handle < MAX_CAMERA_FEED_DISPLAYS)
			{
				m_cameraFeeds[handle].xScale = x;
				m_cameraFeeds[handle].yScale = y;
			}
		}

		void CameraInputManager::GetCameraInputPosition(int handle, int* out_x, int* out_y)
		{
			if (handle >= 0 && handle < MAX_CAMERA_FEED_DISPLAYS)
			{
				*out_x = m_cameraFeeds[handle].x;
				*out_y = m_cameraFeeds[handle].y;
			}
		}

		void CameraInputManager::GetCameraInputScale(int handle, float* out_x, float* out_y)
		{
			if (handle >= 0 && handle < MAX_CAMERA_FEED_DISPLAYS)
			{
				*out_x = m_cameraFeeds[handle].xScale;
				*out_y = m_cameraFeeds[handle].yScale;
			}
		}

		bool CameraInputManager::IsWindowOpen()
		{
			return m_window->IsOpen();
		}

		NIUI_STATUS CameraInputManager::ApplyToUDKTexture(BYTE* buffer, int xRes, int yRes, size_t pixelSize, CameraFeedTypeSet type)
		{
			switch (type)
			{
			case CFeed_ImageMap:
				return NIUI_STATUS_OK;
			case CFeed_DepthMap:
				return NIUI_STATUS_OK;
			case CFeed_UserColorMap:
				return NIUI_STATUS_OK;
			}
			return NIUI_STATUS_FAIL;
		}
		
		void CameraInputManager::EnableBackBufferClearing(bool enable)
		{
			this->m_useClear = enable;
		}
		
		void CameraInputManager::GetCameraViewPortSize(int* out_xRes, int* out_yRes)
		{
			*out_xRes = m_imageMap->GetBufferWidth();
			*out_yRes = m_imageMap->GetBufferHeight();
		}

		
		void CameraInputManager::DiscardAllCameraFeeds()
		{
			for (int iter = 0; iter < MAX_CAMERA_FEED_DISPLAYS; iter++)
			{
				m_cameraFeeds[iter].Clear();
			}
		}

		bool CameraInputManager::IsUpdating() const
		{
			return m_isUpdating;
		}

		
	const char* CameraFeedTypeToString(CameraFeedTypeSet type)
	{
		switch (type)
		{
		case CFeed_ImageMap:
			return "ImageMap";
		case CFeed_DepthMap:
			return "DepthMap";
		case CFeed_UserColorMap:
			return "UserColorMap";
		default:
			break;
		}

		return "Invalid";
	}

}; // End Namespace NIUI
