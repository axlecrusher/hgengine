#include <X11Window.h>

Callback0R< MercuryWindow* > MercuryWindow::genWindowClbk(X11Window::GenX11Window); //Register window generation callback

X11Window::X11Window(const string& title, int width, int height, int bits, int depthBits, bool fullscreen)
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
	attr.event_mask = StructureNotifyMask | SubstructureNotifyMask | ExposureMask | ButtonPressMask | ButtonReleaseMask | ButtonPressMask | PointerMotionMask | ButtonMotionMask | EnterWindowMask | LeaveWindowMask |KeyPressMask |KeyReleaseMask | SubstructureNotifyMask;
	
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
}

X11Window::~X11Window()
{
	if (m_display) XCloseDisplay(m_display);
	m_display = NULL;
}

MercuryWindow* X11Window::GenX11Window()
{
	return new X11Window("Mercury2 Tests", 640, 480, 24, 16, false);
}

bool X11Window::SwapBuffers()
{
	glXSwapBuffers(m_display, m_window);
	return true;
}

bool X11Window::PumpMessages()
{
	XEvent event;
	while ( XPending(m_display) > 0)
	{
		XNextEvent(m_display, &event);
		switch (event.type)
		{
			case ClientMessage:
			{
				if ( event.xclient.data.l[0] == m_wmDeleteMessage )
					XDestroyWindow(m_display,m_window);
				break;
			}
			case DestroyNotify:
			{
				XDestroyWindowEvent* e = (XDestroyWindowEvent*)&event;
				if (e->window == m_window) return false;
				break;
			}
			case ButtonPress:
			{
				XButtonEvent* e = (XButtonEvent*)&event;
				break;
			}
			case ButtonRelease:
			{
				XButtonEvent* e = (XButtonEvent*)&event;
				break;
			}
			case KeyPress:
			{
				XKeyEvent* e = (XKeyEvent*)&event;
				break;
			}
			case KeyRelease:
			{
				XKeyEvent* e = (XKeyEvent*)&event;
				break;
			}
			case MotionNotify:
			{
				XMotionEvent* e = (XMotionEvent*)&event;
				break;
			}

			default:
				break;
		}
	}
	return true;
}

void* X11Window::GetProcAddress(const string& x)
{
	
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
