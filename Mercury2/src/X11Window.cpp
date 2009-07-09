#include <X11Window.h>
#include <MercuryMessageManager.h>
#include <MercuryInput.h>
#include <MercuryPrefs.h>

Callback0R< MercuryWindow* > MercuryWindow::genWindowClbk(X11Window::GenX11Window); //Register window generation callback

//XXX: THIS SECTION IS INCOMPLETE!  IT NEEDS The right half of the keyboard (Bar arrow keys) + it needs the windows keys/sel keys mapped
short X11Window::ConvertScancode( int scanin )
{
	switch( scanin )
	{
	case 9: return 27;	//esc
	case 19: return '0';
	case 49: return 97;	//`
	case 22: return 8;      //backspace
	case 95: return 292;    //F11
	case 96: return 293;    //F12
	case 20: return 45; 	//-
	case 21: return 61;	//=
	case 51: return 92;	//backslash
	case 23: return 9;	//tab
	case 50: return 160;	//[lshift]
	case 62: return 161;	//[rshift]

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

	case 34: return 91;	//misc keys inbetween letters and enter
	case 35: return 93;
	case 47: return 59;
	case 48: return 39;
	case 59: return 44;
	case 60: return 46;
	case 61: return 47;

	case 111: return 273;	//arrow keys
	case 113: return 276;
	case 114: return 275;
	case 116: return 274;

	case 37: return 162;	//ctrl, win, alt
	case 133: return 91;
	case 64: return 164;
	
	case 108: return 165;	//RIGHT buttons like ctrl, sel, alt
	case 135: return 93;
	case 105: return 263;

	case 36: return 13;
	case 66: return 15;

	default:
		if( scanin >= 10 && scanin <= 18 )
			return scanin + ( (short)'1' - 10 );
		if( scanin >= 67 && scanin <= 76 )	//f1-f10
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
	glXMakeCurrent(m_display, win, ctx);
	
	XFree(visinfo);

	m_window = win;
	m_renderCtx = ctx;
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);
	
	glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
	static bool inFocus = false;
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
				XFocusChangeEvent*e = (XFocusChangeEvent*)&event;
				inFocus = (event.type == FocusIn);
				if (inFocus) XWarpPointer(m_display, None, m_window, 0,0,0,0,m_width/2,m_height/2);
				break;
			}
		}

		//The events below only get processed if window is in focus
		if ( !inFocus ) continue;
		switch (event.type)
		{
			case ButtonPress:
			case ButtonRelease:
			{
				XButtonEvent* e = (XButtonEvent*)&event;
				MouseInput::ProcessMouseInput(0, 0, e->button & Button1, e->button & Button3, e->button & Button2);
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
				bool left, right, center;
				left = e->state & Button1Mask;
				right = e->state & Button3Mask;
				center = e->state & Button2Mask;
				x = m_width/2 - e->x;
				y = m_height/2 - e->y;
				if (x!=0 || y!=0) //prevent recursive XWarp
				{
					MouseInput::ProcessMouseInput(x, y,
					left, right, center);
					XWarpPointer(m_display, None, m_window, 0,0,0,0,m_width/2,m_height/2);
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
			KeyboardInput::ProcessKeyInput(e->keycode, true, true); //set repeat flag
			return true;
		}
	}
	
	return false;
}

void X11Window::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void* X11Window::GetProcAddress(const MString& x)
{
return NULL;	
}

/***************************************************************************
 *   Copyright (C) 2008 by Joshua Allen   *
 *      *
 *                                                                         *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: *
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. *
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. *
 *     * Neither the name of the Mercury developers nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission. *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR *
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR *
 *   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, *
 *   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,   *
 *   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR    *
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF *
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  *
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS    *
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.          *
 ***************************************************************************/
