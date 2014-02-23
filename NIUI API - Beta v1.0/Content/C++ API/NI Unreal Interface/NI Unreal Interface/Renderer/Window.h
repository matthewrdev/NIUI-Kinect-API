//****************************************************************************
//**
//**    Window.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 10/2010
//**
//****************************************************************************

#ifndef WINDOW_H
#define WINDOW_H

#include <Windows.h>
#include <string>

// Forward Declarations
namespace NIUI
{
	class RenderDevice;
};

namespace NIUI 
{

	struct WindowSettings
	{
		LPCWSTR name;
		int width, height;

		WindowSettings(){}

		void operator=(const WindowSettings& ws)
		{
			name = ws.name;
			width = ws.width;
			height = ws.height;
		}
	};

	// ------------------------------------------------------------------------
	//								Window
	// ------------------------------------------------------------------------
	// Desc:
	// 
	// ------------------------------------------------------------------------
	class Window
	{
	private:

		HWND m_windowHandle;
		HINSTANCE m_hInstance;
		WNDCLASS m_windowClass;
		RenderDevice* m_renderer;
		WindowSettings settings;

		bool isWindowOpen;
		bool isInFocus;
		
		static LRESULT CALLBACK StaticWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	protected:
	public:

		Window();
		~Window();

		bool Create(const WindowSettings &ws, HINSTANCE hInstance);
		bool Release();
		HWND GetWindowHandle() const;

		bool AttachRenderer(RenderDevice* renderer);
		bool ReleaseAttachedDevice();

		void ProcessMessages();
		bool IsOpen() const;
		bool InFocus() const;
	};
	
}; // End namespace NIUI.

#endif // #ifndef WINDOW_H