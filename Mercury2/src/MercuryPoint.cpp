#include <MercuryPoint.h>
#include <MercuryMath.h>

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

/***************************************************************************
 *   Copyright (C) 2008 by Joshua Allen, Charles Lohr   *
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
