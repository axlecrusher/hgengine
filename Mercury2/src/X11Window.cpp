#include <X11Window.h>
#include <MercuryMessageManager.h>
#include <MercuryInput.h>
#include <MercuryPrefs.h>

#include <GLHeaders.h>

#define MOUSE_BTN_LEFT 1
#define MOUSE_BTN_RIGHT 3
#define MOUSE_BTN_CENTER 2
#define MOUSE_BTN_SCROLL_UP 4
#define MOUSE_BTN_SCROLL_DOWN 5

MVRefBool GlobalMouseGrabbed_Set( "Input.CursorGrabbed" );

// Use X11_MASK(MOUSE_BTN_SCROLL_UP) to generate the token Button4Mask
#define X11_MASK(x) _X11_MASK(x)
// Sigh... second #define needed, because otherwise x doesn't get evaluated.
// That is, instead of giving you Button4Mask this would give ButtonMOUSE_BTN_SCROLL_UPMask
#define _X11_MASK(x) Button##x##Mask

Callback0R< MercuryWindow* > MercuryWindow::genWindowClbk(X11Window::GenX11Window); //Register window generation callback

//XXX: THIS SECTION IS INCOMPLETE!  IT NEEDS The right half of the keyboard (Bar arrow keys) + it needs the windows keys/sel keys mapped
short X11Window::ConvertScancode( int scanin )
{
	switch( scanin )
	{
	case 9: return 27;      //esc
	case 19: return '0';
	case 49: return 97;     //`
	case 22: return 8;      //backspace
	case 95: return 292;    //F11
	case 96: return 293;    //F12
	case 20: return 45;     //-
	case 21: return 61;     //=
	case 51: return 92;     //backslash
	case 23: return 9;      //tab
	case 66: return 15;     //Caps lock
	case 50: return 160;    //[lshift]
	case 62: return 161;    //[rshift]

	case 38: return 'a';
	case 56: return 'b';
	case 54: return 'c';
	case 40: return 'd';
	case 26: return 'e';
	case 41: return 'f';
	case 42: return 'g';
	case 43: return 'h';
	case 31: return 'i';
	case 44: return 'j';
	case 45: return 'k';	
	case 46: return 'l';	
	case 58: return 'm';	
	case 57: return 'n';	
	case 32: return 'o';	
	case 33: return 'p';	
	case 24: return 'q';	
	case 27: return 'r';	
	case 39: return 's';	
	case 28: return 't';	
	case 30: return 'u';	
	case 55: return 'v';	
	case 25: return 'w';	
	case 53: return 'x';	
	case 29: return 'y';	
	case 52: return 'z';	

	case 47: return 59;	//;
	case 48: return 39;	//'
	case 59: return 44;	//,
	case 60: return 46;	//.
	case 61: return 47;	// /

	case 111: return 273;	//arrow keys: up
	case 113: return 276;	//arrow keys: left
	case 114: return 275;	//arrow keys: right
	case 116: return 274;	//arrow keys: down

	case 37: return 162;	//left ctrl
	case 133: return 91;	//left super (aka win)
	case 64: return 164;	//left alt
	case 65: return 32;	//space bar
	case 108: return 165;	//right alt
	case 134: return 91;	//right super (aka win)
	case 135: return 93;	//menu
	case 105: return 268;	//right control
	
	case 107: return 316;	//Print Screen
	//case 78: scroll lock
	case 127: return 19;	//Pause
	case 118: return 277;	//Insert
	case 110: return 278;	//Home
	case 112: return 280;	//Page Up
	case 119: return 127;	//Delete
	case 115: return 279;	//End
	case 117: return 181;	//Page Down
	
	//case 77: Num Lock (not mapped)
	case 106: return 267;	//Keypad /
	case 63: return 268;	//Keypad *
	case 82: return 269;	//Keypad -
	case 79: return 263;	//Keypad 7
	case 80: return 264;	//Keypad 8
	case 81: return 265;	//Keypad 9
	case 86: return 270;	//Keypad +
	case 83: return 260;	//Keypad 4
	case 84: return 261;	//Keypad 5
	case 85: return 262;	//Keypad 6
	case 87: return 257;	//Keypad 1
	case 88: return 258;	//Keypad 2
	case 89: return 259;	//Keypad 3
	case 36:		//Enter
	case 104: return 13;	//Keypad enter
	case 90: return 260;	//Keypad 0
	case 91: return 266;	//Keypad .
	
	default:
		// numbers
		if( scanin >= 10 && scanin <= 18 )
			return scanin + ( (short)'1' - 10 );
		// f1 -- f10
		if( scanin >= 67 && scanin <= 76 )
			return scanin + ( 282 - 67 );
		return scanin;
	}
}

X11Window::X11Window(const MString& title, int width, int height, int bits, int depthBits, bool fullscreen)
	:MercuryWindow(title, width, height, bits, depthBits, fullscreen), m_display(NULL)
{
	m_display = XOpenDisplay(NULL);
	if (m_display == NULL)
	{
		fprintf(stderr, "Cannot connect to X server %s\n", ":0");
		exit (-1);
	}
	
	int screen_num = DefaultScreen(m_display);
	Window root_window = RootWindow(m_display, screen_num);
	
	int attrib[] = { GLX_RGBA,
				GLX_RED_SIZE, 1,
				GLX_GREEN_SIZE, 1,
				GLX_BLUE_SIZE, 1,
				GLX_DOUBLEBUFFER,
				GLX_DEPTH_SIZE, 1,
				None };

	XVisualInfo* visinfo = glXChooseVisual( m_display, screen_num, attrib );
	if (!visinfo) {
		printf("Error: couldn't get an RGB, Double-buffered visual\n");
		exit(1);
	}

	XSetWindowAttributes attr;
	/* window attributes */
	attr.background_pixel = 0;
	attr.border_pixel = 0;
	attr.colormap = XCreateColormap( m_display, root_window, visinfo->visual, AllocNone);
	attr.event_mask = StructureNotifyMask | SubstructureNotifyMask | ExposureMask | ButtonPressMask | ButtonReleaseMask | ButtonPressMask | PointerMotionMask | ButtonMotionMask | EnterWindowMask | LeaveWindowMask |KeyPressMask |KeyReleaseMask | SubstructureNotifyMask | FocusChangeMask;
	
	unsigned long mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

	Window win = XCreateWindow( m_display, root_window, 0, 0, width, height,
						 0, visinfo->depth, InputOutput,
	   visinfo->visual, mask, &attr );
	
	m_wmDeleteMessage = XInternAtom(m_display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(m_display, win, &m_wmDeleteMessage, 1);
	
	/* set hints and properties */
	{
		XSizeHints sizehints;
		sizehints.x = 0;
		sizehints.y = 0;
		sizehints.width  = width;
		sizehints.height = height;
		sizehints.flags = USSize | USPosition;
		XSetNormalHints(m_display, win, &sizehints);
		XSetStandardProperties(m_display, win, title.c_str(), title.c_str(),
							   None, (char **)NULL, 0, &sizehints);
	}

	GLXContext ctx = glXCreateContext( m_display, visinfo, NULL, True );
	if (!ctx) {
		printf("Error: glXCreateContext failed\n");
		exit(1);
	}

	XMapWindow(m_display, win);
	GLCALL( glXMakeCurrent(m_display, win, ctx) );
	
	XFree(visinfo);

	m_window = win;
	m_renderCtx = ctx;
	
	GLCALL( glEnable(GL_DEPTH_TEST) );
	GLCALL( glEnable(GL_CULL_FACE) );
	GLCALL( glEnable(GL_NORMALIZE) );
	
	GLCALL( glEnable (GL_BLEND); );
	GLCALL( glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );
}

X11Window::~X11Window()
{
	if (m_renderCtx) glXDestroyContext(m_display, m_renderCtx);
	if (m_display) XCloseDisplay(m_display);
	m_renderCtx = NULL;
	m_display = NULL;
}

MercuryWindow* X11Window::GenX11Window()
{
	return new X11Window(
		PREFSMAN.GetValueS("Screen.Name", "Screen name not set." ),
		PREFSMAN.GetValueI( "Screen.Width", 640 ),
		PREFSMAN.GetValueI( "Screen.Height", 480 ),
		24,
		PREFSMAN.GetValueI( "Screen.Depth", 16 ),
		PREFSMAN.GetValueB( "Screen.FullScreen", 1 )
		);
}

bool X11Window::SwapBuffers()
{
	glXSwapBuffers(m_display, m_window);
	return true;
}

bool X11Window::PumpMessages()
{
//	static bool inFocus = false;
	XEvent event;
	while ( XPending(m_display) > 0)
	{
		XNextEvent(m_display, &event);

		switch (event.type)
		{
			case ClientMessage:
			{
				if ( unsigned(event.xclient.data.l[0]) == m_wmDeleteMessage )
				{
					XDestroyWindow(m_display,m_window);
					return false;
				}
				break;
			}
			case DestroyNotify:
			{
				XDestroyWindowEvent* e = (XDestroyWindowEvent*)&event;
				if (e->window == m_window) return false;
				break;
			}
			case ConfigureNotify:
			{
				XConfigureEvent* e = (XConfigureEvent*)&event;
				m_width = e->width;
				m_height = e->height;
				break;
			}
			case FocusIn:
			case FocusOut:
			{
				//XFocusChangeEvent*e = (XFocusChangeEvent*)&event;
				m_inFocus = (event.type == FocusIn);
				if (m_inFocus && GlobalMouseGrabbed_Set.Get() ) XWarpPointer(m_display, None, m_window, 0,0,0,0,m_width/2,m_height/2);
				break;
			}
		}

		//The events below only get processed if window is in focus
		if ( !m_inFocus ) continue;
		switch (event.type)
		{
			case ButtonPress:
			case ButtonRelease:
			{
				XButtonEvent* e = (XButtonEvent*)&event;
				uint8_t left, right, center, su, sd;
				left   = ((e->state & X11_MASK(MOUSE_BTN_LEFT))!=0)        ^ (e->button == MOUSE_BTN_LEFT);
				right  = ((e->state & X11_MASK(MOUSE_BTN_RIGHT))!=0)       ^ (e->button == MOUSE_BTN_RIGHT);
				center = ((e->state & X11_MASK(MOUSE_BTN_CENTER))!=0)      ^ (e->button == MOUSE_BTN_CENTER);
				su     = ((e->state & X11_MASK(MOUSE_BTN_SCROLL_UP))!=0)   ^ (e->button == MOUSE_BTN_SCROLL_UP);
				sd     = ((e->state & X11_MASK(MOUSE_BTN_SCROLL_DOWN))!=0) ^ (e->button == MOUSE_BTN_SCROLL_DOWN);

				MouseInput::ProcessMouseInput(m_iLastMouseX, m_iLastMouseY , 
					left, right, center, su, sd, false);
				break;
			}
			case KeyPress:
			{
				//ignore autorepeat
				if ( IsKeyRepeat(&event.xkey) ) break;
				
				KeyboardInput::ProcessKeyInput( ConvertScancode( event.xkey.keycode ), true, false);
				break;
			}
			case KeyRelease:
			{
				//ignore autorepeat
				if ( IsKeyRepeat(&event.xkey) ) break;
				
				KeyboardInput::ProcessKeyInput( ConvertScancode( event.xkey.keycode ), false, false);
				break;
			}
			case MotionNotify:
			{
				
				XMotionEvent* e = (XMotionEvent*)&event;
				int x, y;
				bool left, right, center, su, sd;
				left = e->state & X11_MASK(MOUSE_BTN_LEFT);
				right = e->state & X11_MASK(MOUSE_BTN_RIGHT);
				center = e->state & X11_MASK(MOUSE_BTN_CENTER);
				su = e->state & X11_MASK(MOUSE_BTN_SCROLL_UP);
				sd = e->state & X11_MASK(MOUSE_BTN_SCROLL_DOWN);
				if( GlobalMouseGrabbed_Set.Get() )
				{
					x = m_width/2 - e->x;
					y = m_height/2 - e->y;
					if (x!=0 || y!=0) //prevent recursive XWarp
					{
						m_iLastMouseX = x;
						m_iLastMouseY = y;
						MouseInput::ProcessMouseInput(x, y, left, right, center, su, sd, true);
						XWarpPointer(m_display, None, m_window, 0,0,0,0,m_width/2,m_height/2);
					}
				}
				else
				{
					m_iLastMouseX = e->x;
					m_iLastMouseY = e->y;
					MouseInput::ProcessMouseInput(e->x, e->y, left, right, center, su, sd, true);
				}
				break;
			}
			default:
				break;
		}
	}
	return true;
}

bool X11Window::IsKeyRepeat(XKeyEvent* e)
{
	XEvent nEvent;
	
	if ( XPending(m_display) > 0 )
	{
		XPeekEvent(m_display, &nEvent);
		if ( (nEvent.type == KeyRelease || nEvent.type == KeyPress) &&
			nEvent.xkey.keycode == e->keycode && 
			nEvent.xkey.time == e->time)
		{
			XNextEvent(m_display, &nEvent); //forget next event
			KeyboardInput::ProcessKeyInput( ConvertScancode( e->keycode ), true, true ); //set repeat flag
			return true;
		}
	}
	
	return false;
}

void X11Window::Clear()
{
	GLCALL( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
}

void* X11Window::GetProcAddress(const MString& x)
{
return NULL;	
}


/****************************************************************************
 *   Copyright (C) 2008 by Joshua Allen                                     *
 *                                                                          *
 *                                                                          *
 *   All rights reserved.                                                   *
 *                                                                          *
 *   Redistribution and use in source and binary forms, with or without     *
 *   modification, are permitted provided that the following conditions     *
 *   are met:                                                               *
 *     * Redistributions of source code must retain the above copyright     *
 *      notice, this list of conditions and the following disclaimer.       *
 *     * Redistributions in binary form must reproduce the above            *
 *      copyright notice, this list of conditions and the following         *
 *      disclaimer in the documentation and/or other materials provided     *
 *      with the distribution.                                              *
 *     * Neither the name of the Mercury Engine nor the names of its        *
 *      contributors may be used to endorse or promote products derived     *
 *      from this software without specific prior written permission.       *
 *                                                                          *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS    *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT      *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR  *
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT   *
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  *
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT       *
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  *
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  *
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT    *
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 ***************************************************************************/

