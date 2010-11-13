#ifndef MERCURYVECTOR_H
#define MERCURYVECTOR_H

#include <MercuryMath.h>
#include <MercuryString.h>

#ifndef WIN32
#define __inline__     __inline__      __attribute__((always_inline))
#else
#define __inline__ inline
#endif

#include <MercuryMemory.h>

class MercuryVertex
{
	public:
		MercuryVertex();
		MercuryVertex( float ix, float iy, float iz, float iw = 0 );
		MercuryVertex( const float* in3f, float f );
		MercuryVertex( const float* in4f );
		MercuryVertex( const MercuryVertex& v);
		MercuryVertex( const MercuryVertex& v, float w);
//		virtual ~MercuryVertex();

		///Direct conversion to float*
		__inline__ operator float* () { return m_xyzw; }
		///Direct conversion to const float*
		__inline__ operator const float* () const { return m_xyzw; }
//		__inline__ float& operator[](unsigned int i) { return m_xyzw[i]; }
//		__inline__ const float& operator[](unsigned int i) const { return m_xyzw[i]; }

		inline FloatRow& ToFloatRow() { return (FloatRow&)*m_xyzw; }
		inline const FloatRow& ToFloatRow() const { return (const FloatRow&)*m_xyzw; }

		inline float GetX() const { return m_xyzw[0]; }
		inline float GetY() const { return m_xyzw[1]; }
		inline float GetZ() const { return m_xyzw[2]; }
		inline float GetW() const { return m_xyzw[3]; }
		inline void SetX(const float ix) { m_xyzw[0] = ix; }
		inline void SetY(const float iy) { m_xyzw[1] = iy; }
		inline void SetZ(const float iz) { m_xyzw[2] = iz; }
		inline void SetW(const float iw) { m_xyzw[3] = iw; }
		
		inline void Zero() { m_xyzw[0] = 0; m_xyzw[1] = 0; m_xyzw[2] = 0; }

		///Normalize (make |point| = 1)
		void NormalizeSelf();
		///Return a normalized point
		MercuryVertex Normalize() const;
		///Return the length of |this|
		float Length() const;

		float GetBiggestElement() const;

		///Write out to be = to this point
		inline void ConvertToVector3( float* out ) const { out[0] = m_xyzw[0]; out[1] = m_xyzw[1]; out[2] = m_xyzw[2]; }
		///Write out to be = to this point, however the 4th element will be 0
		inline void ConvertToVector4( float* out, float w = 0 ) const { out[0] = m_xyzw[0]; out[1] = m_xyzw[1]; out[2] = m_xyzw[2]; out[3] = w; }
		///Write out to be = - to this point, however the 4th element will be 0
		inline void ConvertToIVector4( float* out, float w = 0 ) const { out[0] = -m_xyzw[0]; out[1] = -m_xyzw[1]; out[2] = -m_xyzw[2]; out[3] = w; }

		const MercuryVertex& operator *= (const MercuryVertex& p);
		const MercuryVertex& operator /= (const MercuryVertex& p);
		inline MercuryVertex operator * (const MercuryVertex& p) const { MercuryVertex r(*this); r*=p; return r; }
		inline MercuryVertex operator / (const MercuryVertex& p) const { MercuryVertex r(*this); r/=p; return r; }
		
		inline MercuryVertex& operator += ( const MercuryVertex& other )		{ m_xyzw[0]+=other[0]; m_xyzw[1]+=other[1]; m_xyzw[2]+=other[2]; return *this; }
		inline MercuryVertex& operator -= ( const MercuryVertex& other )		{ m_xyzw[0]-=other[0]; m_xyzw[1]-=other[1]; m_xyzw[2]-=other[2]; return *this; }
		inline MercuryVertex& operator *= ( float f ) { m_xyzw[0]*=f; m_xyzw[1]*=f; m_xyzw[2]*=f; return *this; }
		inline MercuryVertex& operator /= ( float f ) { m_xyzw[0]/=f; m_xyzw[1]/=f; m_xyzw[2]/=f; return *this; }

		inline MercuryVertex operator + ( const MercuryVertex& other ) const	{ return MercuryVertex( m_xyzw[0]+other[0], m_xyzw[1]+other[1], m_xyzw[2]+other[2] ); }
		inline MercuryVertex operator - ( const MercuryVertex& other ) const	{ return MercuryVertex( m_xyzw[0]-other[0], m_xyzw[1]-other[1], m_xyzw[2]-other[2] ); }
		inline MercuryVertex operator * ( float f ) const { return MercuryVertex( m_xyzw[0]*f, m_xyzw[1]*f, m_xyzw[2]*f ); }
		inline MercuryVertex operator / ( float f ) const { return MercuryVertex( m_xyzw[0]/f, m_xyzw[1]/f, m_xyzw[2]/f ); }

		bool operator==(const MercuryVertex& p) const;
		inline bool operator!=(const MercuryVertex& p) const { return !(*this == p); }

		bool operator==(const float f) const;
		inline bool operator!=(const float f) const { return !(*this == f); }

		inline const MercuryVertex& operator=(const MercuryVertex& p) { Copy4f(m_xyzw,p.m_xyzw);return *this; }

		///Obtain the cross product (*this) x p
		MercuryVertex CrossProduct(const MercuryVertex& p) const;
		
		float DotProduct(const MercuryVertex& rhs) const;
		MercuryVertex DotProduct3(const MercuryVertex& rhs1, const MercuryVertex& rhs2, const MercuryVertex& rhs3) const;

		MString Stringify(const MString& s = "Vertex") const;

		float AddComponents() const;
		
		static MercuryVertex CreateFromString(const MString& s);

//		float m_xyzw[3];
	private:
		FloatRow m_xyzw;
//		float* m_xyzw;
//		static MercuryMemory< FloatRow >* m_memory;
//		static float* GetFloatMem();
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
