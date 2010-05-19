#include "MercuryMatrix.h"
#include <MercuryLog.h>

MercuryMatrixMemory::MercuryMatrixMemory()
{
	m_data.Allocate(rows,16);
}

MercuryMatrixMemory& MercuryMatrixMemory::GetInstance()
{
	static MercuryMatrixMemory* mmm = NULL;
	
	if (mmm==NULL)
	{
		mmm = new MercuryMatrixMemory();
		mmm->Init();
	}
	
	return *mmm;
}

void MercuryMatrixMemory::Init()
{
	MSemaphoreLock lock(&m_lock);

	for (unsigned int i = 0; i < rows;i++)
		m_free.push_back( m_data.Buffer()+i );
/*
	//test matrix transpose
	MercuryMatrix test;
	for (int i = 0; i < 16; ++i)
		test.Ptr()[i] = i+1;

	LOG.Write("before transpose\n");
	test.Print();
	test.Transpose();
	LOG.Write("after Transpose\n");
	test.Print();
	*/
}

FloatRow* MercuryMatrixMemory::GetNewMatrix()
{
	MatrixArray* m = (MatrixArray*)0x0;
	MSemaphoreLock lock(&m_lock);
	if ( m_free.begin() != m_free.end() )
	{
		m = m_free.front();
		m_free.pop_front();
	}
	if (m==0x0) ***m=0;
	return (FloatRow*)m;
}

void MercuryMatrixMemory::FreeMatrix(FloatRow* m)
{
	MSemaphoreLock lock(&m_lock);
	m_free.push_back((MatrixArray*)m);
}

MercuryMatrix::MercuryMatrix()
	:m_matrix(0)
{
	m_matrix = MercuryMatrixMemory::GetInstance().GetNewMatrix();
	LoadIdentity();
}

MercuryMatrix::MercuryMatrix(const MercuryMatrix& m)
	:m_matrix(0)
{
	m_matrix = MercuryMatrixMemory::GetInstance().GetNewMatrix();
	Copy16f(m_matrix, m.m_matrix);
}

MercuryMatrix::~MercuryMatrix()
{
	if (m_matrix)
		MercuryMatrixMemory::GetInstance().FreeMatrix( m_matrix );
	m_matrix = NULL;
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
	float *f = (float*)m_matrix;
	for (uint8_t i = 0; i < 16; ++i)
		f[i] = 0;
}

const MercuryMatrix& MercuryMatrix::Identity()
{
	return IdentityMatrix;
/*	static bool bSetIdentity = false;

	if (!bSetIdentity)
	{
		bSetIdentity = true;
#ifdef USE_SSEXXX //XXX broken _mm_load1_ps routines
		MercuryMatrix::IdentityMatrix.m_matrix[0] = _mm_load1_ps( &base_matrix_identity[0] );
		MercuryMatrix::IdentityMatrix.m_matrix[1] = _mm_load1_ps( &base_matrix_identity[4] );
		MercuryMatrix::IdentityMatrix.m_matrix[2] = _mm_load1_ps( &base_matrix_identity[8] );
		MercuryMatrix::IdentityMatrix.m_matrix[3] = _mm_load1_ps( &base_matrix_identity[12] );
#else
		Copy16f(MercuryMatrix::IdentityMatrix.m_matrix[0], base_matrix_identity );
#endif
	}
	
	return IdentityMatrix;
	*/
}

void MercuryMatrix::LoadIdentity()
{
	::LoadIdentity(m_matrix);
}

void MercuryMatrix::Translate(float x, float y, float z)
{
	MercuryMatrix m;

	m[0][3] = x;
	m[1][3] = y;
	m[2][3] = z;

	MatrixMultiply4f(m_matrix,m.m_matrix,m_matrix);
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
	matrix[0][0] = cy*cz;
	matrix[1][0] = (sx*sy*cz)-(cx*sz);
	matrix[2][0] = (cx*sy*cz)+(sx*sz);
	matrix[3][0] = 0;

	matrix[0][1] = cy*sz;
	matrix[1][1] = (sx*sy*sz)+(cx*cz);
	matrix[2][1] = (cx*sy*sz)-(sx*cz);
	matrix[3][1] = 0;

	matrix[0][2] = -sy;
	matrix[1][2] = sx*cy;
	matrix[2][2] = cx*cy;
	matrix[3][2] = 0;

	matrix[0][3] = 0;
	matrix[1][3] = 0;
	matrix[2][3] = 0;
	matrix[3][3] = 1;

	MatrixMultiply4f(m_matrix,matrix.m_matrix,m_matrix);
}

void MercuryMatrix::RotateAngAxis( float fAngle, float ix, float iy, float iz )
{
	float c = COS( fAngle*Q_PI/180 );
	float s = SIN( fAngle*Q_PI/180 );
	float absin = SQRT( ix*ix + iy*iy + iz*iz );
	float x = ix/absin;
	float y = iy/absin;
	float z = iz/absin;

	(*this)[0][0] = x*x*(1-c)+c;
	(*this)[0][1] = x*y*(1-c)-z*s;
	(*this)[0][2] = x*z*(1-c)+y*s;
	(*this)[0][3] = 0;

	(*this)[1][0] = y*x*(1-c)+z*s;
	(*this)[1][1] = y*y*(1-c)+c;
	(*this)[1][2] = y*z*(1-c)-x*s;
	(*this)[1][3] = 0;

	(*this)[2][0] = x*z*(1-c)-y*s;
	(*this)[2][1] = y*z*(1-c)+x*s;
	(*this)[2][2] = z*z*(1-c)+c;
	(*this)[2][3] = 0;

	(*this)[3][0] = 0;
	(*this)[3][1] = 0;
	(*this)[3][2] = 0;
	(*this)[3][3] = 1;
}

void MercuryMatrix::Rotate(const MQuaternion& q)
{
	MercuryMatrix m;
	q.toMatrix4( m );
	MatrixMultiply4f( m_matrix, m.m_matrix, m_matrix );
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
	matrix[0][0] = sX*cy*cz;
	matrix[1][0] = sX*((sx*sy*cz)-(cx*sz));
	matrix[2][0] = sX*((cx*sy*cz)+(sx*sz));
	matrix[3][0] = 0;

	matrix[0][1] = sY*cy*sz;
	matrix[1][1] = sY*((sx*sy*sz)+(cx*cz));
	matrix[2][1] = sY*((cx*sy*sz)-(sx*cz));
	matrix[3][1] = 0;

	matrix[0][2] = sZ*(-sy);
	matrix[1][2] = sZ*sx*cy;
	matrix[2][2] = sZ*cx*cy;
	matrix[3][2] = 0;

	matrix[0][3] = tX;
	matrix[1][3] = tY;
	matrix[2][3] = tZ;
	matrix[3][3] = 1;

	MatrixMultiply4f(m_matrix,matrix.m_matrix,m_matrix);
}


void MercuryMatrix::Scale(float x, float y, float z)
{
	MercuryMatrix m;

	m[0][0] = x;
	m[1][1] = y;
	m[2][2] = z;

	MatrixMultiply4f ( m_matrix, m.m_matrix, m_matrix);
}

MercuryMatrix MercuryMatrix::operator*(const MercuryMatrix& m) const
{
	MercuryMatrix r;
	MatrixMultiply4f ( m_matrix, m.m_matrix, r.m_matrix);
	return r;
}

void MercuryMatrix::Print() const
{
	for (int i = 0; i < 4; ++i)
	{
		LOG.Write( ssprintf( "%f %f %f %f", (*this)[i][0], (*this)[i][1], (*this)[i][2], (*this)[i][3] ) );
	}
	LOG.Write("\n");
}

MercuryVector MercuryMatrix::operator*(const MercuryVector& v) const
{
	MercuryVector r;
	VectorMultiply4f( m_matrix, v.ToFloatRow(), r.ToFloatRow() );
	return r;
}

void MercuryMatrix::Transpose()
{
	//we know we will be operating on this data so try to go get it, 3-4x increase in speed.
	PREFETCH((const char*)m_matrix,_MM_HINT_NTA);
	TransposeMatrix( m_matrix, m_matrix );
}


const MercuryMatrix MercuryMatrix::IdentityMatrix;

/* 
 * Copyright (c) 2006-2009 Joshua Allen
 *                         Charles Lohr
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

