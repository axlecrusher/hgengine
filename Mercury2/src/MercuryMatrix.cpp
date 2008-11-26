#include "MercuryMatrix.h"

namespace MercuryMath
{
	MercuryMatrix IdentityMatrix;
}

MercuryMatrix::MercuryMatrix()
{
	Identity();
}

const MercuryMatrix& MercuryMatrix::operator=(const MercuryMatrix& m)
{
	Copy16f(m_matrix, m.m_matrix);
	return *this;
}

const MercuryMatrix& MercuryMatrix::operator=(const float* m)
{
	Copy16f(m_matrix, m);
	return *this;
}

void MercuryMatrix::Zero()
{
	m_matrix[0][0] = 0;
	m_matrix[0][1] = 0;
	m_matrix[0][2] = 0;
	m_matrix[0][3] = 0;
	
	m_matrix[1][0] = 0;
	m_matrix[1][1] = 0;
	m_matrix[1][2] = 0;
	m_matrix[1][3] = 0;
	
	m_matrix[2][0] = 0;
	m_matrix[2][1] = 0;
	m_matrix[2][2] = 0;
	m_matrix[2][3] = 0;
	
	m_matrix[3][0] = 0;
	m_matrix[3][1] = 0;
	m_matrix[3][2] = 0;
	m_matrix[3][3] = 0;
}

void MercuryMatrix::Identity()
{
	m_matrix[0][0] = 1;
	m_matrix[0][1] = 0;
	m_matrix[0][2] = 0;
	m_matrix[0][3] = 0;
	
	m_matrix[1][0] = 0;
	m_matrix[1][1] = 1;
	m_matrix[1][2] = 0;
	m_matrix[1][3] = 0;
	
	m_matrix[2][0] = 0;
	m_matrix[2][1] = 0;
	m_matrix[2][2] = 1;
	m_matrix[2][3] = 0;
	
	m_matrix[3][0] = 0;
	m_matrix[3][1] = 0;
	m_matrix[3][2] = 0;
	m_matrix[3][3] = 1;
}

void MercuryMatrix::Translate(float x, float y, float z)
{
	MercuryMatrix m;
	m.m_matrix[0][3] = x;
	m.m_matrix[1][3] = y;
	m.m_matrix[2][3] = z;
	*this *= m;
}

void MercuryMatrix::RotateXYZ(float x, float y, float z)
{
	//x,y,z must be negated for some reason
	float X = -x*2*Q_PI/360; //Reduced calulation for speed
	float Y = -y*2*Q_PI/360;
	float Z = -z*2*Q_PI/360;
	float cx = COS(X);
	float sx = SIN(X);
	float cy = COS(Y);
	float sy = SIN(Y);
	float cz = COS(Z);
	float sz = SIN(Z);

	MercuryMatrix matrix;

	//Row major
	//manually transposed
	matrix.m_matrix[0][0] = cy*cz;
	matrix.m_matrix[1][0] = (sx*sy*cz)-(cx*sz);
	matrix.m_matrix[2][0] = (cx*sy*cz)+(sx*sz);
	matrix.m_matrix[3][0] = 0;

	matrix.m_matrix[0][1] = cy*sz;
	matrix.m_matrix[1][1] = (sx*sy*sz)+(cx*cz);
	matrix.m_matrix[2][1] = (cx*sy*sz)-(sx*cz);
	matrix.m_matrix[3][1] = 0;

	matrix.m_matrix[0][2] = -sy;
	matrix.m_matrix[1][2] = sx*cy;
	matrix.m_matrix[2][2] = cx*cy;
	matrix.m_matrix[3][2] = 0;

	matrix.m_matrix[0][3] = 0;
	matrix.m_matrix[1][3] = 0;
	matrix.m_matrix[2][3] = 0;
	matrix.m_matrix[3][3] = 1;

	*this *= matrix;	
}

void MercuryMatrix::RotateAngAxis( float fAngle, float ix, float iy, float iz )
{
	float c = COS( fAngle*Q_PI/180 );
	float s = SIN( fAngle*Q_PI/180 );
	float absin = SQRT( ix*ix + iy*iy + iz*iz );
	float x = ix/absin;
	float y = iy/absin;
	float z = iz/absin;

	m_matrix[0][0] = x*x*(1-c)+c;
	m_matrix[0][1] = x*y*(1-c)-z*s;
	m_matrix[0][2] = x*z*(1-c)+y*s;
	m_matrix[0][3] = 0;

	m_matrix[1][0] = y*x*(1-c)+z*s;
	m_matrix[1][1] = y*y*(1-c)+c;
	m_matrix[1][2] = y*z*(1-c)-x*s;
	m_matrix[1][3] = 0;

	m_matrix[2][0] = x*z*(1-c)-y*s;
	m_matrix[2][1] = y*z*(1-c)+x*s;
	m_matrix[2][2] = z*z*(1-c)+c;
	m_matrix[2][3] = 0;

	m_matrix[3][0] = 0;
	m_matrix[3][1] = 0;
	m_matrix[3][2] = 0;
	m_matrix[3][3] = 1;
}

void MercuryMatrix::Transotale( float tX, float tY, float tZ, float rX, float rY, float rZ, float sX, float sY, float sZ )
{
	//x,y,z must be negated for some reason
	float X = -rX*DEGRAD; //Reduced calulation for speed
	float Y = -rY*DEGRAD;
	float Z = -rZ*DEGRAD;
	float cx = COS(X);
	float sx = SIN(X);
	float cy = COS(Y);
	float sy = SIN(Y);
	float cz = COS(Z);
	float sz = SIN(Z);

	MercuryMatrix matrix;

	//Row major
	//manually transposed
	matrix.m_matrix[0][0] = sX*cy*cz;
	matrix.m_matrix[1][0] = sX*((sx*sy*cz)-(cx*sz));
	matrix.m_matrix[2][0] = sX*((cx*sy*cz)+(sx*sz));
	matrix.m_matrix[3][0] = 0;

	matrix.m_matrix[0][1] = sY*cy*sz;
	matrix.m_matrix[1][1] = sY*((sx*sy*sz)+(cx*cz));
	matrix.m_matrix[2][1] = sY*((cx*sy*sz)-(sx*cz));
	matrix.m_matrix[3][1] = 0;

	matrix.m_matrix[0][2] = sZ*(-sy);
	matrix.m_matrix[1][2] = sZ*sx*cy;
	matrix.m_matrix[2][2] = sZ*cx*cy;
	matrix.m_matrix[3][2] = 0;

	matrix.m_matrix[0][3] = tX;
	matrix.m_matrix[1][3] = tY;
	matrix.m_matrix[2][3] = tZ;
	matrix.m_matrix[3][3] = 1;

	*this *= matrix;	
}


void MercuryMatrix::Scale(float x, float y, float z)
{
	MercuryMatrix m;

	m.m_matrix[0][0] = x;
	m.m_matrix[1][1] = y;
	m.m_matrix[2][2] = z;

	*this *= m;
}

MercuryMatrix MercuryMatrix::operator*(const MercuryMatrix& m) const
{
	MercuryMatrix r(*this);
	R_ConcatTransforms4 ( (float*)&m_matrix, (float*)&m.m_matrix, (float*)&r.m_matrix);
	return r;
}

MercuryMatrix& MercuryMatrix::operator*=(const MercuryMatrix& m) 
{
	MercuryMatrix r(*this);
	R_ConcatTransforms4 ( (float*)&r.m_matrix, (float*)&m.m_matrix, (float*)&m_matrix);
	return *this;
}

void TransposeMatrix( const MercuryMatrix &in, MercuryMatrix &out )
{
	float tmp;
	const float* _in = (const float*)in.Ptr();
	float* _out = out.Ptr();

	//unchanging
	_out[0] = _in[0];
	_out[5] = _in[5];
	_out[10] = _in[10];
	_out[15] = _in[15];

	tmp = _in[1];
	_out[1] = _in[4];
	_out[4] = tmp;
	tmp = _in[2];
	_out[2] = _in[8];
	_out[8] = tmp;
	tmp = _in[3];
	_out[3] = _in[12];
	_out[12] = tmp;

	tmp = _in[6];
	_out[6] = _in[9];
	_out[9] = tmp;
	tmp = _in[7];
	_out[7] = _in[13];
	_out[13] = tmp;

	tmp = _in[11];
	_out[11] = _in[14];
	_out[14] = tmp;
}

/* 
 * Copyright (c) 2006 Joshua Allen
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

