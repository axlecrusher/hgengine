#include <BillboardNode.h>
#include <MercuryVertex.h>
#include <Viewport.h>

REGISTER_NODE_TYPE(Billboard);

MercuryMatrix Billboard::ManipulateMatrix(const MercuryMatrix& matrix)
{
	MercuryMatrix m = RenderableNode::ManipulateMatrix( matrix );
	
	//Compute the object's center point (position?) in world space
	MercuryVertex center(0,0,0,1);
	center = matrix * center;

	//vector from object to eye projected on XZ
	MercuryVector objToEye = (EYE - center); objToEye[1] = 0; objToEye.NormalizeSelf();
	
	MercuryVector objLookAt(0,0,1); //origional look vector of object
	objLookAt = matrix * objLookAt; //convert to world space
	objLookAt.NormalizeSelf();
//	objLookAt.Print();
	
//	MercuryVector up = (objLookAt.CrossProduct( objToEye )).Normalize();
//	up = objLookAt;
//	up.Print();
	MercuryVector up(0,0,1);  //we wan't the camera's up
	
	float angleCos = LOOKAT.DotProduct(objToEye);

	if ((angleCos < 0.99990) && (angleCos > -0.9999))
	{
		float f = ACOS(angleCos)*RADDEG;
		MercuryMatrix mtmp;
		mtmp.RotateAngAxis(f, up[0], up[1], up[2]);
		m = m * mtmp;
	}
	
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
