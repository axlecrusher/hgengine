#include <MercuryUtil.h>
#include <MercuryFile.h>
#include <stdint.h>

#ifndef WIN32
#include <sys/time.h>
#else
#include <windows.h>
#endif

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

float StrToFloat(const  MString & s)
{
	float x;
	sscanf(s.c_str(), "%f", &x);
	return x;
}

void* mmemalign(size_t align, size_t size, void*& mem)
{
	uintptr_t mask = ~(uintptr_t)(align - 1);
	mem = malloc(size+align-1);
	void *ptr = (void *)(((uintptr_t)mem+align-1) & mask);
	return ptr;
}

int64_t GetTimeInMicroSeconds()
{
	struct timeval tv;
	gettimeofday( &tv, 0 );
	
	return (int64_t(tv.tv_sec) * 1000000) + tv.tv_usec;

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
		return -1;
	}

	int r = f->Read( data, length );

	if( r != length )
	{
		free( data );
		data = 0;
		return -1;
	}

	return length;
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
 *     * Neither the name of the <ORGANIZATION> nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission. *
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
