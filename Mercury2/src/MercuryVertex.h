#ifndef MERCURYVECTOR_H
#define MERCURYVECTOR_H

class MercuryVertex
{
	public:
		MercuryVertex();
		MercuryVertex( float ix, float iy, float iz );
		MercuryVertex( const float * in );
		MercuryVertex( const MercuryVertex& v);

		///Direct conversion to float*
		operator float* () { return m_xyz; }
		///Direct conversion to const float*
		operator const float* () const { return m_xyz; }

		inline const float GetX() const { return m_xyz[0]; }
		inline const float GetY() const { return m_xyz[1]; }
		inline const float GetZ() const { return m_xyz[2]; }
		inline void SetX(const float ix) { m_xyz[0] = ix; }
		inline void SetY(const float iy) { m_xyz[1] = iy; }
		inline void SetZ(const float iz) { m_xyz[2] = iz; }
		
		inline void Zero() { m_xyz[0] = 0; m_xyz[1] = 0; m_xyz[2] = 0; }

		///Normalize (make |point| = 1)
		void NormalizeSelf();
		///Return a normalized point
		MercuryVertex Normalize() const;
		///Return the length of |this|
		float Length() const;

		float GetBiggestElement() const;

		///Write out to be = to this point
		inline void ConvertToVector3( float* out ) const { out[0] = m_xyz[0]; out[1] = m_xyz[1]; out[2] = m_xyz[2]; }
		///Write out to be = to this point, however the 4th element will be 0
		inline void ConvertToVector4( float* out ) const { out[0] = m_xyz[0]; out[1] = m_xyz[1]; out[2] = m_xyz[2]; out[3] = 0; }
		///Write out to be = - to this point, however the 4th element will be 0
		inline void ConvertToIVector4( float* out ) const { out[0] = -m_xyz[0]; out[1] = -m_xyz[1]; out[2] = -m_xyz[2]; out[3] = 0; }

		const MercuryVertex& operator *= (const MercuryVertex& p);
		const MercuryVertex& operator /= (const MercuryVertex& p);
		inline MercuryVertex operator * (const MercuryVertex& p) const { MercuryVertex r(*this); r*=p; return r; }
		inline MercuryVertex operator / (const MercuryVertex& p) const { MercuryVertex r(*this); r/=p; return r; }

		inline MercuryVertex& operator += ( const MercuryVertex& other )		{ m_xyz[0]+=other.m_xyz[0]; m_xyz[1]+=other.m_xyz[1]; m_xyz[2]+=other.m_xyz[2]; return *this; }
		inline MercuryVertex& operator -= ( const MercuryVertex& other )		{ m_xyz[0]-=other.m_xyz[0]; m_xyz[1]-=other.m_xyz[1]; m_xyz[2]-=other.m_xyz[2]; return *this; }
		inline MercuryVertex& operator *= ( float f ) { m_xyz[0]*=f; m_xyz[1]*=f; m_xyz[2]*=f; return *this; }
		inline MercuryVertex& operator /= ( float f ) { m_xyz[0]/=f; m_xyz[1]/=f; m_xyz[2]/=f; return *this; }

		inline MercuryVertex operator + ( const MercuryVertex& other ) const	{ return MercuryVertex( m_xyz[0]+other.m_xyz[0], m_xyz[1]+other.m_xyz[1], m_xyz[2]+other.m_xyz[2] ); }
		inline MercuryVertex operator - ( const MercuryVertex& other ) const	{ return MercuryVertex( m_xyz[0]-other.m_xyz[0], m_xyz[1]-other.m_xyz[1], m_xyz[2]-other.m_xyz[2] ); }
		inline MercuryVertex operator * ( float f ) const { return MercuryVertex( m_xyz[0]*f, m_xyz[1]*f, m_xyz[2]*f ); }
		inline MercuryVertex operator / ( float f ) const { return MercuryVertex( m_xyz[0]/f, m_xyz[1]/f, m_xyz[2]/f ); }

		bool operator==(const MercuryVertex& p) const;
		inline bool operator!=(const MercuryVertex& p) const { return !(*this == p); }

		bool operator==(const float f) const;
		inline bool operator!=(const float f) const { return !(*this == f); }

		///Obtain the cross product (*this) x p
		MercuryVertex CrossProduct(const MercuryVertex& p) const;
		
		float DotProduct(const MercuryVertex& rhs) const;
		void Print() const;

		float m_xyz[3];
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
