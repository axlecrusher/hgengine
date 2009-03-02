#include <MercuryVertex.h>
#include <MercuryUtil.h>
#include <MercuryMath.h>

MercuryVertex::MercuryVertex()
{
	m_xyz[0] = m_xyz[1] = m_xyz[2] = 0;
}

MercuryVertex::MercuryVertex( float ix, float iy, float iz )
{
	m_xyz[0] = ix;
	m_xyz[1] = iy;
	m_xyz[2] = iz;
}

MercuryVertex::MercuryVertex( const float * in )
{
	for (unsigned int i = 0; i < 3; ++i)
		m_xyz[i] = in[i];
}

MercuryVertex::MercuryVertex( const MercuryVertex& v)
{
	for (unsigned int i = 0; i < 3; ++i)
		m_xyz[i] = v.m_xyz[i];
}

void MercuryVertex::NormalizeSelf()
{
	float imag = 1.0f/Length();
	for (unsigned int i = 0; i < 3; ++i)
		m_xyz[i] *= imag;
}
		
MercuryVertex MercuryVertex::Normalize() const
{
	MercuryVertex r(*this);
	r.NormalizeSelf();
	return r;
}

float MercuryVertex::Length() const
{
	float length = m_xyz[0]*m_xyz[0];
	length += m_xyz[1]*m_xyz[1];
	length += m_xyz[2]*m_xyz[2];
	return SQRT(length);
}

float MercuryVertex::GetBiggestElement() const
{
	float tmp = m_xyz[0];
	tmp = max<float>(tmp, m_xyz[1]);
	return max<float>(tmp, m_xyz[2]);
}

const MercuryVertex& MercuryVertex::operator *= (const MercuryVertex& p)
{
	for (unsigned int i = 0; i < 3; ++i)
		m_xyz[i] *= p.m_xyz[i];
	return *this;
}
		
const MercuryVertex& MercuryVertex::operator /= (const MercuryVertex& p)
{
	for (unsigned int i = 0; i < 3; ++i)
		m_xyz[i] /= p.m_xyz[i];
	return *this;
}

bool MercuryVertex::operator==(const MercuryVertex& p) const
{
	for (unsigned int i = 0; i < 3; ++i)
		if (m_xyz[i] != p.m_xyz[i]) return false;
	return true;
}

bool MercuryVertex::operator==(const float f) const
{
	for (unsigned int i = 0; i < 3; ++i)
		if (m_xyz[i] != f) return false;
	return true;
}

MercuryVertex MercuryVertex::CrossProduct(const MercuryVertex& p) const
{
	MercuryVertex ret;
	ret.m_xyz[0] = m_xyz[1]*p.m_xyz[2] - m_xyz[2]*p.m_xyz[1];
	ret.m_xyz[1] = m_xyz[2]*p.m_xyz[0] - m_xyz[0]*p.m_xyz[2];
	ret.m_xyz[2] = m_xyz[0]*p.m_xyz[1] - m_xyz[1]*p.m_xyz[0];
	return ret;
}

float MercuryVertex::DotProduct(const MercuryVertex& rhs) const
{
	return (m_xyz[0]*rhs.m_xyz[0]+m_xyz[1]*rhs.m_xyz[1]+m_xyz[2]*rhs.m_xyz[2]);
}

void MercuryVertex::Print() const
{
	printf("%f %f %f\n", m_xyz[0], m_xyz[1], m_xyz[2]);
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
