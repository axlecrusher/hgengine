#ifndef MERCURYUTIL_H
#define MERCURYUTIL_H

#include <stdlib.h>
#include <global.h>
#include <stdio.h>

#define SAFE_DELETE( x ) { if (x) { delete x; } x = NULL; }
#define SAFE_DELETE_ARRAY( x ) { if (x) { delete[] x; } x = NULL; }
#define SAFE_FREE(p)       { if(p) { free(p); p=0; } }

#define TO_ENDIAN( x )

//returns an aligned pointer, mem is the actual (unaligned) pointer for freeing
void* mmemalign(size_t align, size_t size, void*& mem);
bool isAligned(size_t align, const void* mem);

template<typename T>
const T& MAX(const T& t1, const T& t2)
{
	return t1>t2?t1:t2;
}

template<typename T>
const T& MIN(const T& t1, const T& t2)
{
	return t1<t2?t1:t2;
}

template<typename T>
const T& Clamp(const T& min, const T& max, const T& value)
{
	if (value > max) return max;
	if (value < min) return min;
	return value;
}

/*  These two functions are very different */
/// nextPow2 will go to the NEXT power of 2 even if x is already a power of 2.
inline int nextPow2(int x) { int num = 1; while(num <= x) num <<= 1; return num; }
///makePow2 will make sure the number is a power of 2 at least equal to x.
inline int makePow2(int x) { int num = 1; while(num < x) num <<= 1; return num; }
inline bool isPow2(int x) { int num = 1; while(num < x) num<<=1; return num==x; }

///Return a prime number.  Note this is NOT sequential.  The general(but not guarenteed)
///pattern is the next prime number that's roughly two times the last.
int GetAPrime( int ith );




//String processing functions
//XXX: This may not be portable. We do not in fact need to include the header for MVector yet.
template<typename T>
class MVector;

///Bytes until desired terminal
long	BytesUntil( const char* strin, const char * termin, long start, long slen, long termlen );

///Bytes until something other than a terminal
long	BytesNUntil( const char* strin, const char * termin, long start, long slen, long termlen );

///millisecond sleep
void msleep(uint32_t msec);

///Utility linear function, in = [0..1] out = [0..1]; respectively, if slice >= 1
float FLinear( float in, float slice = 1. );

///Utility exponential function, in = [0..1] out = [0..1]; respectively; regardless of pow.  If pow == 1, would be identical to linear.
float FExponential( float in, float powx = 1. );

///Utility step function; out = 0 when in < stepplace; out = 1 when in >= stepplace
float FStep( float in, float stepplace = 1. );

///Utility sigmoid function; in = [0..1] out = [0..1]; speed is the slope of change in the middle.
float FSigmoid( float in, float fspeed = 1. );

#endif

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
