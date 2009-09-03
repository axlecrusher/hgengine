#ifndef _MERCURYTYPES_H
#define _MERCURYTYPES_H

#include "MercuryMath.h"
#include "MercuryMatrix.h"

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

///Vector of all 0's (0,0,0)
extern const MercuryPoint gpZero;
///Vector of all 1's (1,1,1)
extern const MercuryPoint gpOne;



///Rotate (in 2D) the X and Y components of a MercuryPoint
MercuryPoint Rotate2DPoint( float fAngle, MercuryPoint pIn );

///Produce a matrix out of a rotation x, then y then z (how Mercury does it)
void AngleMatrix (const MercuryPoint & angles, MercuryMatrix & mat );
///Produce a translation matrix, by (x,y,z)
void TranslationMatrix( const MercuryPoint & position, MercuryMatrix & mat );
///Concatenate matrices, not using the last row. (faster when applicable, and breaks things that don't expect that last row.)
void R_ConcatTransforms3 (  MercuryMatrix in1,  MercuryMatrix in2, MercuryMatrix & out );

///Legacy vector rotation only functions.  It's similar to vector-matrix multiplication
void VectorIRotate (const MercuryPoint & in1, MercuryMatrix &in2, MercuryPoint & out);
void VectorRotate (const MercuryPoint & in1, const MercuryMatrix &in2, MercuryPoint & out);

///Vector-Matrix multiplication
void VectorMultiply( MercuryMatrix &m, const MercuryPoint &p, MercuryPoint &out );

///Matrix Inverse of a 4x4 matrix.
void InvertMatrix( MercuryMatrix &in, MercuryMatrix & out );

///Mathematical Quaternion (Used for Rotation)
class MQuaternion {
public:
	//Defines a Quaternion such that q = w + xi + yj + zk
	float w,x,y,z;
	MQuaternion() : w(0), x(0), y(0), z(0) { };
	MQuaternion(float W, float X, float Y, float Z) : w(W), x(X), y(Y), z(Z) { };
	MQuaternion(float* wxyz) : w(wxyz[0]), x(wxyz[1]), y(wxyz[2]), z(wxyz[3]) { };
	MQuaternion(const MercuryPoint& p) : w(0), x(p.GetX()), y(p.GetY()), z(p.GetZ()) {};

	///Make this quaternion represent to a set of euler angles
	void SetEuler(const MercuryPoint& angles);

	///Make the quaternion represent a given angle radians around an axis p
	void CreateFromAxisAngle(const MercuryPoint& p, const float radians);

	///Access a component of the quaternion with the [] operator
	float & operator[] ( const int rhs );
	const float & operator[] ( const int rhs ) const;

	///Returns the magnitude
	float magnitude() const;
	///Returns the normalized Quaternion
	MQuaternion normalize() const;
	///Returns the conjugate Quaternion
	MQuaternion conjugate() const;
	///Returns the reciprocal Quaternion
	MQuaternion reciprocal() const;
	///Rotates Quaternion about another Quaternion
	MQuaternion rotateAbout(const MQuaternion &spinAxis) const;
	///Converts Quaternion to 4x4 Matrix(3x3 Spatial)
	void toMatrix( MercuryMatrix & mat ) const;
	///Converts Quaternion to complete 4x4 Matrix
	void toMatrix4( MercuryMatrix & mat ) const;
	///Convert the quaternion to a point.
	MercuryPoint ToPoint() { return MercuryPoint( x,y,z ); }
	/******************************************************
	 * NOTE: Quaternion multipication is not commutative  *
	 *       Therefore the / operator could imply for a/b *
	 *       a*b.reciprocal() or b.reciprocal()*a         *
	 ******************************************************/
	MQuaternion operator + (const MQuaternion &rhs) const;
	MQuaternion operator - (const MQuaternion &rhs) const;
	MQuaternion operator * (const MQuaternion &rhs) const;
	MQuaternion& operator =  (const MQuaternion &rhs);
	MQuaternion& operator += (const MQuaternion &rhs);
	MQuaternion& operator -= (const MQuaternion &rhs);
	MQuaternion& operator *= (const MQuaternion &rhs);
	MQuaternion operator * (const float &rhs) const;
	MQuaternion operator / (const float &rhs) const;
	MQuaternion& operator *= (const float &rhs);
	MQuaternion& operator /= (const float &rhs);
};
//Normally aligned.

///Returns the Euclidian Inner Product of two Quaternions (Similar to Vector Dot-Product)
float innerProduct( const MQuaternion &a, const MQuaternion &b );

///Returns the Euclidian Outer Product of two Quaternions
MercuryPoint outerProduct( const MQuaternion &a, const MQuaternion &b );

///Returns the Even Product of two Quaternions
MQuaternion evenProduct(const MQuaternion &a, const MQuaternion &b );

///Returns the Odd Product of two Quaternions (Similar to Vector Cross-Product)
MercuryPoint oddProduct( const MQuaternion &a, const MQuaternion &b );

///Spherical Linear Interpolation between two Quaternions at t percent completion(0-1)
MQuaternion SLERP( const MQuaternion &a, const MQuaternion &b, float t );

#endif


/* 
 * Copyright (c) 2005-2006, Joshua Allen, Charles Lohr, Adam Lowman
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
