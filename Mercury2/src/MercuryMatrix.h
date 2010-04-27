#ifndef MERCURYMATRIX_H
#define MERCURYMATRIX_H

//This matrix will work identically to float[16]
#include "MercuryMath.h"
#include "MercuryUtil.h"
#include <MercuryVertex.h>
#include <MQuaternion.h>

///General Purpose 4x4 row-major matrix
 VC_ALIGN(16) class MercuryMatrix
{
private:
	///[row][column] (The internal matrix)
//	float m_matrix[4][4];
	FloatRow m_matrix[4];
	
	static MercuryMatrix IdentityMatrix;
public:
	MercuryMatrix();
	inline MercuryMatrix(const MercuryMatrix& m) { *this = m; }
	inline float* operator[](unsigned int i) { return (float*)&m_matrix[i]; }
	///Allow typecasting to float * for use in APIs
	inline const float* operator[](unsigned int i) const { return (float*)&m_matrix[i]; }
	const MercuryMatrix& operator=(const MercuryMatrix& m);
	const MercuryMatrix& operator=(const float* m);
	inline float* Ptr() { return (float*)&m_matrix; }
	inline const float* Ptr() const { return (float*)&m_matrix; }

	MercuryMatrix operator*(const MercuryMatrix& m) const;
	MercuryMatrix& operator*=(const MercuryMatrix& m);
	
	MercuryVector operator*(const MercuryVertex& v) const;

	void Translate(float x, float y, float z);
	inline void Translate(const MercuryVertex& v) { Translate(v[0], v[1], v[2]); }
	
	///Rotate along the 3 primariy axes by given amounts (in deg)
	void RotateXYZ(float x, float y, float z);
	inline void RotateXYZ(const MercuryVertex& v) { RotateXYZ(v[0], v[1], v[2]); }
	
	///Rotate a given amount (fAngle) in degrees around pAxis
	void RotateAngAxis( float fAngle, float x, float y, float z );
	
	void Rotate(const MQuaternion& q);
	
	void Scale(float x, float y, float z);
	inline void Scale(const MercuryVertex& v) { Scale(v[0], v[1], v[2]); }
	
	void Transotale( float tX, float tY, float tZ, float rX, float rY, float rZ, float sX, float sY, float sZ );
	inline void Transpose() { TransposeMatrix( m_matrix ); }

	void Zero();
	static const MercuryMatrix& Identity();
	void LoadIdentity();
	
	void Print() const;
} CC_ALIGN(16);

//namespace MercuryMath
//{
//}


#endif

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

