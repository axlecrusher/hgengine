#include <MTriangle.h>

#include <stdio.h>

MTriangle::MTriangle()
{/*
	m_verts[0] = MercuryVertex(0,0,0,0);
	m_verts[1] = MercuryVertex(0,0,0,0);
	m_verts[2] = MercuryVertex(0,0,0,0);
	*/
}

MTriangle::MTriangle(const MTriangle& t)
{
	m_verts[0] = t.m_verts[0];
	m_verts[1] = t.m_verts[1];
	m_verts[2] = t.m_verts[2];
}

MTriangle::MTriangle(const MercuryVertex& a, const MercuryVertex& b, const MercuryVertex& c)
{
	m_verts[0] = a;
	m_verts[1] = b;
	m_verts[2] = c;
}

MercuryVertex MTriangle::Barycentric(const MercuryVertex& p)
{
	MercuryVector v[3];
	v[0] = m_verts[1] - m_verts[0];
	v[1] = m_verts[2] - m_verts[0];
	v[2] = p - m_verts[0];
	
	float dp[5];
	dp[0] = v[0].DotProduct(v[0]);
	dp[1] = v[0].DotProduct(v[1]);
	dp[2] = v[0].DotProduct(v[2]);
	dp[3] = v[1].DotProduct(v[1]);
	dp[4] = v[1].DotProduct(v[2]);

	// Compute barycentric coordinates
	float invDenom = 1 / (dp[0] * dp[3] - dp[1] * dp[1]);
	float ru = (dp[3] * dp[2] - dp[1] * dp[4]) * invDenom;
	float rv = (dp[0] * dp[4] - dp[1] * dp[2]) * invDenom;

	return MercuryVertex(ru, rv, 1-(ru+rv));
}

MercuryVertex MTriangle::InterpolatePosition(const MercuryVertex& barycentric)
{
	MercuryVertex result( m_verts[0] );
		
	MercuryVertex tmp( m_verts[1] - m_verts[0] );
	result += tmp.Normalize()*(barycentric[0] * tmp.Length());
	
	tmp = m_verts[2] - m_verts[0];
	result += tmp.Normalize()*(barycentric[1] * tmp.Length());
	
	result[3] = 1;
	
	return result;
}

bool MTriangle::IsInTriangle(const MercuryVertex& p)
{
	MercuryVertex v = Barycentric(p);
	
	//no edges
//	return (v.GetX() > 0) && (v.GetY() > 0) && (v.GetX() + v.GetY() < 1);

	//includes edges
	return (v.GetX() >= 0) && (v.GetY() >= 0) && (v.GetX() + v.GetY() <= 1);
}

float MTriangle::Area()
{
	MercuryVector v[2];
	v[0] = m_verts[1] - m_verts[0];
	v[1] = m_verts[2] - m_verts[0];
	MercuryVector r( v[0].CrossProduct( v[1] ) );
	return r.Length() * 0.5f;
}

bool MTriangle::operator == (const MTriangle& rhs) const
{
	if (m_verts[0] != rhs.m_verts[0]) return false;
	if (m_verts[1] != rhs.m_verts[1]) return false;
	if (m_verts[2] != rhs.m_verts[2]) return false;
	return true;
}

BoundingBox MTriangle::MakeBoundingBox()
{
	float minX, minY, minZ;
	float maxX, maxY, maxZ;
	minX=minY=minZ = 9999999999999.0f;
	maxX=maxY=maxZ = -9999999999999.0f;
	
	for (uint8_t i = 0; i<3; ++i)
	{
		minX = MIN<float>(minX,m_verts[i].GetX());
		minY = MIN<float>(minY,m_verts[i].GetY());
		minZ = MIN<float>(minZ,m_verts[i].GetZ());
		maxX = MAX<float>(maxX,m_verts[i].GetX());
		maxY = MAX<float>(maxY,m_verts[i].GetY());
		maxZ = MAX<float>(maxZ,m_verts[i].GetZ());
	}
	
	//compute center
	MercuryVertex center( (maxX+minX)/2.0f, (maxY+minY)/2.0f, (maxZ+minZ)/2.0f );

	//extends
	MercuryVertex extend( (maxX-minX)/2.0f, (maxY-minY)/2.0f, (maxZ-minZ)/2.0f );

	return BoundingBox(center, extend);
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
