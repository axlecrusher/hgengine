#include <Mint.h>
#include "MercuryMath.h"

//the SSE version of this was really slow, this is quicker
void TransposeMatrix( FloatRow* m )
{
	float tmp;
	float *_m = (float*)m;
	
	tmp = _m[1];
	_m[1] = _m[4];
	_m[4] = tmp;

	tmp = _m[2];
	_m[2] = _m[8];
	_m[8] = tmp;
	tmp = _m[3];
	_m[3] = _m[12];
	_m[12] = tmp;

	tmp = _m[6];
	_m[6] = _m[9];
	_m[9] = tmp;
	tmp = _m[7];
	_m[7] = _m[13];
	_m[13] = tmp;

	tmp = _m[11];
	_m[11] = _m[14];
	_m[14] = tmp;
}

#ifndef USE_SSE

//Generic Math functions. Compile these if you can not use optimized functions.

void ZeroFloatRow(FloatRow& r)
{
	Copy4f(&r, &gfrZero );
}

void Mul4f(const FloatRow& first, const FloatRow& second, FloatRow& out)
{
	for (uint8_t i = 0; i < 4; ++i)
	    out[i] = first[i] * second[i];
}

void Div4f(const FloatRow& first, const FloatRow& second, FloatRow& out)
{
	for (uint8_t i = 0; i < 4; ++i)
		out[i] = first[i] / second[i];
}

void Add4f(const FloatRow& first, const FloatRow& second, FloatRow& out)
{
	for (uint8_t i = 0; i < 4; ++i)
		out[i] = first[i] + second[i];
}

void Sub4f(const FloatRow& first, const FloatRow& second, FloatRow& out)
{
	for (uint8_t i = 0; i < 4; ++i)
		out[i] = first[i] - second[i];
}

void Copy4f( void * dest, const void * source )
{
	COPY<float,4>((float*)source, (float*)dest);
}

void Copy8f( void * dest, const void * source )
{
	COPY<float,8>((float*)source, (float*)dest);
}

void Copy16f( void * dest, const void * source )
{
	COPY<float,16>((float*)source, (float*)dest);
}

void MatrixMultiply4f ( const FloatRow* in1a, const FloatRow* in2a, FloatRow* outa)
{
	const float *in1 = *in1a;
	const float *in2 = *in2a;
	float *out = *outa;
	
	out[0] = in1[0] * in2[0] + in1[1] * in2[4] +
				in1[2] * in2[8] + in1[3] * in2[12];
	out[1] = in1[0] * in2[1] + in1[1] * in2[5] +
				in1[2] * in2[9] + in1[3] * in2[13];
	out[2] = in1[0] * in2[2] + in1[1] * in2[6] +
				in1[2] * in2[10] + in1[3] * in2[14];
	out[3] = in1[0] * in2[3] + in1[1] * in2[7] +
				in1[2] * in2[11] + in1[3] * in2[15];

	out[4] = in1[4] * in2[0] + in1[5] * in2[4] +
				in1[6] * in2[8] + in1[7] * in2[12];
	out[5] = in1[4] * in2[1] + in1[5] * in2[5] +
				in1[6] * in2[9] + in1[7] * in2[13];
	out[6] = in1[4] * in2[2] + in1[5] * in2[6] +
				in1[6] * in2[10] + in1[7] * in2[14];
	out[7] = in1[4] * in2[3] + in1[5] * in2[7] +
				in1[6] * in2[11] + in1[7] * in2[15];

	out[8] = in1[8] * in2[0] + in1[9] * in2[4] +
				in1[10] * in2[8] + in1[11] * in2[12];
	out[9] = in1[8] * in2[1] + in1[9] * in2[5] +
				in1[10] * in2[9] + in1[11] * in2[13];
	out[10] = in1[8] * in2[2] + in1[9] * in2[6] +
				in1[10] * in2[10] + in1[11] * in2[14];
	out[11] = in1[8] * in2[3] + in1[9] * in2[7] +
				in1[10] * in2[11] + in1[11] * in2[15];

	out[12] = in1[12] * in2[0] + in1[13] * in2[4] +
				in1[14] * in2[8] + in1[15] * in2[12];
	out[13] = in1[12] * in2[1] + in1[13] * in2[5] +
				in1[14] * in2[9] + in1[15] * in2[13];
	out[14] = in1[12] * in2[2] + in1[13] * in2[6] +
				in1[14] * in2[10] + in1[15] * in2[14];
	out[15] = in1[12] * in2[3] + in1[13] * in2[7] +
				in1[14] * in2[11] + in1[15] * in2[15];
}

void VectorMultiply4f( const FloatRow* matrix, const FloatRow& pa, FloatRow& outa )
{
	const float *m = *matrix;
	const float *p = pa;
	float *out = outa;
	out[0] = p[0] * m[0] + p[1] * m[1] + p[2] * m[2] + p[3] * m[3];
	out[1] = p[0] * m[4] + p[1] * m[5] + p[2] * m[6] + p[3] * m[7];
	out[2] = p[0] * m[8] + p[1] * m[9] + p[2] * m[10] + p[3] * m[11];
	out[3] = p[0] * m[12] + p[1] * m[13] + p[2] * m[14] + p[3] * m[15];
}

void Float2FloatRow(const float* f, FloatRow& r)
{
	for (uint8_t i = 0; i < 4; ++i)
		r[i] = f[i];
}

void FloatRow2Float( const FloatRow& r, float* f)
{
	for (uint8_t i = 0; i < 4; ++i)
		f[i] = r[i];
}

void MMCrossProduct( const FloatRow& r1, const FloatRow& r2, FloatRow& result)
{
	result[0] = r1[1]*r2[2] - r1[2]*r2[1];
	result[1] = r1[2]*r2[0] - r1[0]*r2[2];
	result[2] = r1[0]*r2[1] - r1[1]*r2[0];
}

#else

//inline __m128 Hadd4(__m128 x);
__m128 Hadd4(__m128 x)
{
	//add the low and high components of x
	x = _mm_add_ps(x, _mm_movehl_ps(x, x));
	//add x[0] and x[1]
	return _mm_add_ps( x, _mm_shuffle_ps(x, x, _MM_SHUFFLE(1,1,1,1)) );
}

void Mul4f(const FloatRow& first, const FloatRow& second, FloatRow& out)
{
	out = _mm_mul_ps( first, second );
}

void Div4f(const FloatRow& first, const FloatRow& second, FloatRow& out)
{
	out = _mm_div_ps( first, second );
}

void Add4f(const FloatRow& first, const FloatRow& second, FloatRow& out)
{
	out = _mm_add_ps( first, second );
}

void Sub4f(const FloatRow& first, const FloatRow& second, FloatRow& out)
{
	out = _mm_sub_ps( first, second );
}

void Copy4f( void * dest, const void * source )
{
	_mm_stream_ps(((float*)dest),((__m128*)source)[0]);
}

void Copy8f( void * dest, const void * source )
{
	_mm_stream_ps(((float*)dest),((__m128*)source)[0]);
	_mm_stream_ps(((float*)dest)+4,((__m128*)source)[1]);
}

void Copy16f( void * dest, const void * source )
{
	_mm_stream_ps(((float*)dest),((__m128*)source)[0]);
	_mm_stream_ps(((float*)dest)+4,((__m128*)source)[1]);
	_mm_stream_ps(((float*)dest)+8,((__m128*)source)[2]);
	_mm_stream_ps(((float*)dest)+12,((__m128*)source)[3]);
}

void MatrixMultiply4f( const FloatRow* in1, const FloatRow* in2, FloatRow* out)
{
	unsigned int y;
	__m128 xmm[4];

//	PREFETCH(in1, _MM_HINT_T0);
//	PREFETCH(in2, _MM_HINT_T1);
//	PREFETCH(out, _MM_HINT_T1);

	for (y = 0; y < 4; ++y)
	{
		//load rows as columns
		xmm[3] = _mm_shuffle_ps (in1[y], in1[y], 0xff);
		xmm[2] = _mm_shuffle_ps (in1[y], in1[y], 0xaa);
		xmm[1] = _mm_shuffle_ps (in1[y], in1[y], 0x55);
		xmm[0] = _mm_shuffle_ps (in1[y], in1[y], 0x00);

		xmm[0] = _mm_mul_ps( xmm[0], in2[0] );
		xmm[1] = _mm_mul_ps( xmm[1], in2[1] );
		xmm[2] = _mm_mul_ps( xmm[2], in2[2] );
		xmm[3] = _mm_mul_ps( xmm[3], in2[3] );

		xmm[0] = _mm_add_ps( xmm[0], xmm[1] );
		xmm[2] = _mm_add_ps( xmm[2], xmm[3] );
		out[y] = _mm_add_ps( xmm[0], xmm[2] );
	}
}

//This is an SSE matrix vector multiply, see the standard C++ code
//for a clear algorithim.  This seems like it works.
void VectorMultiply4f( const FloatRow* matrix, const FloatRow& p, FloatRow& out )
{
	__m128 tmp, XY;
	
	//compute term X and term Y and store them in the low order of XY
	XY = Hadd4( _mm_mul_ps( matrix[0], p ) ); //compute X
	tmp = Hadd4( _mm_mul_ps( matrix[1], p ) ); //compute Y
	XY = _mm_unpacklo_ps(XY, tmp);

	//compute term Z and term W and store them in the low order of out
	out = Hadd4( _mm_mul_ps( matrix[2], p ) ); //compute Z
	tmp = Hadd4( _mm_mul_ps( matrix[3], p ) ); //compute W
	out = _mm_unpacklo_ps(out, tmp);

	//shuffle the low order of XY into the loworder of out
	//and shuffle the low order of out into the high order of out
	out = _mm_movelh_ps(XY, out);
}

void ZeroFloatRow(FloatRow& r)
{
	r = _mm_setzero_ps();
}

void Float2FloatRow(const float* f, FloatRow& r)
{
	r = _mm_load_ps( f );
}

void FloatRow2Float( const FloatRow& r, float* f)
{
	_mm_store_ps( f, r );
}

void MMCrossProduct( const FloatRow& r1, const FloatRow& r2, FloatRow& result)
{
	__m128 a,b,c,d,r;//using more registers is faster

	a = _mm_shuffle_ps(r1, r1, 0xc9);
	b = _mm_shuffle_ps(r2, r2, 0xd2);
	r = _mm_mul_ps( a, b );

	c = _mm_shuffle_ps(r2, r2, 0xc9);
	d = _mm_shuffle_ps(r1, r1, 0xd2);
	r -= _mm_mul_ps( c, d );
	result = r;
}

#endif

/*
 * (c) 2006 Joshua Allen, Charles Lohr
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
