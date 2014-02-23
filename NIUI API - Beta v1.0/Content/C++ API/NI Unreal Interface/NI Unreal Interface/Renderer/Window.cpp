//****************************************************************************
//**
//**    NAME
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 08/2010
//**
//****************************************************************************

#include "Window.h"

#include "RenderDevice.h"

static NIUI::Window* g_window = NULL;

namespace NIUI
{

	Window::Window()
		: m_windowHandle(0)
		, m_hInstance(0)
		, m_renderer(NULL)
	{
	}

	Window::~Window()
	{
	}

	LRESULT CALLBACK Window::StaticWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (g_window != NULL)
			return g_window->WinProc(hWnd, msg, wParam, lParam);

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	LRESULT CALLBACK Window::WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{			
		case WM_DESTROY:
		case WM_CLOSE:
			this->isWindowOpen = false;
			return 0;

		case WM_ACTIVATE:
			switch (wParam)
			{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				isInFocus = true;
				break;
			case WA_INACTIVE:
				isInFocus = false;
				break;
			};
			return 0;
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	bool Window::Create(const WindowSettings &ws, HINSTANCE hInstance)
	{
		if (g_window != NULL)
			return false;

		g_window = this;
		isInFocus = false;
		settings = ws;
		m_hInstance = hInstance;

		ZeroMemory(&m_windowClass, sizeof(WNDCLASS));

		m_windowClass.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		m_windowClass.lpfnWndProc    = (WNDPROC)StaticWinProc;
		m_windowClass.cbClsExtra     = 0;
		m_windowClass.cbWndExtra		= 0;
		m_windowClass.hInstance      = hInstance; // hInstance;
		m_windowClass.hIcon          = NULL;
		m_windowClass.hCursor        = NULL;
		m_windowClass.hbrBackground  = NULL;
		m_windowClass.lpszMenuName   = NULL;
		m_windowClass.lpszClassName  = ws.name;

		RegisterClass(&m_windowClass);

		DWORD style;

		RECT clientRect;
		style = WS_OVERLAPPED;
		clientRect.left = clientRect.top = 0;
		clientRect.right = ws.width - 1;
		clientRect.bottom = ws.height - 1;
		BOOL ok = AdjustWindowRect(&clientRect, style, FALSE);

		m_windowHandle = CreateWindow(ws.name, 
									L"",
									style,
									CW_USEDEFAULT,
									CW_USEDEFAULT,
									(clientRect.right - clientRect.left) + 1,
									(clientRect.bottom - clientRect.top) + 1,
									NULL,
									NULL,
									NULL,
									NULL);

		if (m_windowHandle == NULL)
		{
			return false;
		}

		// Show and update the window.
		ShowWindow(m_windowHandle, SW_SHOWNORMAL);
		UpdateWindow(m_windowHandle);

		SetFocus(m_windowHandle);
		SetCursor(LoadCursor(NULL, IDC_ARROW));

		isWindowOpen = true;
		return true;
	}

	bool Window::Release()
	{
		DestroyWindow(m_windowHandle);
		g_window = NULL;
		return true;
	}

	HWND Window::GetWindowHandle() const
	{
		return m_windowHandle;
	}

	bool Window::AttachRenderer(RenderDevice* renderer)
	{
		printf("Render attached");
		if (renderer == NULL)
			return false;
		
		printf("Render attached succes");
		m_renderer = renderer;
		m_renderer->Initialise(settings.width, settings.height, this->m_windowHandle);
		return true;
	}

	bool Window::ReleaseAttachedDevice()
	{
		m_renderer = NULL;

		return true;
	}

	void Window::ProcessMessages()
	{
		MSG msg;
		
		// Process windows messages.
        while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE)
        {
			if (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				return;
			}
        }
	}

	bool Window::IsOpen() const
	{
		return this->isWindowOpen;
	}

	bool Window::InFocus() const
	{
		return isInFocus;
	}
}; // End namespace NIUI.
