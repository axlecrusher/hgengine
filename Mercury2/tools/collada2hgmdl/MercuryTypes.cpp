#include <cmath>
#include <string.h>
#include "MercuryTypes.h"
#include "MercuryMath.h"
//#include "MercuryUtil.h"

const float MercuryPoint::operator[] ( const int rhs ) const
{
	switch (rhs)
	{
	case 0: return x;
	case 1: return y;
	case 2: return z;
	}
	return x;	//haha we won't even get here.
}

float & MercuryPoint::operator [] ( const int rhs )
{
	switch (rhs)
	{
	case 0: return x;
	case 1: return y;
	case 2: return z;
	}
	return x;	//haha we won't even get here.
}

MercuryPoint MercuryPoint::operator*(const MercuryPoint& p) const
{
	MercuryPoint tmp;
	tmp.x = x * p.x;
	tmp.y = y * p.y; 
	tmp.z = z * p.z;
	return tmp;
}

MercuryPoint MercuryPoint::operator/(const MercuryPoint& p) const
{
	MercuryPoint tmp;
	tmp.x = x / p.x;
	tmp.y = y / p.y; 
	tmp.z = z / p.z;
	return tmp;
}

bool MercuryPoint::operator==(const MercuryPoint& p) const
{
	if ((x == p.x) && (y == p.y) && (z == p.z))
		return true;

	return false;
}

bool MercuryPoint::operator==(const float f) const
{
	if ((x == f) && (y == f) && (z == f))
		return true;

	return false;
}

MercuryPoint MercuryPoint::CrossProduct(const MercuryPoint& p) const
{
	MercuryPoint ret;

	ret[0] = y*p.z - z*p.y;
	ret[1] = z*p.x - x*p.z;
	ret[2] = x*p.y - y*p.x;

	return ret;
}

void MercuryPoint::NormalizeSelf()
{
	float imag = 1.0f/Magnitude();
	x *= imag; y *= imag; z *= imag;
}

const MercuryPoint MercuryPoint::Normalize() const
{
	MercuryPoint t(*this);
	t.NormalizeSelf();
	return t;
}

float MercuryPoint::Magnitude() const
{
	float length = 0;
	length += x*x;
	length += y*y;
	length += z*z;
	return SQRT(length);
}

MercuryPoint Rotate2DPoint( float fAngle, MercuryPoint pIn )
{
	MercuryPoint ret;
	ret.x += SIN(fAngle)*pIn.y + COS(-fAngle)*pIn.x;
	ret.y += COS(fAngle)*pIn.y + SIN(-fAngle)*pIn.x;
	return ret;
}

void AngleMatrix (const MercuryPoint & angles, MercuryMatrix & matrix )
{
	float X = angles[0]*2*Q_PI/360; //Reduced calulation for speed
	float Y = angles[1]*2*Q_PI/360;
	float Z = angles[2]*2*Q_PI/360;
	float cx = COS(X);
	float sx = SIN(X);
	float cy = COS(Y);
	float sy = SIN(Y);
	float cz = COS(Z);
	float sz = SIN(Z);
	  
	//Row major
	matrix[0][0] = cy*cz;
	matrix[0][1] = (sx*sy*cz)-(cx*sz);
	matrix[0][2] = (cx*sy*cz)+(sx*sz);
	matrix[0][3] = 0;

	matrix[1][0] = cy*sz;
	matrix[1][1] = (sx*sy*sz)+(cx*cz);
	matrix[1][2] = (cx*sy*sz)-(sx*cz);
	matrix[1][3] = 0;

	matrix[2][0] = -sy;
	matrix[2][1] = sx*cy;
	matrix[2][2] = cx*cy;
	matrix[2][3] = 0;

	matrix[3][0] = 0;
	matrix[3][1] = 0;
	matrix[3][2] = 0;
	matrix[3][3] = 1;
}

void R_ConcatTransforms3 ( MercuryMatrix in1, MercuryMatrix in2, MercuryMatrix & out)
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
				in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
				in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
				in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
				in1[0][2] * in2[2][3] + in1[0][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
				in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
				in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
				in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
				in1[1][2] * in2[2][3] + in1[1][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
				in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
				in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
				in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
				in1[2][2] * in2[2][3] + in1[2][3];
}

//rewrite
//Get rid of vectors and use Quaternion.rotateAbout(Quaternion)
void VectorRotate (const MercuryPoint &in1,  const MercuryMatrix &in2, MercuryPoint &out) {
	out[0] = DotProduct(in1, in2[0]);
	out[1] = DotProduct(in1, in2[1]);
	out[2] = DotProduct(in1, in2[2]);
}

//Must rewrite
// rotate by the inverse of the matrix
void VectorIRotate (const MercuryPoint &in1,  MercuryMatrix &in2, MercuryPoint & out)
{
	out[0] = in1[0]*in2[0][0] + in1[1]*in2[1][0] + in1[2]*in2[2][0];
	out[1] = in1[0]*in2[0][1] + in1[1]*in2[1][1] + in1[2]*in2[2][1];
	out[2] = in1[0]*in2[0][2] + in1[1]*in2[1][2] + in1[2]*in2[2][2];
}

void TranslationMatrix( const MercuryPoint & position, MercuryMatrix & mat )
{
	mat[0][0] = 1;
	mat[1][0] = 0;
	mat[2][0] = 0;
	mat[3][0] = 0;
	mat[0][1] = 0;
	mat[1][1] = 1;
	mat[2][1] = 0;
	mat[3][1] = 0;
	mat[0][2] = 0;
	mat[1][2] = 0;
	mat[2][2] = 1;
	mat[3][2] = 0;
	mat[0][3] = position.x;
	mat[1][3] = position.y;
	mat[2][3] = position.z;
	mat[3][3] = 1;
}

void VectorMultiply( MercuryMatrix &m, const MercuryPoint &p, MercuryPoint &out )
{
	out[0] = p[0] * m[0][0] + p[1] * m[0][1] + p[2] * m[0][2] + m[0][3];
	out[1] = p[0] * m[1][0] + p[1] * m[1][1] + p[2] * m[1][2] + m[1][3];
	out[2] = p[0] * m[2][0] + p[1] * m[2][1] + p[2] * m[2][2] + m[2][3];
}

/* CODE FOR COMPLETING MATRIX-MATRIX MULTIPLICATION  (Thank god for MATH 221 w/ Dr. Yoon Song @ UMBC ) */
void ScaleSector ( MercuryMatrix &in, MercuryMatrix &out, int iLine, float fAmount )
{
	out[iLine][0] = in[iLine][0] * fAmount;
	out[iLine][1] = in[iLine][1] * fAmount;
	out[iLine][2] = in[iLine][2] * fAmount;
	out[iLine][3] = in[iLine][3] * fAmount;
}

void TranslateSector ( MercuryMatrix &in, MercuryMatrix &out, int iLine, int iLine2, float fAmount )
{
	out[iLine][0] += in[iLine2][0] * fAmount;
	out[iLine][1] += in[iLine2][1] * fAmount;
	out[iLine][2] += in[iLine2][2] * fAmount;
	out[iLine][3] += in[iLine2][3] * fAmount;
}

void InvertMatrix( MercuryMatrix &in, MercuryMatrix &out )
{
	MercuryMatrix tmp;
	memcpy( &tmp[0][0], &in[0][0], sizeof(float) * 16 );

	out[0][1] = out[0][2] = out[0][3] = 0;
	out[1][0] = out[1][2] = out[1][3] = 0;
	out[2][0] = out[2][1] = out[2][3] = 0;
	out[3][0] = out[3][1] = out[3][2] = 0;
	out[0][0] = out[1][1] = out[2][2] = out[3][3] = 1;
	
	for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ )
		{
			//Ok, this is a pretty damn discusting trick.
			//On matricies with one of the pivots having tiny values
			// there's a big issue when it tries to get inverted.
			// in many cases it can produce a matrix off by as much as 1 or 2 hundred
			// the only way around this is to pick another row and add it to the 
			// row with an issue ahead of time, bringing all of the numbers to 
			// usable numbers.
			if( ABS( 1.0f/tmp[i][i] ) > 1000.0f )
			{
				TranslateSector( out, out, i, (i+1)%4, 1 );
				TranslateSector( tmp, tmp, i, (i+1)%4, 1 );
			}

			float fMt;
			if ( i == j )
			{
				fMt = 1/tmp[i][i];
				ScaleSector( tmp, tmp, j, fMt );
				ScaleSector( out, out, j, fMt );
			}
			else
			{
				fMt = -tmp[j][i]/tmp[i][i];
				TranslateSector( tmp, tmp, j, i, fMt );
				TranslateSector( out, out, j, i, fMt );
			}
		}

	tmp.Ptr();
}



















float & MQuaternion::operator [] (int i)
{
	switch (i)
	{
	case 0: return x;
	case 1: return y;
	case 2: return z;
	case 3: return w;
	}
	return x;	//haha we won't even get here.
}

void MQuaternion::SetEuler(const MercuryPoint& angles)
{
	float X = angles[0]/2.0f; //roll
	float Y = angles[1]/2.0f; //pitch
	float Z = angles[2]/2.0f; //yaw

	float cx = COS(X);
	float sx = SIN(X);
	float cy = COS(Y);
	float sy = SIN(Y);
	float cz = COS(Z);
	float sz = SIN(Z);

	//Correct according to
	//http://en.wikipedia.org/wiki/Conversion_between_MQuaternions_and_Euler_angles
	w = cx*cy*cz+sx*sy*sz;//q1
	x = sx*cy*cz-cx*sy*sz;//q2
	y = cx*sy*cz+sx*cy*sz;//q3
	z = cx*cy*sz-sx*sy*cz;//q4
}

void MQuaternion::CreateFromAxisAngle(const MercuryPoint& p, const float radians)
{
	float sn = SIN(radians/2.0f);
	w = COS(radians/2.0f);
	x = sn * p.x;
	y = sn * p.y;
	z = sn * p.z;
}

//Returns the magnitude
float MQuaternion::magnitude() const {
	return SQRT((w*w)+(x*x)+(y*y)+(z*z));
}

//Returns the normalized MQuaternion
MQuaternion MQuaternion::normalize() const {
	return (*this)/magnitude();
}

//Returns the conjugate MQuaternion
MQuaternion MQuaternion::conjugate() const {
	MQuaternion c(w,-x,-y,-z);
	return c;
}

//Returns the reciprocal MQuaternion
MQuaternion MQuaternion::reciprocal() const {
	float m = magnitude();
	return conjugate()/(m*m);
}

//Rotate MQuaternion about another MQuaternion
MQuaternion MQuaternion::rotateAbout(const MQuaternion &spinAxis) const {
	return (*this)*spinAxis;
}

//Converts MQuaternion to 4x4 Matrix(3x3 Spatial)
void MQuaternion::toMatrix( MercuryMatrix &matrix ) const {
	float X = 2*x*x; //Reduced calulation for speed
	float Y = 2*y*y;
	float Z = 2*z*z;
	float a = 2*w*x;
	float b = 2*w*y;
	float c = 2*w*z;
	float d = 2*x*y;
	float e = 2*x*z;
	float f = 2*y*z;

	//row major
	matrix[0][0] = 1-Y-Z;
	matrix[0][1] = d-c;
	matrix[0][2] = e+b;  

	matrix[1][0] = d+c;
	matrix[1][1] = 1-X-Z;
	matrix[1][2] = f-a;

	matrix[2][0] = e-b;
	matrix[2][1] = f+a;
	matrix[2][2] = 1-X-Y;
}

void MQuaternion::toMatrix4( MercuryMatrix &matrix ) const {
	toMatrix( matrix );

	//row major (even though it looks like column)
	matrix[0][3] = 0;
	matrix[1][3] = 0;
	matrix[2][3] = 0;
	matrix[3][3] = 1;

	matrix[3][0] = 0;
	matrix[3][1] = 0;
	matrix[3][2] = 0;
}

MQuaternion MQuaternion::operator + (const MQuaternion &rhs) const
{
	MQuaternion result;
	result.w = w + rhs.w;
	result.x = x + rhs.x;
	result.y = y + rhs.y;
	result.z = z + rhs.z;
	return result;
}

MQuaternion MQuaternion::operator - (const MQuaternion &rhs) const
{
	MQuaternion result;
	result.w = w - rhs.w;
	result.x = x - rhs.x;
	result.y = y - rhs.y;
	result.z = z - rhs.z;
	return result;
}

MQuaternion MQuaternion::operator * (const MQuaternion &rhs) const 
{
	MQuaternion result;
	result.w = (w*rhs.w)-(x*rhs.x)-(y*rhs.y)-(z*rhs.z);
	result.x = (w*rhs.x)+(x*rhs.w)+(y*rhs.z)-(z*rhs.y);
	result.y = (w*rhs.y)-(x*rhs.z)+(y*rhs.w)+(z*rhs.x);
	result.z = (w*rhs.z)+(x*rhs.y)-(y*rhs.x)+(z*rhs.w);
	return result;
}

MQuaternion& MQuaternion::operator = (const MQuaternion &rhs) 
{
	w = rhs.w;
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	return (*this);
}

MQuaternion& MQuaternion::operator += (const MQuaternion &rhs) {
	w += rhs.w;
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return (*this);
}

MQuaternion& MQuaternion::operator -= (const MQuaternion &rhs) {
	w -= rhs.w;
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return (*this);
}

MQuaternion& MQuaternion::operator *= (const MQuaternion &rhs) {
	w = (w*rhs.w)-(x*rhs.x)-(y*rhs.y)-(z*rhs.z);
	x = (w*rhs.x)+(x*rhs.w)+(y*rhs.z)-(z*rhs.y);
	y = (w*rhs.y)-(x*rhs.z)+(y*rhs.w)+(z*rhs.x);
	z = (w*rhs.z)+(x*rhs.y)-(y*rhs.x)+(z*rhs.w);
	return (*this);
}

MQuaternion MQuaternion::operator * (const float &rhs) const {
	MQuaternion result;
	result.w = w*rhs;
	result.x = x*rhs;
	result.y = y*rhs;
	result.z = z*rhs;
	return result;
}

MQuaternion MQuaternion::operator / (const float &rhs) const {
	MQuaternion result;
	result.w = w/rhs;
	result.x = x/rhs;
	result.y = y/rhs;
	result.z = z/rhs;
	return result;
}

MQuaternion& MQuaternion::operator *= (const float &rhs) {
	w *= rhs;
	x *= rhs;
	y *= rhs;
	z *= rhs;
	return (*this);
}

MQuaternion& MQuaternion::operator /= (const float &rhs) {
	w /= rhs;
	x /= rhs;
	y /= rhs;
	z /= rhs;
	return (*this);
}

//Returns the Euclidian Inner Product of two MQuaternions (Similar to Vector Dot-Product)
float innerProduct(const MQuaternion & a, const MQuaternion &b) 
{
	return (a.w*b.w)+(a.x*b.x)+(a.y*b.y)+(a.z*b.z);
}

//Returns the Euclidian Outer Product of two MQuaternions
MercuryPoint outerProduct(MQuaternion a,MQuaternion b) 
{
	MercuryPoint result;
	result.x = (a.w*b.x)-(a.x*b.w)-(a.y*b.z)+(a.z*b.y);
	result.y = (a.w*b.y)+(a.x*b.z)-(a.y*b.w)-(a.z*b.x);
	result.z = (a.w*b.z)-(a.x*b.y)+(a.y*b.x)-(a.z*b.w);
	return result;
}

//Returns the Even Product of two MQuaternions
MQuaternion evenProduct(MQuaternion a,MQuaternion b) {
	MQuaternion result;
	result.w = (a.w*b.w)-(a.x*b.x)-(a.y*b.y)-(a.z*b.z);
	result.x = (a.w*b.x)+(a.x*b.w);
	result.y = (a.w*b.y)+(a.y*b.w);
	result.z = (a.w*b.z)+(a.z*b.w);
	return result;
}

//Returns the Odd Product of two MQuaternions (Similar to Vector Cross-Product)
MercuryPoint oddProduct(MQuaternion a,MQuaternion b) {
	MercuryPoint result;
	result.x = (a.y*b.z)-(a.z*b.y);
	result.y = (a.z*b.x)-(a.x*b.z);
	result.z = (a.x*b.y)-(a.y*b.x);
	return result;
}

//Spherical Linear Interpolation between two MQuaternions at t percent completion(0-1)
MQuaternion SLERP( const MQuaternion &a, const MQuaternion &b,float t) {
    MQuaternion an = a.normalize(), bn = b.normalize();
	float cosTheta = innerProduct(MQuaternion(an),bn);
	float sinTheta;

	//Careful: If cosTheta is exactly one, or even if it's infinitesimally over, it'll
	// cause SQRT to produce not a number, and screw everything up.
	if ( 1 - (cosTheta*cosTheta) <= 0 )
		sinTheta = 0;
	else
		sinTheta = SQRT(1 - (cosTheta*cosTheta));

	float Theta = ACOS(cosTheta); //Theta is half the angle between the 2 MQuaternions

	if(fabs(Theta) < 0.01)
		return a;
	if(fabs(sinTheta) < 0.01)
		return (a+b)/2;
	return ( (a*SIN((1-t)*Theta)) + (b*SIN(t*Theta)) ) / sinTheta;
}

const MercuryPoint gpZero = MercuryPoint( 0,0,0 );
const MercuryPoint gpOne = MercuryPoint( 1,1,1 );



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
