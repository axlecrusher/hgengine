#include <MercuryVertex.h>
#include <MercuryUtil.h>
#include <MercuryMath.h>
#include <MQuaternion.h>

MercuryVertex::MercuryVertex()
{
	(*this)[0] = (*this)[1] = (*this)[2] = (*this)[3] = 0;
}

MercuryVertex::MercuryVertex( float ix, float iy, float iz, float iw )
{
	(*this)[0] = ix;
	(*this)[1] = iy;
	(*this)[2] = iz;
	(*this)[3] = iw;
}

MercuryVertex::MercuryVertex( const float * in )
{
	for (unsigned int i = 0; i < 4; ++i)
		(*this)[i] = in[i];
}

MercuryVertex::MercuryVertex( const MercuryVertex& v)
{
	for (unsigned int i = 0; i < 4; ++i)
		(*this)[i] = v[i];
}

void MercuryVertex::NormalizeSelf()
{
	float imag = 1.0f/Length();
	for (unsigned int i = 0; i < 4; ++i)
		(*this)[i] *= imag;
}
		
MercuryVertex MercuryVertex::Normalize() const
{
	MercuryVertex r(*this);
	r.NormalizeSelf();
	return r;
}

float MercuryVertex::Length() const
{
	float length = (*this)[0]*(*this)[0];
	length += (*this)[1]*(*this)[1];
	length += (*this)[2]*(*this)[2];
	length += (*this)[3]*(*this)[3];
	return SQRT(length);
}

float MercuryVertex::GetBiggestElement() const
{
	float tmp = (*this)[0];
	tmp = MAX<float>(tmp, (*this)[1]);
	tmp = MAX<float>(tmp, (*this)[2]);
	return MAX<float>(tmp, (*this)[3]);
}

const MercuryVertex& MercuryVertex::operator *= (const MercuryVertex& p)
{
	for (unsigned int i = 0; i < 4; ++i)
		(*this)[i] *= p[i];
	return *this;
}
		
const MercuryVertex& MercuryVertex::operator /= (const MercuryVertex& p)
{
	for (unsigned int i = 0; i < 4; ++i)
		(*this)[i] /= p[i];
	return *this;
}

bool MercuryVertex::operator==(const MercuryVertex& p) const
{
	for (unsigned int i = 0; i < 4; ++i)
		if ((*this)[i] != p[i]) return false;
	return true;
}

bool MercuryVertex::operator==(const float f) const
{
	for (unsigned int i = 0; i < 4; ++i)
		if ((*this)[i] != f) return false;
	return true;
}

MercuryVertex MercuryVertex::CrossProduct(const MercuryVertex& p) const
{
	//XXX should this use all 4 components?
	MercuryVertex r;
	MMCrossProduct( m_xyzw, p.m_xyzw, r.m_xyzw );
	return r;
}

float MercuryVertex::DotProduct(const MercuryVertex& rhs) const
{
	//XXX should this use all 4 components?
	return ((*this)[0]*rhs[0]+(*this)[1]*rhs[1]+(*this)[2]*rhs[2]);
}

MercuryVertex MercuryVertex::DotProduct3(const MercuryVertex& rhs1, const MercuryVertex& rhs2, const MercuryVertex& rhs3) const
{
	//XXX should this use all 4 components?
	MercuryVertex dp;
	dp[0] = DotProduct(rhs1);
	dp[1] = DotProduct(rhs2);
	dp[2] = DotProduct(rhs3);
	return dp;
}

void MercuryVertex::Print(const MString& s) const
{
	printf("%s: %f %f %f %f\n", s.c_str(), (*this)[0], (*this)[1], (*this)[2], (*this)[3]);
}

MercuryVertex MercuryVertex::Rotate(const MQuaternion& q) const
{
	return (q * MQuaternion(0, *this) * q.reciprocal()).ToVector();
}

MercuryVertex MercuryVertex::CreateFromString(const MString& s)
{
	float x,y,z;
	sscanf(s.c_str(), "%f,%f,%f", &x, &y, &z);
	return MercuryVertex(x,y,z);
}


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
