#include <MQuaternion.h>
#include <MercuryMath.h>

MQuaternion::MQuaternion(float W, float X, float Y, float Z)
{
	m_xyzw[0] = X;
	m_xyzw[1] = Y;
	m_xyzw[2] = Z;
	m_xyzw[3] = W;
}

MQuaternion::MQuaternion(float* xyzw)
{
	for (uint8_t i = 0; i < 4; ++i)
		m_xyzw[i] = xyzw[i];
}

MQuaternion::MQuaternion(const MercuryVertex& p)
{
	m_xyzw[0] = p.GetX();
	m_xyzw[1] = p.GetY();
	m_xyzw[2] = p.GetZ();
	m_xyzw[3] = 0;
}

float & MQuaternion::operator [] (int i)
{
	return m_xyzw[i];
}

void MQuaternion::SetEuler(const MercuryVector& angles)
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
	m_xyzw[3] = cx*cy*cz+sx*sy*sz;//q1
	m_xyzw[0] = sx*cy*cz-cx*sy*sz;//q2
	m_xyzw[1] = cx*sy*cz+sx*cy*sz;//q3
	m_xyzw[2] = cx*cy*sz-sx*sy*cz;//q4
}

void MQuaternion::CreateFromAxisAngle(const MercuryVector& p, const float radians)
{
	float sn = SIN(radians/2.0f);
	m_xyzw[3] = COS(radians/2.0f);
	m_xyzw[0] = sn * p.m_xyzw[0];
	m_xyzw[1] = sn * p.m_xyzw[1];
	m_xyzw[2] = sn * p.m_xyzw[2];
}

//Returns the magnitude
float MQuaternion::magnitude() const {
	return SQRT((m_xyzw[3]*m_xyzw[3])+(m_xyzw[0]*m_xyzw[0])+(m_xyzw[1]*m_xyzw[1])+(m_xyzw[2]*m_xyzw[2]));
}

//Returns the normalized MQuaternion
MQuaternion MQuaternion::normalize() const {
	return (*this)/magnitude();
}

//Returns the conjugate MQuaternion
MQuaternion MQuaternion::conjugate() const {
	MQuaternion c(m_xyzw[3],-m_xyzw[0],-m_xyzw[1],-m_xyzw[2]);
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
	float X = 2*m_xyzw[0]*m_xyzw[0]; //Reduced calulation for speed
	float Y = 2*m_xyzw[1]*m_xyzw[1];
	float Z = 2*m_xyzw[2]*m_xyzw[2];
	float a = 2*m_xyzw[3]*m_xyzw[0];
	float b = 2*m_xyzw[3]*m_xyzw[1];
	float c = 2*m_xyzw[3]*m_xyzw[2];
	float d = 2*m_xyzw[0]*m_xyzw[1];
	float e = 2*m_xyzw[0]*m_xyzw[2];
	float f = 2*m_xyzw[1]*m_xyzw[2];

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
	result.m_xyzw[3] = m_xyzw[3] + rhs.m_xyzw[3];
	result.m_xyzw[0] = m_xyzw[0] + rhs.m_xyzw[0];
	result.m_xyzw[1] = m_xyzw[1] + rhs.m_xyzw[1];
	result.m_xyzw[2] = m_xyzw[2] + rhs.m_xyzw[2];
	return result;
}

MQuaternion MQuaternion::operator - (const MQuaternion &rhs) const
{
	MQuaternion result;
	result.m_xyzw[3] = m_xyzw[3] - rhs.m_xyzw[3];
	result.m_xyzw[0] = m_xyzw[0] - rhs.m_xyzw[0];
	result.m_xyzw[1] = m_xyzw[1] - rhs.m_xyzw[1];
	result.m_xyzw[2] = m_xyzw[2] - rhs.m_xyzw[2];
	return result;
}

MQuaternion MQuaternion::operator * (const MQuaternion &rhs) const 
{
	MQuaternion result;
	result.m_xyzw[3] = (m_xyzw[3]*rhs.m_xyzw[3])-(m_xyzw[0]*rhs.m_xyzw[0])-(m_xyzw[1]*rhs.m_xyzw[1])-(m_xyzw[2]*rhs.m_xyzw[2]);
	result.m_xyzw[0] = (m_xyzw[3]*rhs.m_xyzw[0])+(m_xyzw[0]*rhs.m_xyzw[3])+(m_xyzw[1]*rhs.m_xyzw[2])-(m_xyzw[2]*rhs.m_xyzw[1]);
	result.m_xyzw[1] = (m_xyzw[3]*rhs.m_xyzw[1])-(m_xyzw[0]*rhs.m_xyzw[2])+(m_xyzw[1]*rhs.m_xyzw[3])+(m_xyzw[2]*rhs.m_xyzw[0]);
	result.m_xyzw[2] = (m_xyzw[3]*rhs.m_xyzw[2])+(m_xyzw[0]*rhs.m_xyzw[1])-(m_xyzw[1]*rhs.m_xyzw[0])+(m_xyzw[2]*rhs.m_xyzw[3]);
	return result;
}

MQuaternion& MQuaternion::operator = (const MQuaternion &rhs) 
{
	m_xyzw[3] = rhs.m_xyzw[3];
	m_xyzw[0] = rhs.m_xyzw[0];
	m_xyzw[1] = rhs.m_xyzw[1];
	m_xyzw[2] = rhs.m_xyzw[2];
	return (*this);
}

MQuaternion& MQuaternion::operator += (const MQuaternion &rhs) {
	m_xyzw[3] += rhs.m_xyzw[3];
	m_xyzw[0] += rhs.m_xyzw[0];
	m_xyzw[1] += rhs.m_xyzw[1];
	m_xyzw[2] += rhs.m_xyzw[2];
	return (*this);
}

MQuaternion& MQuaternion::operator -= (const MQuaternion &rhs) {
	m_xyzw[3] -= rhs.m_xyzw[3];
	m_xyzw[0] -= rhs.m_xyzw[0];
	m_xyzw[1] -= rhs.m_xyzw[1];
	m_xyzw[2] -= rhs.m_xyzw[2];
	return (*this);
}

MQuaternion& MQuaternion::operator *= (const MQuaternion &rhs) {
	m_xyzw[3] = (m_xyzw[3]*rhs.m_xyzw[3])-(m_xyzw[0]*rhs.m_xyzw[0])-(m_xyzw[1]*rhs.m_xyzw[1])-(m_xyzw[2]*rhs.m_xyzw[2]);
	m_xyzw[0] = (m_xyzw[3]*rhs.m_xyzw[0])+(m_xyzw[0]*rhs.m_xyzw[3])+(m_xyzw[1]*rhs.m_xyzw[2])-(m_xyzw[2]*rhs.m_xyzw[1]);
	m_xyzw[1] = (m_xyzw[3]*rhs.m_xyzw[1])-(m_xyzw[0]*rhs.m_xyzw[2])+(m_xyzw[1]*rhs.m_xyzw[3])+(m_xyzw[2]*rhs.m_xyzw[0]);
	m_xyzw[2] = (m_xyzw[3]*rhs.m_xyzw[2])+(m_xyzw[0]*rhs.m_xyzw[1])-(m_xyzw[1]*rhs.m_xyzw[0])+(m_xyzw[2]*rhs.m_xyzw[3]);
	return (*this);
}

MQuaternion MQuaternion::operator * (const float &rhs) const {
	MQuaternion result;
	result.m_xyzw[3] = m_xyzw[3]*rhs;
	result.m_xyzw[0] = m_xyzw[0]*rhs;
	result.m_xyzw[1] = m_xyzw[1]*rhs;
	result.m_xyzw[2] = m_xyzw[2]*rhs;
	return result;
}

MQuaternion MQuaternion::operator / (const float &rhs) const {
	MQuaternion result;
	result.m_xyzw[3] = m_xyzw[3]/rhs;
	result.m_xyzw[0] = m_xyzw[0]/rhs;
	result.m_xyzw[1] = m_xyzw[1]/rhs;
	result.m_xyzw[2] = m_xyzw[2]/rhs;
	return result;
}

MQuaternion& MQuaternion::operator *= (const float &rhs) {
	m_xyzw[3] *= rhs;
	m_xyzw[0] *= rhs;
	m_xyzw[1] *= rhs;
	m_xyzw[2] *= rhs;
	return (*this);
}

MQuaternion& MQuaternion::operator /= (const float &rhs) {
	m_xyzw[3] /= rhs;
	m_xyzw[0] /= rhs;
	m_xyzw[1] /= rhs;
	m_xyzw[2] /= rhs;
	return (*this);
}

//Returns the Euclidian Inner Product of two MQuaternions (Similar to Vector Dot-Product)
float innerProduct(const MQuaternion & a, const MQuaternion &b) 
{
	return (a.m_xyzw[3]*b.m_xyzw[3])+(a.m_xyzw[0]*b.m_xyzw[0])+(a.m_xyzw[1]*b.m_xyzw[1])+(a.m_xyzw[2]*b.m_xyzw[2]);
}

//Returns the Euclidian Outer Product of two MQuaternions
MercuryVector outerProduct(MQuaternion a,MQuaternion b) 
{
	MercuryVector result;
	result.m_xyzw[0] = (a.m_xyzw[3]*b.m_xyzw[0])-(a.m_xyzw[0]*b.m_xyzw[3])-(a.m_xyzw[1]*b.m_xyzw[2])+(a.m_xyzw[2]*b.m_xyzw[1]);
	result.m_xyzw[1] = (a.m_xyzw[3]*b.m_xyzw[1])+(a.m_xyzw[0]*b.m_xyzw[2])-(a.m_xyzw[1]*b.m_xyzw[3])-(a.m_xyzw[2]*b.m_xyzw[0]);
	result.m_xyzw[2] = (a.m_xyzw[3]*b.m_xyzw[2])-(a.m_xyzw[0]*b.m_xyzw[1])+(a.m_xyzw[1]*b.m_xyzw[0])-(a.m_xyzw[2]*b.m_xyzw[3]);
	return result;
}

//Returns the Even Product of two MQuaternions
MQuaternion evenProduct(MQuaternion a,MQuaternion b) {
	MQuaternion result;
	result.m_xyzw[3] = (a.m_xyzw[3]*b.m_xyzw[3])-(a.m_xyzw[0]*b.m_xyzw[0])-(a.m_xyzw[1]*b.m_xyzw[1])-(a.m_xyzw[2]*b.m_xyzw[2]);
	result.m_xyzw[0] = (a.m_xyzw[3]*b.m_xyzw[0])+(a.m_xyzw[0]*b.m_xyzw[3]);
	result.m_xyzw[1] = (a.m_xyzw[3]*b.m_xyzw[1])+(a.m_xyzw[1]*b.m_xyzw[3]);
	result.m_xyzw[2] = (a.m_xyzw[3]*b.m_xyzw[2])+(a.m_xyzw[2]*b.m_xyzw[3]);
	return result;
}

//Returns the Odd Product of two MQuaternions (Similar to Vector Cross-Product)
MercuryVector oddProduct(MQuaternion a,MQuaternion b) {
	MercuryVector result;
	result.m_xyzw[0] = (a.m_xyzw[1]*b.m_xyzw[2])-(a.m_xyzw[2]*b.m_xyzw[1]);
	result.m_xyzw[1] = (a.m_xyzw[2]*b.m_xyzw[0])-(a.m_xyzw[0]*b.m_xyzw[2]);
	result.m_xyzw[2] = (a.m_xyzw[0]*b.m_xyzw[1])-(a.m_xyzw[1]*b.m_xyzw[0]);
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

bool MQuaternion::operator==(const MQuaternion& p) const
{
	for (unsigned int i = 0; i < 4; ++i)
		if (m_xyzw[i] != p.m_xyzw[i]) return false;
	return true;
}

void AngleMatrix (const MercuryVector & angles, MercuryMatrix & matrix )
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
