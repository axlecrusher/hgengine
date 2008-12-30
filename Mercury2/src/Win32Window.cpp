#include <Win32Window.h>

LRESULT CALLBACK WindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam); //Window callback
Callback0R< MercuryWindow* > MercuryWindow::genWindowClbk(Win32Window::GenWin32Window); //Register window generation callback

MercuryWindow* Win32Window::GenWin32Window()
{
	return new Win32Window("Mercury2 Tests", 640, 480, 24, 16, false);
}

LPCTSTR StringToLPCTSTR(const MString & s)
{
	size_t length = s.length();
	LPCTSTR str = new WCHAR[length+1];
	for (size_t i=0;i<length; ++i) ((WCHAR*)str)[i]=s[i];
	((WCHAR*)str)[length]=0;
	return str;
}

Win32Window::Win32Window(const MString& title, int width, int height, int bits, int depthBits, bool fullscreen)
	:m_hwnd(NULL), m_hdc(NULL), m_hglrc(NULL), m_hInstance(NULL), m_className(NULL), m_windowAtom(NULL), m_winTitle(NULL),
	MercuryWindow(title, width, height, bits, depthBits, fullscreen)
{
	m_className = (WCHAR*)StringToLPCTSTR("Mercury Render Window");
	m_winTitle = (WCHAR*)StringToLPCTSTR(title);

	GenWinClass();
	GenWindow();
	GenPixelType();
	SetPixelType();
	CreateRenderingContext();
}

Win32Window::~Win32Window()
{
	wglMakeCurrent(NULL, NULL);

	if (m_hglrc)
	{
		wglDeleteContext(m_hglrc);
		m_hglrc = NULL;
	}

	if (m_hwnd)
	{
		::DestroyWindow( m_hwnd );
		m_hwnd = NULL;
	}
	if ( !UnregisterClass((LPCWSTR)m_windowAtom,m_hInstance) )
	{
		m_hInstance = NULL;
	}
}

void Win32Window::GenWinClass()
{
	m_hInstance = GetModuleHandle(NULL);
	m_wndclass.cbSize = sizeof(WNDCLASSEX);
	m_wndclass.hIconSm = NULL;
	m_wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	m_wndclass.lpfnWndProc = (WNDPROC)WindowCallback;
	m_wndclass.cbClsExtra = 0;
	m_wndclass.cbWndExtra = 0;
	m_wndclass.hInstance = m_hInstance;
	m_wndclass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	m_wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	m_wndclass.hbrBackground = NULL;
	m_wndclass.lpszMenuName = NULL;
	m_wndclass.lpszClassName = m_className;
}

void Win32Window::GenWindow()
{
	DWORD dwExStyle; // Window Extended Style
	DWORD dwStyle;	// Window Style
	RECT rect;

	m_windowAtom = RegisterClassEx( &m_wndclass );

	if (m_windowAtom == NULL)
	{
		printf("Error registering window class, code %d", GetLastError() );
		throw(5);
	}

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPEDWINDOW;

	rect.left=(long)0;
	rect.right=(long)m_width;
	rect.top=(long)0;
	rect.bottom=(long)m_height;	

	AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle);

	m_hwnd = CreateWindowEx(
		dwExStyle,
		(LPCWSTR)m_windowAtom,
		m_winTitle,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle,
		0, 0,
		m_width,
		m_height,
		NULL,
		NULL,
		m_hInstance,
		NULL);

	if (m_hwnd == NULL)
	{
		printf("Error creating window, code %d", GetLastError() );
		throw(5);
	}

	ShowWindow(m_hwnd,SW_SHOW);
	SetForegroundWindow(m_hwnd);					// Slightly Higher Priority
	SetFocus(m_hwnd);								// Sets Keyboard Focus To The Window
}

void Win32Window::SetPixelType()
{
	int pixelFormat;
	m_hdc = GetDC(m_hwnd);

	if ( m_hdc == NULL )
	{
		printf("Error getting a device handle, code %d", GetLastError() );
		throw(5);
	}

	pixelFormat = ChoosePixelFormat(m_hdc, &m_pfd);
	if (pixelFormat == 0)
	{
		printf("Error choosing pixel format, code %d", GetLastError() );
		throw(5);
	}

	if( !SetPixelFormat(m_hdc, pixelFormat, &m_pfd) )
	{
		printf("Failed setting pixel format, code %d", GetLastError() );
		throw(5);
	}
}

void Win32Window::CreateRenderingContext()
{
	m_hglrc = wglCreateContext(m_hdc);

	if (m_hglrc == NULL)
	{
		printf("Failed to create rendering context, code %d", GetLastError() );
		throw(5);
	}

	if ( !wglMakeCurrent(m_hdc, m_hglrc) )
	{
		printf("Failed to set current rendering context, code %d", GetLastError() );
		throw(5);
	}
}

void Win32Window::GenPixelType()
{
	memset(&m_pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	m_pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	m_pfd.nVersion = 1;
	m_pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	m_pfd.iPixelType = PFD_TYPE_RGBA;
	m_pfd.cColorBits = m_bits;
	m_pfd.cDepthBits = m_depthBits;
	m_pfd.iLayerType = PFD_MAIN_PLANE;
}

bool Win32Window::SwapBuffers()
{
	return (::SwapBuffers( m_hdc )==TRUE);
}

bool Win32Window::PumpMessages()
{
	MSG message;

	while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
	{
		// Stop Update if the user is trying to quit
		if (message.message == WM_QUIT)
			return false;
		else
		{
			TranslateMessage(&message);				// Translate The Message
			DispatchMessage(&message);				// Dispatch The Message
		}
	}
	return true;
}

LRESULT CALLBACK WindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		{
			exit(1);
		}
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

/* Copyright (c) 2008, Joshua Allen
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *	-	Redistributions of source code must retain the above
 *		copyright notice, this list of conditions and the following disclaimer.
 *	-	Redistributions in binary form must reproduce the above copyright
 *		notice, this list of conditions and the following disclaimer in
 *		the documentation and/or other materials provided with the distribution.
 *	-	Neither the name of the Mercury Engine nor the names of its
 *		contributors may be used to endorse or promote products derived from
 *		this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
