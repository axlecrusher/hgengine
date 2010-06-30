#include "MercuryCrash.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#define _CWINDOWS
#else
#if defined( MACOSX ) || defined( __APPLE__ )
#define _CMAC
#else
#define _CLINUX
#endif
#endif



#if defined( _CLINUX ) || defined( _CMAC )

#ifdef _CLINUX

#ifndef __USE_POSIX
#define __USE_POSIX
#include <signal.h>
#undef __USE_POSIX
#else
#include <signal.h>
#endif

#include <bits/siginfo.h>

#else

#include <signal.h>

#endif

int inCrashHandler = 0;

static int msignals[] =
{
	SIGALRM, SIGBUS, SIGFPE, SIGHUP, SIGILL, SIGINT, SIGABRT,
	SIGQUIT, SIGSEGV, SIGTRAP, SIGTERM, SIGVTALRM, SIGXCPU, SIGXFSZ,
#if defined(HAVE_DECL_SIGPWR) && HAVE_DECL_SIGPWR
	SIGPWR,
#endif
#if defined(HAVE_DECL_SIGUSR1) && HAVE_DECL_SIGUSR1
	SIGUSR1,
#endif
	-1
};

static const struct ExceptionLookup {
	int code;
	const char *name;
} exceptions[]={
	{ SIGALRM,		"SIGALRM"	},
	{ SIGBUS,		"SIGBUS"	},
	{ SIGFPE,		"SIGFPE"	},
	{ SIGINT,		"SIGINT"	},
	{ SIGABRT,		"SIGABRT"	},
	{ SIGQUIT,		"SIGQUIT"	},
	{ SIGSEGV,		"Segmentation Fault",	},
	{ SIGTRAP,		"SIGTRAP",	},
	{ SIGTERM,		"SIGTERM",	},
	{ SIGXCPU,		"SIGXCPU",	},
	{ SIGXFSZ,		"SIGXFSZ",	},
	{ 0,			"Unknown Exception",	},
	{ 0 },
};

FNType chHandler;

static void SigHandler( int signal, siginfo_t *si, void *ucp )
{
	if( inCrashHandler )
		exit( -1 );

	inCrashHandler = 1;

	if( chHandler( signal ) == 0x12121212 )
		return;

	inCrashHandler = 0;
	if( signal == SIGINT || signal == SIGQUIT )
	{
		printf( "Ctrl+Break Hit, Exit.\n" );
		exit( 1 );
		return;
	}

	if( signal == SIGTERM || signal == SIGHUP )
		return;

	struct sigaction sa;
	sa.sa_flags = 0;
	sigemptyset( &sa.sa_mask );
	sa.sa_handler = SIG_DFL;

	struct sigaction old;
	sigaction( signal, &sa, &old );
	raise( signal );
	sigaction( signal, &old, NULL );
}


int cnset_execute_on_crash( FNType fn )
{
	int i;
	struct sigaction sa;
	chHandler = fn;

	sa.sa_flags = 0;
	sa.sa_flags |= SA_NODEFER;
	sa.sa_flags |= SA_SIGINFO;
	sigemptyset(&sa.sa_mask);

	// Set up our signal handlers. 
	sa.sa_sigaction = SigHandler;
	for( i = 0; msignals[i] != -1; ++i )
		sigaction( msignals[i], &sa, NULL );

	// Block SIGPIPE, so we get EPIPE.
	sa.sa_handler = SIG_IGN;
	sigaction( SIGPIPE, &sa, NULL );
	return 0;
}

#elif defined( _CWINDOWS )
#include <windows.h>

FNType top;

long __stdcall cnException(
    struct _EXCEPTION_POINTERS * filter)
{
	int ret = 0;
	ret = top( filter->ExceptionRecord->ExceptionCode );
	if( ret == 0x12121212 )
		return EXCEPTION_CONTINUE_EXECUTION;
	else
		return EXCEPTION_EXECUTE_HANDLER;
}

int cnset_execute_on_crash( FNType fn )
{
	top = fn;
	SetUnhandledExceptionFilter(cnException);
	return 0;
}

static const struct ExceptionLookup {
	DWORD	code;
	const char *name;
} exceptions[]={
	{ EXCEPTION_ACCESS_VIOLATION,		"Access Violation"		},
	{ EXCEPTION_BREAKPOINT,			"Breakpoint"			},
	{ EXCEPTION_FLT_DENORMAL_OPERAND,	"FP Denormal Operand"		},
	{ EXCEPTION_FLT_DIVIDE_BY_ZERO,		"FP Divide-by-Zero"		},
	{ EXCEPTION_FLT_INEXACT_RESULT,		"FP Inexact Result"		},
	{ EXCEPTION_FLT_INVALID_OPERATION,	"FP Invalid Operation"		},
	{ EXCEPTION_FLT_OVERFLOW,		"FP Overflow",			},
	{ EXCEPTION_FLT_STACK_CHECK,		"FP Stack Check",		},
	{ EXCEPTION_FLT_UNDERFLOW,		"FP Underflow",			},
	{ EXCEPTION_INT_DIVIDE_BY_ZERO,		"Integer Divide-by-Zero",	},
	{ EXCEPTION_INT_OVERFLOW,		"Integer Overflow",		},
	{ EXCEPTION_PRIV_INSTRUCTION,		"Privileged Instruction",	},
	{ EXCEPTION_ILLEGAL_INSTRUCTION,	"Illegal instruction"		},
	{ EXCEPTION_INVALID_HANDLE,		"Invalid handle"		},
	{ EXCEPTION_STACK_OVERFLOW,		"Stack overflow"		},
	{ 0xe06d7363,				"Unhandled Microsoft C++ Exception",	},
	{ 0, "UNKNOWN EXCEPTION", },
	{ 0 },
};


#endif


const char * cn_get_crash_description( int code )
{
	int i;

	for( i = 0; exceptions[i].code; ++i )
		if( exceptions[i].code == code )
			return exceptions[i].name;

	return exceptions[i].name;
}

void fail_m( const char * message, const char * file, int line )
{
	char backtracebuffer[2048];
	//Probably should message box here somewhere in the event we're running on Windows.
	fprintf( stderr, "Fatal Error: \"%s\" in %s:%d\n", message, file, line );
	cnget_backtrace( 1, backtracebuffer, 2047 );
	puts( backtracebuffer );
	exit(-1);
}

/*
 * (c) 2003-2008 Glenn Maynard, Steve Checkoway, Avery Lee, Charles Lohr
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
