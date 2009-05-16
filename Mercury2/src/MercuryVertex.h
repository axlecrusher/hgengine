#ifndef MERCURYVECTOR_H
#define MERCURYVECTOR_H

#include <MercuryMath.h>

#ifndef WIN32
#define __inline__     __inline__      __attribute__((always_inline))
#else
#define __inline__ inline
#endif

class MercuryVertex
{
	public:
		MercuryVertex();
		MercuryVertex( float ix, float iy, float iz );
		MercuryVertex( const float * in );
		MercuryVertex( const MercuryVertex& v);

		///Direct conversion to float*
		__inline__ operator float* () { return (float*)&m_xyzw; }
		///Direct conversion to const float*
		__inline__ operator const float* () const { return (float*)&m_xyzw; }

		inline FloatRow& ToFloatRow() { return m_xyzw; }
		inline const FloatRow& ToFloatRow() const { return m_xyzw; }

		inline float GetX() const { return (*this)[0]; }
		inline float GetY() const { return (*this)[1]; }
		inline float GetZ() const { return (*this)[2]; }
		inline void SetX(const float ix) { (*this)[0] = ix; }
		inline void SetY(const float iy) { (*this)[1] = iy; }
		inline void SetZ(const float iz) { (*this)[2] = iz; }
		
		inline void Zero() { (*this)[0] = 0; (*this)[1] = 0; (*this)[2] = 0; }

		///Normalize (make |point| = 1)
		void NormalizeSelf();
		///Return a normalized point
		MercuryVertex Normalize() const;
		///Return the length of |this|
		float Length() const;

		float GetBiggestElement() const;

		///Write out to be = to this point
		inline void ConvertToVector3( float* out ) const { out[0] = (*this)[0]; out[1] = (*this)[1]; out[2] = (*this)[2]; }
		///Write out to be = to this point, however the 4th element will be 0
		inline void ConvertToVector4( float* out, float w = 0 ) const { out[0] = (*this)[0]; out[1] = (*this)[1]; out[2] = (*this)[2]; out[3] = w; }
		///Write out to be = - to this point, however the 4th element will be 0
		inline void ConvertToIVector4( float* out, float w = 0 ) const { out[0] = -(*this)[0]; out[1] = -(*this)[1]; out[2] = -(*this)[2]; out[3] = w; }

		const MercuryVertex& operator *= (const MercuryVertex& p);
		const MercuryVertex& operator /= (const MercuryVertex& p);
		inline MercuryVertex operator * (const MercuryVertex& p) const { MercuryVertex r(*this); r*=p; return r; }
		inline MercuryVertex operator / (const MercuryVertex& p) const { MercuryVertex r(*this); r/=p; return r; }

		inline MercuryVertex& operator += ( const MercuryVertex& other )		{ (*this)[0]+=other[0]; (*this)[1]+=other[1]; (*this)[2]+=other[2]; return *this; }
		inline MercuryVertex& operator -= ( const MercuryVertex& other )		{ (*this)[0]-=other[0]; (*this)[1]-=other[1]; (*this)[2]-=other[2]; return *this; }
		inline MercuryVertex& operator *= ( float f ) { (*this)[0]*=f; (*this)[1]*=f; (*this)[2]*=f; return *this; }
		inline MercuryVertex& operator /= ( float f ) { (*this)[0]/=f; (*this)[1]/=f; (*this)[2]/=f; return *this; }

		inline MercuryVertex operator + ( const MercuryVertex& other ) const	{ return MercuryVertex( (*this)[0]+other[0], (*this)[1]+other[1], (*this)[2]+other[2] ); }
		inline MercuryVertex operator - ( const MercuryVertex& other ) const	{ return MercuryVertex( (*this)[0]-other[0], (*this)[1]-other[1], (*this)[2]-other[2] ); }
		inline MercuryVertex operator * ( float f ) const { return MercuryVertex( (*this)[0]*f, (*this)[1]*f, (*this)[2]*f ); }
		inline MercuryVertex operator / ( float f ) const { return MercuryVertex( (*this)[0]/f, (*this)[1]/f, (*this)[2]/f ); }

		bool operator==(const MercuryVertex& p) const;
		inline bool operator!=(const MercuryVertex& p) const { return !(*this == p); }

		bool operator==(const float f) const;
		inline bool operator!=(const float f) const { return !(*this == f); }

		///Obtain the cross product (*this) x p
		MercuryVertex CrossProduct(const MercuryVertex& p) const;
		
		float DotProduct(const MercuryVertex& rhs) const;
		MercuryVertex DotProduct3(const MercuryVertex& rhs1, const MercuryVertex& rhs2, const MercuryVertex& rhs3) const;

		void Print() const;

//		float (*this)[3];
		FloatRow m_xyzw;
};

typedef MercuryVertex MercuryVector;

#endif
/****************************************************************************
 *   Copyright (C) 2009 by Joshua Allen                                     *
 *                                                                          *
 *                                                                          *
 *   All rights reserved.                                                   *
 *                                                                          *
 *   Redistribution and use in source and binary forms, with or without     *
 *   modification, are permitted provided that the following conditions     *
 *   are met:                                                               *
 *     * Redistributions of source code must retain the above copyright     *
 *      notice, this list of conditions and the following disclaimer.       *
 *     * Redistributions in binary form must reproduce the above            *
 *      copyright notice, this list of conditions and the following         *
 *      disclaimer in the documentation and/or other materials provided     *
 *      with the distribution.                                              *
 *     * Neither the name of the Mercury Engine nor the names of its        *
 *      contributors may be used to endorse or promote products derived     *
 *      from this software without specific prior written permission.       *
 *                                                                          *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS    *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT      *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR  *
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT   *
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  *
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT       *
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  *
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  *
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT    *
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 ***************************************************************************/
