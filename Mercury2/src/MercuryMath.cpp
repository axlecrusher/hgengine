#include <Mint.h>
#include "MercuryMath.h"

//the SSE version of this was really slow, this is quicker

#ifndef USE_SSE

//Generic Math functions. Compile these if you can not use optimized functions.

void TransposeMatrix( const FloatRow* matrix, FloatRow* out )
{
	float tmp;
	float *_m = (float*)out;
	
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

void ZeroFloatRow(FloatRow& r)
{
	Copy4f(&r, &gfrZero );
}

void Mul4f(const FloatRow& first, const FloatRow& second, FloatRow& out)
{
	FloatRow r;
	for (uint8_t i = 0; i < 4; ++i)
	    r[i] = first[i] * second[i];
	Copy4f(out,r);
}

void Div4f(const FloatRow& first, const FloatRow& second, FloatRow& out)
{
	FloatRow r;
	for (uint8_t i = 0; i < 4; ++i)
		out[i] = first[i] / second[i];
	Copy4f(out,r);
}

void Add4f(const FloatRow& first, const FloatRow& second, FloatRow& out)
{
	FloatRow r;
	for (uint8_t i = 0; i < 4; ++i)
		out[i] = first[i] + second[i];
	Copy4f(out,r);
}

void Sub4f(const FloatRow& first, const FloatRow& second, FloatRow& out)
{
	FloatRow r;
	for (uint8_t i = 0; i < 4; ++i)
		out[i] = first[i] - second[i];
	Copy4f(out,r);
}

void MatrixMultiply4f ( const FloatRow* in1a, const FloatRow* in2a, FloatRow* outa)
{
	const float *in1 = *in1a;
	const float *in2 = *in2a;
	FloatRow r[4];
	
	(*r)[0] = in1[0] * in2[0] + in1[1] * in2[4] +
				in1[2] * in2[8] + in1[3] * in2[12];
	(*r)[1] = in1[0] * in2[1] + in1[1] * in2[5] +
				in1[2] * in2[9] + in1[3] * in2[13];
	(*r)[2] = in1[0] * in2[2] + in1[1] * in2[6] +
				in1[2] * in2[10] + in1[3] * in2[14];
	(*r)[3] = in1[0] * in2[3] + in1[1] * in2[7] +
				in1[2] * in2[11] + in1[3] * in2[15];

	(*r)[4] = in1[4] * in2[0] + in1[5] * in2[4] +
				in1[6] * in2[8] + in1[7] * in2[12];
	(*r)[5] = in1[4] * in2[1] + in1[5] * in2[5] +
				in1[6] * in2[9] + in1[7] * in2[13];
	(*r)[6] = in1[4] * in2[2] + in1[5] * in2[6] +
				in1[6] * in2[10] + in1[7] * in2[14];
	(*r)[7] = in1[4] * in2[3] + in1[5] * in2[7] +
				in1[6] * in2[11] + in1[7] * in2[15];

	(*r)[8] = in1[8] * in2[0] + in1[9] * in2[4] +
				in1[10] * in2[8] + in1[11] * in2[12];
	(*r)[9] = in1[8] * in2[1] + in1[9] * in2[5] +
				in1[10] * in2[9] + in1[11] * in2[13];
	(*r)[10] = in1[8] * in2[2] + in1[9] * in2[6] +
				in1[10] * in2[10] + in1[11] * in2[14];
	(*r)[11] = in1[8] * in2[3] + in1[9] * in2[7] +
				in1[10] * in2[11] + in1[11] * in2[15];

	(*r)[12] = in1[12] * in2[0] + in1[13] * in2[4] +
				in1[14] * in2[8] + in1[15] * in2[12];
	(*r)[13] = in1[12] * in2[1] + in1[13] * in2[5] +
				in1[14] * in2[9] + in1[15] * in2[13];
	(*r)[14] = in1[12] * in2[2] + in1[13] * in2[6] +
				in1[14] * in2[10] + in1[15] * in2[14];
	(*r)[15] = in1[12] * in2[3] + in1[13] * in2[7] +
				in1[14] * in2[11] + in1[15] * in2[15];

	Copy16f(outa,r);
}

void VectorMultiply4f( const FloatRow* matrix, const FloatRow& pa, FloatRow& outa )
{
	FloatRow r;
	const float *m = *matrix;
	const float *p = pa;
	
	r[0] = p[0] * m[0] + p[1] * m[1] + p[2] * m[2] + p[3] * m[3];
	r[1] = p[0] * m[4] + p[1] * m[5] + p[2] * m[6] + p[3] * m[7];
	r[2] = p[0] * m[8] + p[1] * m[9] + p[2] * m[10] + p[3] * m[11];
	r[3] = p[0] * m[12] + p[1] * m[13] + p[2] * m[14] + p[3] * m[15];
	
	Copy4f(outa,r);
}
/*
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
*/
void LoadIdentity(FloatRow* matrix)
{
	float *m = (float*)matrix;
	for (uint8_t i = 0; i < 16; ++i)
		m[i] = i%5?0.0f:1.0f;
}

void MMCrossProduct( const FloatRow& r1, const FloatRow& r2, FloatRow& result)
{
	FloatRow r;
	Copy4f(r,result); //we needs value4
	
	r[0] = r1[1]*r2[2] - r1[2]*r2[1];
	r[1] = r1[2]*r2[0] - r1[0]*r2[2];
	r[2] = r1[0]*r2[1] - r1[1]*r2[0];
	
	Copy4f(result,r);
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
	__m128 a,b,o;
	a = _mm_loadu_ps(first);
	b = _mm_loadu_ps(second);
	o = _mm_mul_ps( a, b );
	_mm_storeu_ps(out,o);
}

void Div4f(const FloatRow& first, const FloatRow& second, FloatRow& out)
{
	__m128 a,b,o;
	a = _mm_loadu_ps(first);
	b = _mm_loadu_ps(second);
	o = _mm_div_ps( a, b );
	_mm_storeu_ps(out,o);
}

void Add4f(const FloatRow& first, const FloatRow& second, FloatRow& out)
{
	__m128 a,b,o;
	a = _mm_loadu_ps(first);
	b = _mm_loadu_ps(second);
	o = _mm_add_ps( a, b );
	_mm_storeu_ps(out,o);
}

void Sub4f(const FloatRow& first, const FloatRow& second, FloatRow& out)
{
	__m128 a,b,o;
	a = _mm_loadu_ps(first);
	b = _mm_loadu_ps(second);
	o = _mm_sub_ps( a, b );
	_mm_storeu_ps(out,o);
}

void MatrixMultiply4f( const FloatRow* in1, const FloatRow* in2, FloatRow* out)
{
	unsigned int y = 0;
	__m128 xmm[4], b[4];

	b[3] = _mm_load_ps(in1[0]); //use b3 as temporary storage for matrix1 row1

	//start loading matrix2
	b[0] = _mm_load_ps(in2[0]);
	b[1] = _mm_load_ps(in2[1]);
	b[2] = _mm_load_ps(in2[2]);
		
	//load row1 of matrix1 into columns
	xmm[0] = _mm_shuffle_ps (b[3], b[3], 0x00);
	xmm[1] = _mm_shuffle_ps (b[3], b[3], 0x55);
	xmm[2] = _mm_shuffle_ps (b[3], b[3], 0xaa);
	xmm[3] = _mm_shuffle_ps (b[3], b[3], 0xff);

	b[3] = _mm_load_ps(in2[3]); //finish loading matrix2, do not change b after this!
/*
	do
	{
		xmm[0] = _mm_mul_ps( xmm[0], b[0] );
		xmm[1] = _mm_mul_ps( xmm[1], b[1] );
		xmm[0] = _mm_add_ps( xmm[0], xmm[1] ); //done with xmm1

		xmm[1] = _mm_load_ps(in1[y+1]); //load next row, shuffle this last

		xmm[2] = _mm_mul_ps( xmm[2], b[2] );
		xmm[3] = _mm_mul_ps( xmm[3], b[3] );
		xmm[2] = _mm_add_ps( xmm[2], xmm[3] ); //done with xmm3

		xmm[2] = _mm_add_ps( xmm[0], xmm[2] ); //final result
		_mm_store_ps(out[y++], xmm[2]);

		xmm[3] = _mm_shuffle_ps (xmm[1], xmm[1], 0xff);
		xmm[0] = _mm_shuffle_ps (xmm[1], xmm[1], 0x00);
		xmm[2] = _mm_shuffle_ps (xmm[1], xmm[1], 0xaa);
		xmm[1] = _mm_shuffle_ps (xmm[1], xmm[1], 0x55); 
	}	while (y < 4);
*/
	//manually unroll loop, much faster!!
	//loop 1
	xmm[0] = _mm_mul_ps( xmm[0], b[0] );
	xmm[1] = _mm_mul_ps( xmm[1], b[1] );
	xmm[0] = _mm_add_ps( xmm[0], xmm[1] ); //done with xmm1

	xmm[1] = _mm_load_ps(in1[1]); //load next row, shuffle this last

	xmm[2] = _mm_mul_ps( xmm[2], b[2] );
	xmm[3] = _mm_mul_ps( xmm[3], b[3] );
	xmm[2] = _mm_add_ps( xmm[2], xmm[3] ); //done with xmm3

	xmm[2] = _mm_add_ps( xmm[0], xmm[2] ); //final result
	_mm_store_ps(out[0], xmm[2]);

	xmm[3] = _mm_shuffle_ps (xmm[1], xmm[1], 0xff);
	xmm[0] = _mm_shuffle_ps (xmm[1], xmm[1], 0x00);
	xmm[2] = _mm_shuffle_ps (xmm[1], xmm[1], 0xaa);
	xmm[1] = _mm_shuffle_ps (xmm[1], xmm[1], 0x55);

	//loop2
	xmm[0] = _mm_mul_ps( xmm[0], b[0] );
	xmm[1] = _mm_mul_ps( xmm[1], b[1] );
	xmm[0] = _mm_add_ps( xmm[0], xmm[1] ); //done with xmm1

	xmm[1] = _mm_load_ps(in1[2]); //load next row, shuffle this last

	xmm[2] = _mm_mul_ps( xmm[2], b[2] );
	xmm[3] = _mm_mul_ps( xmm[3], b[3] );
	xmm[2] = _mm_add_ps( xmm[2], xmm[3] ); //done with xmm3

	xmm[2] = _mm_add_ps( xmm[0], xmm[2] ); //final result
	_mm_store_ps(out[1], xmm[2]);

	xmm[3] = _mm_shuffle_ps (xmm[1], xmm[1], 0xff);
	xmm[0] = _mm_shuffle_ps (xmm[1], xmm[1], 0x00);
	xmm[2] = _mm_shuffle_ps (xmm[1], xmm[1], 0xaa);
	xmm[1] = _mm_shuffle_ps (xmm[1], xmm[1], 0x55);

	//loop3
	xmm[0] = _mm_mul_ps( xmm[0], b[0] );
	xmm[1] = _mm_mul_ps( xmm[1], b[1] );
	xmm[0] = _mm_add_ps( xmm[0], xmm[1] ); //done with xmm1

	xmm[1] = _mm_load_ps(in1[3]); //load next row, shuffle this last

	xmm[2] = _mm_mul_ps( xmm[2], b[2] );
	xmm[3] = _mm_mul_ps( xmm[3], b[3] );
	xmm[2] = _mm_add_ps( xmm[2], xmm[3] ); //done with xmm3

	xmm[2] = _mm_add_ps( xmm[0], xmm[2] ); //final result
	_mm_store_ps(out[2], xmm[2]);

	xmm[3] = _mm_shuffle_ps (xmm[1], xmm[1], 0xff);
	xmm[0] = _mm_shuffle_ps (xmm[1], xmm[1], 0x00);
	xmm[2] = _mm_shuffle_ps (xmm[1], xmm[1], 0xaa);
	xmm[1] = _mm_shuffle_ps (xmm[1], xmm[1], 0x55);

	//loop4
	xmm[0] = _mm_mul_ps( xmm[0], b[0] );
	xmm[1] = _mm_mul_ps( xmm[1], b[1] );
	xmm[0] = _mm_add_ps( xmm[0], xmm[1] ); //done with xmm1

	xmm[2] = _mm_mul_ps( xmm[2], b[2] );
	xmm[3] = _mm_mul_ps( xmm[3], b[3] );
	xmm[2] = _mm_add_ps( xmm[2], xmm[3] ); //done with xmm3

	xmm[2] = _mm_add_ps( xmm[0], xmm[2] ); //final result
	_mm_store_ps(out[3], xmm[2]);
}

//This is an SSE matrix vector multiply, see the standard C++ code
//for a clear algorithim.  This seems like it works.
void VectorMultiply4f( const FloatRow* matrix, const FloatRow& p, FloatRow& out )
{
	__m128 tmp,tmp2, XY, pp;
	
	//load and loadu seem to run at nearly the same speed
	//see benchmark file
	pp=_mm_loadu_ps(p);
//	pp=_mm_load_ps(p);

	//this function can run long so try to move the output clocer to the CPU while function is running
	PREFETCH((const char*)out,_MM_HINT_T1);
	
	//compute term X and term Y and store them in the low order of XY
	XY = Hadd4( _mm_mul_ps( _mm_load_ps(matrix[0]), pp ) ); //compute X
	tmp = Hadd4( _mm_mul_ps( _mm_load_ps(matrix[1]), pp ) ); //compute Y
	XY = _mm_unpacklo_ps(XY, tmp);

	//compute term Z and term W and store them in the low order of out
	tmp2 = Hadd4( _mm_mul_ps( _mm_load_ps(matrix[2]), pp ) ); //compute Z
	tmp = Hadd4( _mm_mul_ps( _mm_load_ps(matrix[3]), pp ) ); //compute W
	pp = _mm_unpacklo_ps(tmp2, tmp);

	//shuffle the low order of XY into the loworder of out
	//and shuffle the low order of out into the high order of out
	tmp = _mm_movelh_ps(XY, pp);
	
	_mm_storeu_ps(out, tmp);
}
/*
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
*/

void LoadIdentity(FloatRow* matrix)
{
	float *m = (float*)matrix;

	__m128 r[2];

	r[0] = _mm_set_ss(1.0f);
	_mm_storer_ps(m+12,r[0]); //reverse r[0]

	r[1] = _mm_shuffle_ps(r[0], r[0], _MM_SHUFFLE(1,1,0,1));
	_mm_storer_ps(m+8,r[1]); //reverse r[1]

	_mm_store_ps(m, r[0]);
	_mm_store_ps(m+4, r[1]);
}

void TransposeMatrix( const FloatRow* matrix, FloatRow* out )
{
	//compiler acts better when we send in 2 parameter rather than 1
	__m128 m[4],r[4];

	m[0] = _mm_load_ps(matrix[0]);
	m[1] = _mm_load_ps(matrix[1]);
	m[2] = _mm_load_ps(matrix[2]);
	m[3] = _mm_load_ps(matrix[3]);

	r[0] = _mm_movelh_ps(m[0],m[1]);
	r[1] = _mm_movelh_ps(m[2],m[3]);
	r[2] = _mm_movehl_ps(m[1],m[0]);
	r[3] = _mm_movehl_ps(m[3],m[2]);
	//done with m matrix, we can reuse it now

	m[0] = _mm_shuffle_ps(r[0], r[0], _MM_SHUFFLE(3,1,2,0)); //produce beginning of new row 0 and 1
	m[1] = _mm_shuffle_ps(r[1], r[1], _MM_SHUFFLE(3,1,2,0)); //produce ending of new row 0 and 1
	m[2] = _mm_shuffle_ps(r[2], r[2], _MM_SHUFFLE(3,1,2,0)); //produce beginning of new row 2 and 3
	m[3] = _mm_shuffle_ps(r[3], r[3], _MM_SHUFFLE(3,1,2,0)); //produce ending of new row 2 and 3

	r[0] = _mm_movelh_ps(m[0],m[1]); //row 0 is done
	r[2] = _mm_movelh_ps(m[2],m[3]); //row 2 is done
	r[1] = _mm_movehl_ps(m[1],m[0]); //row 1 is done
	r[3] = _mm_movehl_ps(m[3],m[2]); //row 3 is done

	_mm_store_ps(out[0], r[0]);
	_mm_store_ps(out[1], r[1]);
	_mm_store_ps(out[2], r[2]);
	_mm_store_ps(out[3], r[3]);
}

void MMCrossProduct( const FloatRow& r1, const FloatRow& r2, FloatRow& result)
{
	//using more registers is faster(8 maximum)
	__m128 a,b,c,d,r;
	__m128 t1,t2;
	
	//unaligned load, vectors are not aligned
	t1 = _mm_loadu_ps(r1);
	t2 = _mm_loadu_ps(r2);

	a = _mm_shuffle_ps(t1, t1, 0xc9);
	b = _mm_shuffle_ps(t2, t2, 0xd2);
	r = _mm_mul_ps( a, b );

	c = _mm_shuffle_ps(t2, t2, 0xc9);
	d = _mm_shuffle_ps(t1, t1, 0xd2);
	a = _mm_mul_ps( c, d );
	r = _mm_sub_ps(r,a);

	_mm_storeu_ps(result, r);
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
