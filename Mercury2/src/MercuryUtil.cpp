#include <MercuryUtil.h>
#include <Mint.h>
#include <math.h>

#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

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

void msleep(uint32_t msec)
{
#ifdef WIN32
	Sleep(msec);
#else
	usleep(msec*1000);
#endif
}

float FLinear( float in, float slice )
{
	float fret = (in - 0.5f) * slice;

	if( fret > 0.5f ) fret = 0.5f;
	else if( fret < -0.5f ) fret = -0.5f;
	return fret + 0.5f;
}

float FExponential( float in, float powx )
{
	return pow( in, powx );
}

float FStep( float in, float stepplace )
{
	return ( in < stepplace )?0.0f:1.0f;
}

float FSigmoid( float in, float fspeed )
{
	float fi = in - 0.5f;
	float fr = ( exp( fi * fspeed ) - 1.0f ) / ( exp( fi * fspeed ) + 1.0f );
	float smax = ( exp( fspeed * 0.5f ) - 1.0f ) / ( exp( fspeed * 0.5f ) + 1.0f );
	return (fr / smax) / 2.0f + 0.5f;
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
