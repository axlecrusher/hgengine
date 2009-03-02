#include <MercuryPlane.h>
#include <stdio.h>
#include <MercuryMath.h>

bool MercuryPlane::IsBehindPlane(const MercuryVertex& point) const
{
	return m_normal.DotProduct( point ) < 0;
}

bool MercuryPlane::IsBehindPlane(const BoundingBox& bb) const
{
	const MercuryVertex& center = bb.GetCenter();
	const MercuryVertex& extends = bb.GetExtend();
	
	MercuryVertex A1, A2, A3, tmp;
	
	tmp = center+MercuryVertex( extends.GetX(), 0, 0 );
	A1 = (center-tmp).Normalize();
	tmp = center+MercuryVertex( 0, extends.GetY(), 0 );
	A2 = (center-tmp).Normalize();
	tmp = center+MercuryVertex( 0, 0, extends.GetZ() );
	A3 = (center-tmp).Normalize();
	
	float x = ABS( extends.GetX() * m_normal.DotProduct( A1 ) );
	x += ABS( extends.GetY() * m_normal.DotProduct( A2 ) );
	x += ABS( extends.GetZ() * m_normal.DotProduct( A3 ) );
	
	float d = m_normal.DotProduct( m_center );
	
	return ABS( d ) > x;
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