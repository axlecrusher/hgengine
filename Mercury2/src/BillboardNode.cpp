#include <BillboardNode.h>
#include <MercuryVertex.h>
#include <Viewport.h>

REGISTER_NODE_TYPE(Billboard);

MercuryMatrix Billboard::ManipulateMatrix(const MercuryMatrix& matrix)
{
	MercuryMatrix m = matrix;
//	m.Transpose();
	
	//manually transposed
	MercuryVertex center(m[3][0], m[3][1], m[3][2]);
	
	const MercuryVertex& eye = FRUSTUM->GetEye();
	const MercuryVector& lookAt = FRUSTUM->GetLookAt();
		
	MercuryVector v = (eye - center).Normalize();
	MercuryVector up = lookAt.CrossProduct( v );
	float angleCos = lookAt.DotProduct(v);
	
	if ((angleCos < 0.99990) && (angleCos > -0.9999))
	{
		float f = ACOS(angleCos)*RADDEG;
//		printf("billboard!!! %f %f %f\n",up[0]*f, up[1]*f, up[2]*f );

//		MercuryMatrix m;
//		m.RotateXYZ(ACOS(angleCosine)*RADDEG,upAux[0], upAux[1], upAux[2])
//		glRotatef(ACOS(angleCosine)*RADDEG,upAux[0], upAux[1], upAux[2]);
		m.RotateXYZ(up[0]*f, up[1]*f, up[2]*f);
	}
//	m.Transpose();
	return m;
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
