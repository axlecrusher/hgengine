#include "MercuryBacktrace.h"


#ifdef WIN32
#define _CWINDOWS
#else
#if defined( MACOSX ) || defined( __APPLE__ )
#define _CMAC
#else
#define _CLINUX
#endif
#endif



#if defined ( _CWINDOWS )
#define CPU_X86
#include <windows.h>
#include <dbghelp.h>
#pragma comment( lib, "dbghelp.lib" )
/*Since we have inline code in windows, there are
  warnings associated with disabling optimization,
  ignore them. */
#pragma warning( disable : 4748)
/*Ignore CRT_SECURE_NO_DEPRECIATE for snprintf */
#pragma warning( disable : 4996)
#elif defined( _CMAC ) || defined( _CLINUX )

#ifdef _CLINUX
#include <malloc.h>
#include <execinfo.h>
#define HAVE_EXECINFO
#endif


#ifdef HAVE_LIBIBERTY

/*Ok, even StepMania had this question!:
 This is in libiberty. Where is it declared? */
#ifdef __cplusplus
extern "C" {
#endif
char *cplus_demangle (const char *mangled, int options);
#ifdef __cplusplus
};
#endif
#endif

#if !defined( __USE_GNU ) && !defined( _CMAC )

typedef struct
{
  __const char *dli_fname;	/* File name of defining object.  */
  void *dli_fbase;		/* Load address of that object.  */
  __const char *dli_sname;	/* Name of nearest symbol.  */
  void *dli_saddr;		/* Exact value of nearest symbol.  */
} Dl_info;

extern int dladdr (__const void *__address, Dl_info *__info);
extern void *dlopen (__const char *__file, int __mode);
extern int dlclose (void *__handle);
extern void *dlsym (void *__restrict __handle,
		    __const char *__restrict __name);

#endif

#include <dlfcn.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
int bUseAddr2Line, DBGSetup = 0;

#endif




struct BacktraceContext
{
	int dummy;
#if defined( _CWINDOWS)
	CONTEXT context;
	HANDLE hThread;
	HANDLE hProcess;
#endif
};

#include <stdio.h>

int GetBacktrace( void **buf, unsigned size, struct BacktraceContext *ctx );
int GetBTName( const void * ptr, char * str, int maxlen, struct BacktraceContext * ctx );
void SetupDBGHelp();


int cnget_backtrace( int SkipFirst, char * buffer, int max_size )
{
	char * tmpbuf = buffer;

	void * buf[256];
	int i = 0;
	SetupDBGHelp();
	GetBacktrace( (void **)buf, 256,  0 );

	for( ; i < SkipFirst; i++ )
		if( buf[i] == 0 )
			return 0;

	while( buf[i] != 0 )
	{
		int space_left = max_size-(int)(tmpbuf-buffer)-2;
		tmpbuf+=GetBTName( buf[i], tmpbuf, space_left, 0 );
		*tmpbuf='\n';
		tmpbuf++;
		*tmpbuf='\0';
		i++;
	}

	return i-1;
}







#if defined( _CMAC ) || defined( _CLINUX )
//Based off of http://stackoverflow.com/questions/289820/getting-the-current-stack-trace-on-mac-os-x

int GetBacktrace( void **buffer, unsigned size, struct BacktraceContext *ctx )
{
#ifdef HAVE_EXECINFO
	return  backtrace( buffer, size );
#else
	//Todo: See if the return is beyond the end of a function.

        void **frame = (void **)__builtin_frame_address(0);
        void **bp = ( void **)(*frame);
        void *ip = frame[1];
        int i;

        for ( i = 0; bp && ip && i < size; i++ )
        {
                *(buffer++) = ip;
                ip = bp[1];
                bp = (void**)(bp[0]);
        }

        return i;
#endif
}

int Demangle( char * out, int maxlen, const char * in )
{
	*out = 0;
	if( !in )
		return 0;
#ifdef HAVE_LIBIBERTY
	int pos = 0;
	char * demangled = cplus_demangle( in, 0 );
	if( !demangled )
		return 0;
	while( demangled[pos] != 0 && pos + 1 < maxlen )
	{
		out[pos] = demangled[pos];
		pos++;
	}
	out[pos] = 0;
	free( demangled );
	return pos;
#else
	return 0;
#endif
}


int Addr2Line( char * out, char * demangled, int maxlen, const char * file, const void * offset )
{
	int tmp;
	char execline[1024];
	char buffer[1024];
	int fds[2];
	int readbytes;
	int newlinepos;
	int slashpos;

	*out = 0;
	*demangled = 0;

	if( strlen( file ) < 1 )
		return 0;

	tmp = open( file, O_RDONLY );
	if( !tmp )
		return 0;
	close( tmp );

	if( pipe( fds ) != 0 )
		return 0;

#ifdef _CMAC
	sprintf( execline, "atos -o %s 0x%lx 1>&%d", file, (unsigned long)offset, fds[1] );
	printf( "Crash Progress: Executing command: %s\n", execline );
#else
	sprintf( execline, "addr2line -fC -e %s 0x%lx 1>&%d", file, (unsigned long)offset, fds[1] );
#endif
	system( execline );

	readbytes = read( fds[0], buffer, 1024 );
	if( readbytes + 1 >= maxlen )
		readbytes = maxlen - 1;
	close( fds[0] );
	close( fds[1] );

	//Something went wrong.
	if( readbytes < 3 )
		return 0;
#ifdef _CMAC
	memcpy( out, buffer, readbytes - 1 );
	out[readbytes] = 0;
	return 1;
#else
	//??:0 is a bad return value.
	if( buffer[0] == '?' )
		return 0;

	if( strstr( buffer, ": No such file" ) > 0 )
		return 0;

	newlinepos = 0;
	for( tmp = 0; tmp < readbytes; tmp++ )
		if( buffer[tmp] == '\n' ) break;
	newlinepos = tmp;

	if( tmp == readbytes )
		return 0;

	memcpy( demangled, buffer, newlinepos );
	demangled[newlinepos] = 0;

	slashpos = 0;
	for( tmp = newlinepos; tmp < readbytes; tmp++ )
	{
		if( buffer[tmp] == '/' ) slashpos = tmp;
	}

	if( slashpos == 0 )
		slashpos = newlinepos;
	slashpos++;
	if( slashpos >= readbytes )
		slashpos = 0;

	readbytes -= slashpos;
	memcpy( out, buffer + slashpos, readbytes );

	if( readbytes > 1 )
		out[readbytes-1] = 0;
	else
		out[readbytes] = 0;

	return readbytes;
#endif

}


int GetBTName( const void * ptr, char * str, int maxlen, struct BacktraceContext * ctx )
{
	char demangled[1024], floc[1024];
	const char * symbol;
	Dl_info dli;
	int c = 0;
	int useaddr=0;

	c = snprintf(str, maxlen, " [%0*lx]", (int)sizeof(void*)*2, (long unsigned int)ptr );	

	if (dladdr(ptr, &dli))
	{
		unsigned offset;
		symbol = dli.dli_sname;

		if( dli.dli_fbase && dli.dli_fname )
		{
			if( bUseAddr2Line )
			{
				unsigned long actualptr = (unsigned long)ptr;
				int fnamelen = strlen( dli.dli_fname );

				//If it's a .so, we need ot check relatively to the start of the .so
				//since it was compiled with -fPIC

				if(     dli.dli_fname[fnamelen-3] == '.' && 
					dli.dli_fname[fnamelen-2] == 's' && 
					dli.dli_fname[fnamelen-1] == 'o' )
				{
					actualptr-=(unsigned long)dli.dli_fbase;
				}

				if( Addr2Line( floc, demangled, 1024, dli.dli_fname, (void*)actualptr ) )
				{
					symbol = demangled;
					useaddr=1;
				}
			}
		}

		//If addr2line doesn't work right, we can try using demangle from libiberty
		if( !useaddr )
			if( Demangle( demangled, 1024, symbol ) )
				symbol = demangled;

		if( symbol )
		{
			offset = ptr - dli.dli_saddr;
			c += snprintf(str+c, maxlen-c, " (%s+0x%x (0x%lx))", symbol, offset,(unsigned long)dli.dli_saddr );
		}
		if( dli.dli_fbase && dli.dli_fname )
		{
			if( useaddr )
				c += snprintf(str+c, maxlen-c, " [%s]", floc );
			else
				c += snprintf(str+c, maxlen-c, " [%s+0x%lx]", dli.dli_fname, (unsigned long)dli.dli_fbase );
		}
	}
	return c;
}

void SetupDBGHelp()
{
	int fds[2];
	char execline[1024];
	if( DBGSetup )
		return;
	DBGSetup = 1;
	pipe( fds );

#ifdef _CMAC
	sprintf( execline, "atos 2>&%d", fds[0] );
#else
	sprintf( execline, "addr2line -v 1>&%d", fds[0] );
#endif

	if( system( execline ) == 0 )
		bUseAddr2Line = 1;
	else
		bUseAddr2Line = 0;
	close( fds[0] );
	close( fds[1] );
}


#elif defined( _CWINDOWS )

int wGetCurrentContext( struct BacktraceContext *CTX )
{
	CTX->context.ContextFlags = CONTEXT_CONTROL;
	CTX->hThread = GetCurrentThread();
	CTX->hProcess = GetCurrentProcess();
#ifdef CPU_X86
	//no way to do it except in ASM, see below.
#else
	if( !RtlCaptureContext( &CTX->context ) )
	{
		printf( "Could not launch context helper.\n" );
		return -1;
	}
#endif
	return 0;
}

int GetBacktrace( void **buf, size_t size, struct BacktraceContext *CTX )
{
	const void **pLast = buf + size - 1;
	int bFirst = 1;
	DWORD MachineType;
	CONTEXT * pContext;
	HANDLE hThread;
	HANDLE hProcess;
	struct BacktraceContext MCTX; //in case we don't have a context.
	int ActuallyThisThread = 0;
	if( CTX == 0 )
	{
		if( wGetCurrentContext( &MCTX ) != 0)
			return -2;
		pContext = &(MCTX.context);
		hThread = MCTX.hThread;
		hProcess = MCTX.hProcess;
		ActuallyThisThread = 1;
	}
	else
	{
		pContext = &(CTX->context);
		hThread = CTX->hThread;
		hProcess = CTX->hProcess;
	}

	{
		size_t count = 0;
		STACKFRAME64 sf64;

#ifdef CPU_X86
		CONTEXT Context;
		MachineType = IMAGE_FILE_MACHINE_I386;

		if( ActuallyThisThread )
		{
			ZeroMemory( &Context, sizeof( CONTEXT ) );

			//http://jpassing.wordpress.com/2008/03/12/walking-the-stack-of-the-current-thread/
			__asm
			{
			Label:
			  mov [Context.Ebp], ebp;
			  mov [Context.Esp], esp;
			  mov eax, [Label];
			  mov [Context.Eip], eax;
			}
			Context.ContextFlags = CONTEXT_CONTROL;
			pContext = &Context;
		}

		memset( &sf64, '\0', sizeof sf64 );
		sf64.AddrPC.Offset = Context.Eip;
		sf64.AddrPC.Mode = AddrModeFlat;
		sf64.AddrFrame.Offset = Context.Ebp;
		sf64.AddrFrame.Mode = AddrModeFlat;
		sf64.AddrStack.Offset = Context.Esp;
		sf64.AddrStack.Segment = AddrModeFlat;
#else
		MachineType = IMAGE_FILE_MACHINE_AMD64;
		memset( &sf64, '\0', sizeof sf64 );
		sf64.AddrPC.Offset = Context.Rip;
		sf64.AddrPC.Mode = AddrModeFlat;
		sf64.AddrFrame.Offset = Context.Rsp;
		sf64.AddrFrame.Mode = AddrModeFlat;
		sf64.AddrStack.Offset = Context.Rsp;
		sf64.AddrStack.Segment = AddrModeFlat;
#endif
		while( 1 )
		{
			int ret;
			SetLastError( 0 );
			ret = StackWalk64( MachineType, hProcess, hThread, &sf64, pContext, 0,
				SymFunctionTableAccess64, SymGetModuleBase64, 0 );

			if( !ret )
			{
				break;
			}

			if( sf64.AddrPC.Offset != 0)
				*buf = (void*)sf64.AddrPC.Offset;
			else
				*buf = (void*)-1;
			buf++;
			count++;
			if( count + 1 >= size ) break;
		}
	}
	SetLastError( 0 );
	*buf = 0;

	return 0;
}

int GetBTName( const void * ptr, char * str, int maxlen, struct BacktraceContext * ctx )
{
	char lsymbol[1024], lfile[1024];
	IMAGEHLP_LINE64 Line;
	PSYMBOL_INFO Symbol;
	HANDLE hProcess;
	int i, pwd = 0;

	Symbol = malloc( sizeof( SYMBOL_INFO ) + 501 );
	Line.SizeOfStruct = sizeof( IMAGEHLP_LINE64 );
	
	Symbol->MaxNameLen = 500;

	if( ctx )
		hProcess = ctx->hProcess;
	else
		hProcess = GetCurrentProcess();

	i = SymFromAddr( hProcess, (DWORD64)ptr, 0, Symbol );
	if( !i )
	{
		return  _snprintf( str, maxlen, "%p (unknown)", ptr );
	}
	i = SymGetLineFromAddr64( hProcess, (DWORD64)ptr, &pwd, &Line );
	
	if( i == 0)
	{
		Line.FileName = 0;
		Line.LineNumber = 0;
	}

	for( i = 0; i < 1023; i++ )
	{
		TCHAR c = Symbol->Name[i];
		lsymbol[i] = (char)c;
		if( c < 28 ) break;
	}
	lsymbol[i] = 0;

	if(  Line.FileName )
	{
		for( i = 0; i < 1023; i++ )
		{
			TCHAR c = Line.FileName[i];
			lfile[i] = (char)c;
			if( c < 28 ) break;
		}
		lfile[i] = 0;
	}

	if( Line.FileName )
		return _snprintf( str, maxlen, "%p %s (%s:%d)", ptr, lsymbol, lfile, Line.LineNumber );
	else
		return _snprintf( str, maxlen, "%p %s", ptr, lsymbol );
}

//Base off of example from http://www.debuginfo.com/example

void SetupDBGHelp()
{
	int bRet = 0;
	static int already_set_up = 0;
	DWORD Options;

	if (already_set_up) return;
	already_set_up = 1;

	Options = SymGetOptions(); 
	Options |= SYMOPT_DEBUG; 
	Options |= SYMOPT_LOAD_LINES;
	SymSetOptions( Options ); 

	bRet = SymInitialize ( 
	            GetCurrentProcess(),  // Process handle of the current process 
	            NULL,                 // No user-defined search path -> use default 
	            TRUE                  // Load symbols for all modules in the current process 
	          ); 

	if( !bRet ) 
	{
		printf("Error: SymInitialize() failed. Error code: %u \n", GetLastError());
		return; 
	}

	{
		TCHAR pFileName[2048];

		DWORD64 ModBase;
		DWORD64   BaseAddr  = 0x10000000; 
		DWORD     FileSize  = 0; 

		GetModuleFileName( 0, pFileName, 2048 );
	
		if( !GetFileSize( pFileName, &FileSize ) ) 
			return; 

		ModBase = SymLoadModule64 ( 
								GetCurrentProcess(),
								NULL,
								(PSTR)pFileName,
								NULL,
								BaseAddr,
								FileSize
							); 

		if( ModBase == 0 ) 
		{
			printf( "Error: SymLoadModule64() failed. Error code: %u \n" , GetLastError());
			return; 
		}
	}


	return; 
}

#else
#error Neither _CWINDOWS, _CMAC OR _CLINUX are defined!
#endif


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
