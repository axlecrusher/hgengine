#include <MercuryUtil.h>
#include <MercuryFile.h>
#include <Mint.h>
#include <MercuryVector.h>
#include <MercuryBacktrace.h>

#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

MString	ConvertToCFormat( const MString & ncf )
{
	MString ret;
	const char* nccf = ncf.c_str();

	for ( int i = 0; (unsigned)i < ncf.length(); i++ )
	{
		switch ( nccf[i] )
		{
		case '\t':	ret += "\\t";	break;
		case '\n':	ret += "\\n";	break;
		case '\r':	ret += "\\r";	break;
		case '\f':	ret += "\\f";	break;
		case '\b':	ret += "\\b";	break;
		case '\\':	ret += "\\\\";	break;
		case '\'':	ret += "\\\'";	break;
		case '\"':	ret += "\\\"";	break;
		default:
			if( nccf[i] < 32 )
			{
				ret += "\\"; 
				ret += ((unsigned char)nccf[i]/64)%8 + '0';
				ret += ((unsigned char)nccf[i]/8)%8 + '0';
				ret += (unsigned char)nccf[i]%8 + '0';
			} else
				ret += nccf[i];
		}
	}
	return ret;
}

MString ConvertToUnformatted( const MString & cf )
{
	MString ret;
	const char* ccf = cf.c_str();

	for ( int i = 0; (unsigned)i < cf.length(); i++ )
	{
		switch ( ccf[i] )
		{
		case '\\':
			i++;
			if ( (unsigned)i >= cf.length() )
				return ret;
			switch ( ccf[i] )
			{
			case 't':	ret += '\t';	break;
			case 'n':	ret += '\n';	break;
			case 'r':	ret += '\r';	break;
			case 'f':	ret += '\f';	break;
			case 'b':	ret += '\b';	break;
			case '\\':	ret += '\\';	break;
			case '\'':	ret += '\'';	break;
			case '\"':	ret += '\"';	break;
			default:
				if( ccf[i] >= '0' && ccf[i] <= '7' )
				{
					char c = ccf[i] - '0';
					if( ccf[i+1] >= '0' && ccf[i+1] <= '8' )
					{
						i++;
						c = c * 8 + ccf[i] - '0';
					}
					if( ccf[i+1] >= '0' && ccf[i+1] <= '8' )
					{
						i++;
						c = c * 8 + ccf[i] - '0';
					}
					ret += c;
				}
			}
			break;
		default:
			ret += ccf[i];
		}
	}
	return ret;
}

MString ToUpper(const MString& s)
{
	MString t = s;
	char * ti = (char*)t.c_str();
	for (unsigned long i = 0; i < s.length(); ++i)
	{
		if( ti[i] >= 'a' && ti[i] <= 'z' )
			ti[i] -= ( 'a' - 'A' );
	}
	return t;
}

MString ToLower(const MString & s)
{
	MString t = s;
	char * ti = (char*)t.c_str();
	for (unsigned long i = 0; i < s.length(); ++i)
	{
		if( ti[i] >= 'A' && ti[i] <= 'Z' )
			ti[i] += ( 'a' - 'A' );
	}
	return t;
}

MString ToProper(const MString & s)
{
	if( s.length() == 0 )
		return "";

	MString t = s;
	char * ti = (char*)t.c_str();

	if( ti[0] >= 'a' && ti[0] <= 'z' )
		ti[0] -= ( 'a' - 'A' );

	for (unsigned long i = 1; i < s.length(); ++i)
	{
		if( ti[i] >= 'A' && ti[i] <= 'Z' )
			ti[i] += ( 'a' - 'A' );
	}
	return t;

}



float StrToFloat(const  MString & s, float d)
{
	float x = d;
#if defined( WIN32 ) && !defined( LEAN_HG )
	if ( s.length() > 0) sscanf_s(s.c_str(), "%f", &x);
#else
	if ( s.length() > 0) sscanf(s.c_str(), "%f", &x);
#endif
	return x;
}

int32_t StrToInt(const  MString & s, int32_t d)
{
	int32_t x = d;
#if defined( WIN32 ) && !defined( LEAN_HG )
	if ( s.length() > 0) sscanf_s(s.c_str(), "%d", &x);
#else
	if ( s.length() > 0) sscanf(s.c_str(), "%d", &x);
#endif
	return x;
}

bool StrToBool( const MString & s, bool d )
{
	MString tmpret = s;

	tmpret = ToUpper( tmpret );
	if( tmpret.compare( "TRUE" ) == 0 )	return 1;
	if( tmpret.compare( "FALSE" ) == 0 )	return 0;
	if( tmpret.compare( "ON" ) == 0 )	return 1;
	if( tmpret.compare( "OFF" ) == 0 )	return 0;
	if( tmpret.compare( "YES" ) == 0 )	return 1;
	if( tmpret.compare( "NO" ) == 0 )	return 0;

	return StrToInt( tmpret, d ) != 0;
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

void* mmemalign(size_t align, size_t size, void*& mem)
{
	uintptr_t mask = ~(uintptr_t)(align - 1);
	mem = malloc(size+align-1);
	void *ptr = (void *)(((uintptr_t)mem+align-1) & mask);
	return ptr;
}

bool isAligned(size_t align, const void* mem)
{
	return (uintptr_t(mem) % align) == 0;
}

long FileToString( const MString & sFileName, char * & data )
{
	data = 0;

	MercuryFile * f = FILEMAN.Open( sFileName );
	if( !f ) return -1;

	int length = f->Length();

	data = (char*)malloc( length + 1 );

	if( !data )
	{
		data = 0;
		delete f;
		return -1;
	}

	int r = f->Read( data, length );

	delete f;

	if( r != length )
	{
		free( data );
		data = 0;
		return -1;
	}

	return length;
}

bool StringToFile( const MString & sFileName, const MString & data )
{
	MercuryFile * f = FILEMAN.Open( sFileName, MFP_WRITE_ONLY );
	if( !f ) return false;
	f->Write( data.c_str(), data.length() );
	delete f;
}


int GeneralUsePrimes[] = { 3, 13, 37, 73, 131, 229, 337, 821, 2477, 4594, 8941, 14797, 24953, 39041, 60811, 104729 };

int GetAPrime( int ith )
{
	return (ith<0)?3:(ith>15)?GeneralUsePrimes[15]:GeneralUsePrimes[ith];
}

//String processing functions
long	BytesUntil( const char* strin, const char * termin, long start, long slen, long termlen )
{
	int i;
	for ( i = start; i < slen; i++ )
		for ( int j = 0; j < termlen; j++ )
			if ( termin[j] == strin[i] )
				return i - start;
	return i - start;
}

long	BytesNUntil( const char* strin, const char * termin, long start, long slen, long termlen )
{
	int i;
	for ( i = start; i < slen; i++ )
	{
		bool found = false;
		for ( int j = 0; j < termlen; j++ )
		{
			if ( termin[j] == strin[i] )
				found = true;
		}
		if ( !found ) 
			return i - start;
	}
	return i - start;
}

void	SplitStrings( const MString & in, MVector < MString > & out, 
					 const char * termin, const char * whitespace, 
					 long termlen, long wslen )
{
	const char * inStr = in.c_str();
	long	curPos = 0;
	long	inLen = in.length();
	while ( curPos < inLen )
	{
		curPos += BytesNUntil( inStr, whitespace, curPos, inLen, wslen );
		long NextPos = BytesUntil( inStr, termin, curPos, inLen, termlen );
		out.push_back( in.substr( curPos, NextPos ) );
		curPos += NextPos + 1;
	}
}

void msleep(uint32_t msec)
{
#ifdef WIN32
	Sleep(msec);
#else
	usleep(msec*1000);
#endif
}

/* Copyright (c) 2009, Joshua Allen and Charles Lohr
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
