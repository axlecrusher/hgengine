#include <MercuryTransform.h>

MercuryTransform::MercuryTransform() : m_bDirty( true )
{
	Scale[0] = Scale[1] = Scale[2] = 1;
}

void MercuryTransform::SetTween( const MercuryTransform & left, const MercuryTransform & right, float fPercent )
{
	Position = left.Position * (1-fPercent) + right.Position * fPercent;
	Scale    = left.Scale 	* (1-fPercent) + right.Scale	* fPercent;
	Rotation = SLERP( left.Rotation, right.Rotation, fPercent );
	Dirty();
}

MercuryMatrix & MercuryTransform::GetMatrix()
{
	if( m_bDirty )
	{
		m_Matrix = MercuryMatrix::Identity();
		m_Matrix.Translate( Position[0], Position[1], Position[2] );
		m_Matrix.Rotate( Rotation );
		m_Matrix.Scale( Scale[0], Scale[1], Scale[2] );
		m_bDirty = 0;
	}
	return m_Matrix;
}


/* 
 * Copyright (c) 2010 Charles Lohr
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

