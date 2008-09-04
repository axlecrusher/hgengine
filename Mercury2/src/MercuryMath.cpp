#include "MercuryMath.h"

#if !defined( USE_SSE )

//Generic Math functions. Compile these if you can not use optimized functions.

void Mul4f(const float* first, const float* second, float* out)
{
    out[0] = first[0] * second[0];
    out[1] = first[1] * second[1];
    out[2] = first[2] * second[2];
    out[3] = first[3] * second[3];
}

void Div4f(const float* first, const float* second, float* out)
{
    out[0] = first[0] / second[0];
    out[1] = first[1] / second[1];
    out[2] = first[2] / second[2];
    out[3] = first[3] / second[3];
}

void Add4f(const float* first, const float* second, float* out)
{
    out[0] = first[0] + second[0];
    out[1] = first[1] + second[1];
    out[2] = first[2] + second[2];
    out[3] = first[3] + second[3];
}

void Sub4f(const float* first, const float* second, float* out)
{
    out[0] = first[0] - second[0];
    out[1] = first[1] - second[1];
    out[2] = first[2] - second[2];
    out[3] = first[3] - second[3];
}

void Copy4f( void * dest, const void * source )
{
    ((float*)dest)[0] = ((float*)source)[0];
    ((float*)dest)[1] = ((float*)source)[1];
    ((float*)dest)[2] = ((float*)source)[2];
    ((float*)dest)[3] = ((float*)source)[3];
}

void Copy8f( void * dest, const void * source )
{
    ((float*)dest)[0] = ((float*)source)[0];
    ((float*)dest)[1] = ((float*)source)[1];
    ((float*)dest)[2] = ((float*)source)[2];
    ((float*)dest)[3] = ((float*)source)[3];

    ((float*)dest)[4] = ((float*)source)[4];
    ((float*)dest)[5] = ((float*)source)[5];
    ((float*)dest)[6] = ((float*)source)[6];
    ((float*)dest)[7] = ((float*)source)[7];
}

void Copy16f( void * dest, const void * source )
{
    ((float*)dest)[0] = ((float*)source)[0];
    ((float*)dest)[1] = ((float*)source)[1];
    ((float*)dest)[2] = ((float*)source)[2];
    ((float*)dest)[3] = ((float*)source)[3];

    ((float*)dest)[4] = ((float*)source)[4];
    ((float*)dest)[5] = ((float*)source)[5];
    ((float*)dest)[6] = ((float*)source)[6];
    ((float*)dest)[7] = ((float*)source)[7];

    ((float*)dest)[8] = ((float*)source)[8];
    ((float*)dest)[9] = ((float*)source)[9];
    ((float*)dest)[10] = ((float*)source)[10];
    ((float*)dest)[11] = ((float*)source)[11];

    ((float*)dest)[12] = ((float*)source)[12];
    ((float*)dest)[13] = ((float*)source)[13];
    ((float*)dest)[14] = ((float*)source)[14];
    ((float*)dest)[15] = ((float*)source)[15];
}

void R_ConcatTransforms4 ( const float* in1, const float* in2, float* out)
{
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

void VectorMultiply4f( const float * m, float *p, float *out )
{
	out[0] = p[0] * m[0] + p[1] * m[1] + p[2] * m[2] + p[3] * m[3];
	out[1] = p[0] * m[4] + p[1] * m[5] + p[2] * m[6] + p[3] * m[7];
	out[2] = p[0] * m[8] + p[1] * m[9] + p[2] * m[10] + p[3] * m[11];
	out[3] = p[0] * m[12] + p[1] * m[13] + p[2] * m[14] + p[3] * m[15];
}

#else
#include <xmmintrin.h>

inline __m128 Hadd4(__m128 x);
__m128 Hadd4(__m128 x)
{
	//add the low and high components of x
	x = _mm_add_ps(x, _mm_movehl_ps(x, x));
	//add x[0] and x[1]
	return _mm_add_ps( x, _mm_shuffle_ps(x, x, _MM_SHUFFLE(1,1,1,1)) );
}

void Mul4f(const float* first, const float* second, float* out)
{
	__m128 xmm[2];

	xmm[0] = _mm_load_ps(first);
	xmm[1] = _mm_load_ps(second);

	xmm[0] = _mm_mul_ps( xmm[0], xmm[1] );

	_mm_store_ps(out, xmm[0]);
}

void Div4f(const float* first, const float* second, float* out)
{
	__m128 xmm[2];

	xmm[0] = _mm_load_ps(first);
	xmm[1] = _mm_load_ps(second);

	xmm[0] = _mm_div_ps( xmm[0], xmm[1] );

	_mm_store_ps(out, xmm[0]);
}

void Add4f(const float* first, const float* second, float* out)
{
	__m128 xmm[2];

	xmm[0] = _mm_load_ps(first);
	xmm[1] = _mm_load_ps(second);

	xmm[0] = _mm_add_ps( xmm[0], xmm[1] );

	_mm_store_ps(out, xmm[0]);

}

void Sub4f(const float* first, const float* second, float* out)
{
	__m128 xmm[2];

	xmm[0] = _mm_load_ps(first);
	xmm[1] = _mm_load_ps(second);

	xmm[0] = _mm_sub_ps( xmm[0], xmm[1] );

	_mm_store_ps(out, xmm[0]);

}

void Copy4f( void * dest, const void * source )
{
	__m128 xmm;

	xmm = _mm_load_ps((float*)source);
	_mm_store_ps((float*)dest, xmm);
}

void Copy8f( void * dest, const void * source )
{
	__m128 xmm[2];

	xmm[0] = _mm_load_ps((float*)source);
	_mm_store_ps((float*)dest, xmm[0]);

	xmm[1] = _mm_load_ps((float*)&(((float*)source)[4]));
	_mm_store_ps((float*)&(((float*)dest)[4]), xmm[1]);
}

void Copy16f( void * dest, const void * source )
{
	__m128 xmm[4];

	xmm[0] = _mm_load_ps((float*)source);
	_mm_store_ps((float*)dest, xmm[0]);

	xmm[1] = _mm_load_ps((float*)&(((float*)source)[4]));
	_mm_store_ps((float*)&(((float*)dest)[4]), xmm[1]);

	xmm[2] = _mm_load_ps((float*)&(((float*)source)[8]));
	_mm_store_ps((float*)&(((float*)dest)[8]), xmm[2]);

	xmm[3] = _mm_load_ps((float*)&(((float*)source)[12]));
	_mm_store_ps((float*)&(((float*)dest)[12]), xmm[3]);
}

void R_ConcatTransforms4 ( const float* in1, const float* in2, float* out)
{
	unsigned int x, y;
	__m128 xmm[8];

	xmm[1] = _mm_load_ps((float*)&(in2[0]));
	xmm[3] = _mm_load_ps((float*)&(in2[4]));
	xmm[5] = _mm_load_ps((float*)&(in2[8]));
	xmm[7] = _mm_load_ps((float*)&(in2[12]));

	for (y = 0; y < 4; ++y)
	{
		xmm[0] = _mm_set_ps1(in1[(y*4)+0]);
		xmm[2] = _mm_set_ps1(in1[(y*4)+1]);
		xmm[4] = _mm_set_ps1(in1[(y*4)+2]);
		xmm[6] = _mm_set_ps1(in1[(y*4)+3]);

		xmm[0] = _mm_mul_ps( xmm[0], xmm[1] );
		xmm[2] = _mm_mul_ps( xmm[2], xmm[3] );
		xmm[4] = _mm_mul_ps( xmm[4], xmm[5] );
		xmm[6] = _mm_mul_ps( xmm[6], xmm[7] );

		xmm[0] = _mm_add_ps( xmm[0], xmm[2] );
		xmm[4] = _mm_add_ps( xmm[4], xmm[6] );
		xmm[0] = _mm_add_ps( xmm[0], xmm[4] );

		_mm_store_ps(&(out[(y*4)]), xmm[0]);
	}
}

//This is an SSE matrix vector multiply, see the standard C++ code
//for a clear algorithim.  This seems like it works.
void VectorMultiply4f( const float * m, float *p, float *out )
{
	__m128 xmm[5], outxmm[2], tmp;

	xmm[0] = _mm_load_ps((float*)p); //the vector

	//store the matrix
	xmm[1] = _mm_load_ps((float*)&(m[0]));
	xmm[2] = _mm_load_ps((float*)&(m[4]));
	xmm[3] = _mm_load_ps((float*)&(m[8]));
	xmm[4] = _mm_load_ps((float*)&(m[12]));

	//compute term 1 and term 2 and store them in the low order
	//of outxmm[0]
	outxmm[0] = Hadd4( _mm_mul_ps( xmm[1], xmm[0] ) );
	tmp = Hadd4( _mm_mul_ps( xmm[2], xmm[0] ) );
	outxmm[0] = _mm_unpacklo_ps(outxmm[0], tmp);

	//compute term 3 and term 4 and store them in the high order
	//of outxmm[1]
	outxmm[1] = Hadd4( _mm_mul_ps( xmm[3], xmm[0] ) );
	tmp = Hadd4( _mm_mul_ps( xmm[4], xmm[0] ) );
	outxmm[1] = _mm_unpacklo_ps(outxmm[1], tmp);

	//shuffle the low order of outxmm[0] into the loworder of tmp
	//and shuffle the low order of outxmm[1] into the high order of tmp
	tmp = _mm_movelh_ps(outxmm[0], outxmm[1]);

	_mm_store_ps(out, tmp);
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
