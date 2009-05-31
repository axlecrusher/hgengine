#ifndef MQUATERNION_H
#define MQUATERNION_H

#include <MercuryVertex.h>
#include <MercuryUtil.h>

class MercuryMatrix;

///Mathematical Quaternion (Used for Rotation)
class MQuaternion {
	public:
		enum WXYZ { W = 0, X, Y, Z };

		//Defines a Quaternion such that q = w + xi + yj + zk
		MQuaternion();
		MQuaternion(float W, float X, float Y, float Z);
		MQuaternion(float W, const MercuryVertex& p);
	
		///Make this quaternion represent to a set of euler angles
		void SetEuler(const MercuryVertex& angles);
	
		///Make the quaternion represent a given angle radians around an axis p
		static MQuaternion CreateFromAxisAngle(const MercuryVertex& p, const float radians);
		void FromAxisAngle(const MercuryVertex& p, const float radians);
		void ToAxisAngle(float& angle, float& x, float& y, float& z) const;
	
		///Access a component of the quaternion with the [] operator
		float & operator[] ( const WXYZ rhs );
		const float & operator[] ( const WXYZ rhs ) const;

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
		MercuryVertex ToVector() const;
		
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
		
		bool operator==(const MQuaternion &rhs) const;
		inline bool operator!=(const MQuaternion &rhs) const { return !(*this == rhs); }
		
		void Print(const MString& s = "MQuaternion") const;
		
//	private:
		FloatRow m_wxyz;
} M_ALIGN(32);

///Produce a matrix out of a rotation x, then y then z (how Mercury does it)
void AngleMatrix (const MercuryVector & angles, MercuryMatrix & mat );

#endif

/****************************************************************************
 *   Copyright (C) 2009 by Joshua Allen, Charles Lohr, Adam Lowman          *
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
