#ifndef _MERCURYMATH_H
#define _MERCURYMATH_H

#include <math.h>

#ifdef USE_SSE
#include <xmmintrin.h>
typedef __m128 FloatRow __attribute__((aligned(16)));
#else
typedef float FloatRow[4];
#endif

void ZeroFloatRow(FloatRow& r);

#define DEGRAD	0.01745329251994329576f		//degree to radian
#define RADDEG	57.2957795130823208767f		//radian to degree
#define	Q_PI	3.14159265358979323846f

#if defined(WIN32)
//In win32, sin works faster than sinf and similar functions
#define SIN( x )		float( sin ( x ) )
#define COS( x )		float( cos ( x ) )
#define ATAN2( x, y )	float( atan2( x, y ) )
#define ASIN( x )		float( asin ( x ) )
#define ACOS( x )		float( acos ( x ) )
#define SQRT( x )		float( sqrt ( x ) )
#define	TAN( x )		float( tan ( x ) )
#define ABS( x )		float( ((x<0)?(-x):(x)) )
inline int LRINTF(float x) { int r = (int)x; (x-r)>=0.5?++r:0; return r; };
#else
//On other OSes in general, sinf works faster than floating a sin
#define SIN( x )		sinf( x )
#define COS( x )		cosf( x )
#define ATAN2( x, y )	atan2f( x, y )
#define ASIN( x )		asinf ( x )
#define ACOS( x )		acosf ( x )
#define SQRT( x )		sqrtf( x )
#define TAN( x )		tanf( x )
#define ABS( x )		((x<0)?(-x):(x))
#define LRINTF( x )     lrintf( x )
#endif

#define SQ(x) ((x)*(x));

//#define DotProduct(x,y) ((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])

void Mul4f(const FloatRow* first, const FloatRow* second, FloatRow* out);
void Div4f(const FloatRow* first, const FloatRow* second, FloatRow* out);
void Add4f(const FloatRow* first, const FloatRow* second, FloatRow* out);
void Sub4f(const FloatRow* first, const FloatRow* second, FloatRow* out);
void Copy4f( void * dest, const void * source );
void Copy8f( void * dest, const void * source );
void Copy16f( void * dest, const void * source );
void MatrixMultiply4f ( const FloatRow* in1, const FloatRow* in2, FloatRow* out );
void VectorMultiply4f(const FloatRow* matrix, const FloatRow* p, FloatRow* out );
void TransposeMatrix( FloatRow* m );

void Float2FloatRow(const float* f, FloatRow* r);
void FloatRow2Float( const FloatRow* fr, float* f);

const FloatRow gfrZero = { 0.f, 0.f, 0.f, 0.f };

#endif

/*
 * (c) 2006 Joshua Allen
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

