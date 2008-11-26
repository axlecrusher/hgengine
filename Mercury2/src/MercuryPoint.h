///A point in space/vector
class MercuryPoint
{
	public:
		MercuryPoint() : x(0), y(0), z(0) { };
		MercuryPoint( float ix, float iy, float iz ) : x(ix), y(iy), z(iz) { };
		MercuryPoint( const float * in ) : x(in[0]), y(in[1]), z(in[2]) { };

	///Direct conversion to float*
		operator float* () { return &x; }
	///Direct conversion to const float*
		operator const float* () const { return &x; }

	///Get X value
		inline const float GetX() const { return x; }
	///Get Y value
		inline const float GetY() const { return y; }
	///Get Z value
		inline const float GetZ() const { return z; }
	///Set X value
		inline bool SetX(const float ix) { if (ix == x) { return false; } x = ix; return true; }
	///Set Y value
		inline bool SetY(const float iy) { if (iy == y) { return false; } y = iy; return true; }
	///Set Z value
		inline bool SetZ(const float iz) { if (iz == z) { return false; } z = iz; return true; }
	///Zero the vector
		inline void Clear() { x = 0; y = 0; z = 0; }
	
	//allow [] to access
		float & operator[] ( const int rhs );
		const float operator[] ( const int rhs ) const;

	///Normalize (make |point| = 1)
		void NormalizeSelf();
	///Return a normalized point
		const MercuryPoint Normalize() const;
	///Return the magnitude of |this|
		float Magnitude() const;

		float GetBiggestElement() const { if( x > y ) return (x>z)?x:z; else return (y>z)?y:z; }

	///Write out to be = to this point
		inline void ConvertToVector3( float* out ) const { out[0] = x; out[1] = y; out[2] = z; }
	///Write out to be = to this point, however the 4th element will be 0
		inline void ConvertToVector4( float* out ) const { out[0] = x; out[1] = y; out[2] = z; out[3] = 0; }
	///Write out to be = - to this point, however the 4th element will be 0
		inline void ConvertToIVector4( float* out ) const { out[0] = -x; out[1] = -y; out[2] = -z; out[3] = 0; }

	///Component-wise multiply
		MercuryPoint operator*(const MercuryPoint& p) const;
	///Component-wise divide
		MercuryPoint operator/(const MercuryPoint& p) const;

		inline MercuryPoint& operator += ( const MercuryPoint& other )		{ x+=other.x; y+=other.y; z+=other.z; return *this; }
		inline MercuryPoint& operator -= ( const MercuryPoint& other )		{ x-=other.x; y-=other.y; z-=other.z; return *this; }
		inline MercuryPoint& operator *= ( float f )						{ x*=f; y*=f; z*=f; return *this; }
		inline MercuryPoint& operator /= ( float f )						{ x/=f; y/=f; z/=f; return *this; }

		inline MercuryPoint operator + ( const MercuryPoint& other ) const	{ return MercuryPoint( x+other.x, y+other.y, z+other.z ); }
		inline MercuryPoint operator - ( const MercuryPoint& other ) const	{ return MercuryPoint( x-other.x, y-other.y, z-other.z ); }
		inline MercuryPoint operator * ( float f ) const					{ return MercuryPoint( x*f, y*f, z*f ); }
		inline MercuryPoint operator / ( float f ) const					{ return MercuryPoint( x/f, y/f, z/f ); }

		friend MercuryPoint operator * ( float f, const MercuryPoint& other )	{ return other*f; }

		bool operator==(const MercuryPoint& p) const;
		inline bool operator!=(const MercuryPoint& p) const { return !(*this == p); }

		bool operator==(const float f) const;
		inline bool operator!=(const float f) const { return !(*this == f); }

	///Obtain the cross product (*this) x p
		MercuryPoint CrossProduct(const MercuryPoint& p) const;

		float x;
		float y;
		float z;
};

/***************************************************************************
 *   Copyright (C) 2008 by Joshua Allen, Charles Lohr *
 *      *
 *                                                                         *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: *
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. *
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. *
 *     * Neither the name of the <ORGANIZATION> nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission. *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR *
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR *
 *   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, *
 *   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,   *
 *   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR    *
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF *
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  *
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS    *
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.          *
 ***************************************************************************/
