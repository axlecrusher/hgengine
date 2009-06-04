#include <MQuaternion.h>
#include <MercuryMath.h>
#include <MercuryMatrix.h>

MQuaternion::MQuaternion()
{
	m_wxyz[0] = 0;
	m_wxyz[1] = 0;
	m_wxyz[2] = 0;
	m_wxyz[3] = 0;
}

MQuaternion::MQuaternion(float W, float X, float Y, float Z)
{
	m_wxyz[0] = W;
	m_wxyz[1] = X;
	m_wxyz[2] = Y;
	m_wxyz[3] = Z;
}

MQuaternion::MQuaternion(float W, const MercuryVertex& p)
{
	m_wxyz[0] = W;
	m_wxyz[1] = p[0];
	m_wxyz[2] = p[1];
	m_wxyz[3] = p[2];
}

float & MQuaternion::operator [] (WXYZ i)
{
	return m_wxyz[i];	//haha we won't even get here.
}

void MQuaternion::SetEuler(const MercuryVertex& angles)
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
	m_wxyz[0] = cx*cy*cz+sx*sy*sz;//q1
	m_wxyz[1] = sx*cy*cz-cx*sy*sz;//q2
	m_wxyz[2] = cx*sy*cz+sx*cy*sz;//q3
	m_wxyz[3] = cx*cy*sz-sx*sy*cz;//q4
}

MQuaternion MQuaternion::CreateFromAxisAngle(const MercuryVertex& p, const float radians)
{
	MQuaternion q;
	q.FromAxisAngle(p, radians);
	return q;
}

void MQuaternion::FromAxisAngle(const MercuryVertex& p, const float radians)
{
	MercuryVertex v( p.Normalize() );
	
	float sn = SIN(radians/2.0f);
	m_wxyz[0] = COS(radians/2.0f);
	m_wxyz[1] = sn * p[0];
	m_wxyz[2] = sn * p[1];
	m_wxyz[3] = sn * p[2];
	
	*this = this->normalize();
}

void MQuaternion::ToAxisAngle(float& angle, float& x, float& y, float& z) const
{
	float scale = SQRT(x*x + y*y + z*z);
	
	//prevent infinite axis
	if (scale == 0)
	{
		x=y=0; z=1;
		angle = 0;
		return;
	}
	
	angle = 2*acos(m_wxyz[0]);
	x = m_wxyz[1] / scale;
	y = m_wxyz[2] / scale;
	z = m_wxyz[3] / scale;
}

//Returns the magnitude
float MQuaternion::magnitude() const {
	return SQRT((m_wxyz[0]*m_wxyz[0])+(m_wxyz[1]*m_wxyz[1])+(m_wxyz[2]*m_wxyz[2])+(m_wxyz[3]*m_wxyz[3]));
}

//Returns the normalized MQuaternion
MQuaternion MQuaternion::normalize() const {
	return (*this)/magnitude();
}

//Returns the conjugate MQuaternion
MQuaternion MQuaternion::conjugate() const {
	MQuaternion c(m_wxyz[0],-m_wxyz[1],-m_wxyz[2],-m_wxyz[3]);
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
	MQuaternion q( this->normalize() );
	
	//Reduced calulation for speed
	float xx = 2*q.X()*q.X();
	float xy = 2*q.X()*q.Y();
	float xz = 2*q.X()*q.Z();
	float xw = 2*q.X()*q.W();
	
	float yy = 2*q.Y()*q.Y();
	float yz = 2*q.Y()*q.Z();
	float yw = 2*q.Y()*q.W();
	
	float zz = 2*q.Z()*q.Z();
	float zw = 2*q.Z()*q.W();

	//row major
	matrix[0][0] = 1-yy-zz;
	matrix[0][1] = xy-zw;
	matrix[0][2] = xz+yw;

	matrix[1][0] = xy+zw;
	matrix[1][1] = 1-xx-zz;
	matrix[1][2] = yz-xw;

	matrix[2][0] = xz-yw;
	matrix[2][1] = yz+xw;
	matrix[2][2] = 1-xx-yy;
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
	for (uint8_t i = 0; i < 4; ++i)
		result.m_wxyz[i] = m_wxyz[i]+rhs.m_wxyz[i];
	return result;
}

MQuaternion MQuaternion::operator - (const MQuaternion &rhs) const
{
	MQuaternion result;
	for (uint8_t i = 0; i < 4; ++i)
		result.m_wxyz[i] = m_wxyz[i]-rhs.m_wxyz[i];
	return result;
}

MQuaternion MQuaternion::operator * (const MQuaternion &rhs) const 
{
	MQuaternion q1( this->normalize() );
	MQuaternion q2( rhs.normalize() );
	
	MQuaternion result;
	result.W() = (q1.W()*q2.W())-(q1.X()*q2.X())-(q1.Y()*q2.Y())-(q1.Z()*q2.Z());
	result.X() = (q1.W()*q2.X())+(q1.X()*q2.W())+(q1.Y()*q2.Z())-(q1.Z()*q2.Y());
	result.Y() = (q1.W()*q2.Y())-(q1.X()*q2.Z())+(q1.Y()*q2.W())+(q1.Z()*q2.X());
	result.Z() = (q1.W()*q2.Z())+(q1.X()*q2.Y())-(q1.Y()*q2.X())+(q1.Z()*q2.W());
	return result;
}

MQuaternion& MQuaternion::operator = (const MQuaternion &rhs) 
{
	for (uint8_t i = 0; i < 4; ++i)
		m_wxyz[i] = rhs.m_wxyz[i];
	return (*this);
}

MQuaternion& MQuaternion::operator += (const MQuaternion &rhs) {
	for (uint8_t i = 0; i < 4; ++i)
		m_wxyz[i] += rhs.m_wxyz[i];
	return (*this);
}

MQuaternion& MQuaternion::operator -= (const MQuaternion &rhs) {
	for (uint8_t i = 0; i < 4; ++i)
		m_wxyz[i] -= rhs.m_wxyz[i];
	return (*this);
}

MQuaternion& MQuaternion::operator *= (const MQuaternion &rhs) {
	*this = *this * rhs;
	return (*this);
}

MQuaternion MQuaternion::operator * (const float &rhs) const {
	MQuaternion result;
	for (uint8_t i = 0; i < 4; ++i)
		result.m_wxyz[i] = m_wxyz[i]*rhs;
	return result;
}

MQuaternion MQuaternion::operator / (const float &rhs) const {
	MQuaternion result;
	for (uint8_t i = 0; i < 4; ++i)
		result.m_wxyz[i] = m_wxyz[i]/rhs;
	return result;
}

MQuaternion& MQuaternion::operator *= (const float &rhs)
{
	for (uint8_t i = 0; i < 4; ++i)
		m_wxyz[i] *= rhs;
	return (*this);
}

MQuaternion& MQuaternion::operator /= (const float &rhs)
{
	for (uint8_t i = 0; i < 4; ++i)
		m_wxyz[i] /= rhs;
	return (*this);
}

bool MQuaternion::operator==(const MQuaternion &rhs) const
{
	for (uint8_t i = 0; i < 4; ++i)
		if (m_wxyz[i] != rhs.m_wxyz[i]) return false;
	return true;
}

MercuryVertex MQuaternion::ToVector() const
{
	MercuryVertex v(m_wxyz[1], m_wxyz[2], m_wxyz[3], m_wxyz[0]);
	return v;
}

void MQuaternion::Print(const MString& s) const
{
	printf("%s: %f %f %f %f\n", s.c_str(), m_wxyz[0], m_wxyz[1], m_wxyz[2], m_wxyz[3]);
}

//Returns the Euclidian Inner Product of two MQuaternions (Similar to Vector Dot-Product)
float innerProduct(const MQuaternion & a, const MQuaternion &b) 
{
	return (a.m_wxyz[0]*b.m_wxyz[0])+(a.m_wxyz[1]*b.m_wxyz[1])+(a.m_wxyz[2]*b.m_wxyz[2])+(a.m_wxyz[3]*b.m_wxyz[3]);
}

//Returns the Euclidian Outer Product of two MQuaternions
MercuryVertex outerProduct(MQuaternion a,MQuaternion b) 
{
	MercuryVertex result;
	result[0] = (a.m_wxyz[0]*b.m_wxyz[1])-(a.m_wxyz[1]*b.m_wxyz[0])-(a.m_wxyz[2]*b.m_wxyz[3])+(a.m_wxyz[3]*b.m_wxyz[2]);
	result[1] = (a.m_wxyz[0]*b.m_wxyz[2])+(a.m_wxyz[1]*b.m_wxyz[3])-(a.m_wxyz[2]*b.m_wxyz[0])-(a.m_wxyz[3]*b.m_wxyz[1]);
	result[2] = (a.m_wxyz[0]*b.m_wxyz[3])-(a.m_wxyz[1]*b.m_wxyz[2])+(a.m_wxyz[2]*b.m_wxyz[1])-(a.m_wxyz[3]*b.m_wxyz[0]);
	return result;
}

//Returns the Even Product of two MQuaternions
MQuaternion evenProduct(MQuaternion a,MQuaternion b) {
	MQuaternion result;
	result.m_wxyz[0] = (a.m_wxyz[0]*b.m_wxyz[0])-(a.m_wxyz[1]*b.m_wxyz[1])-(a.m_wxyz[2]*b.m_wxyz[2])-(a.m_wxyz[3]*b.m_wxyz[3]);
	result.m_wxyz[1] = (a.m_wxyz[0]*b.m_wxyz[1])+(a.m_wxyz[1]*b.m_wxyz[0]);
	result.m_wxyz[2] = (a.m_wxyz[0]*b.m_wxyz[2])+(a.m_wxyz[2]*b.m_wxyz[0]);
	result.m_wxyz[3] = (a.m_wxyz[0]*b.m_wxyz[3])+(a.m_wxyz[3]*b.m_wxyz[0]);
	return result;
}

//Returns the Odd Product of two MQuaternions (Similar to Vector Cross-Product)
MercuryVertex oddProduct(MQuaternion a,MQuaternion b) {
	MercuryVertex result;
	result[0] = (a.m_wxyz[2]*b.m_wxyz[3])-(a.m_wxyz[3]*b.m_wxyz[2]);
	result[1] = (a.m_wxyz[3]*b.m_wxyz[1])-(a.m_wxyz[1]*b.m_wxyz[3]);
	result[2] = (a.m_wxyz[1]*b.m_wxyz[2])-(a.m_wxyz[2]*b.m_wxyz[1]);
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
