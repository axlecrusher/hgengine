#include "BillboardNode.h"
#include <MercuryVertex.h>
#include <Viewport.h>

REGISTER_NODE_TYPE(BillboardNode);

BillboardNode::BillboardNode()
	:m_sphere(false)
{
}

MercuryMatrix BillboardNode::ManipulateMatrix(const MercuryMatrix& matrix)
{
	//Compute the object's center point (position?) in world space
	MercuryVertex center(0,0,0,1);
	center = matrix * center;

	MercuryVector objToEye = (EYE - center);
	MercuryVector objToEyeProj( objToEye );
	
	//vector from object to eye projected on XZ
	objToEyeProj[1] = 0; objToEyeProj.NormalizeSelf();
	
	MercuryVector objLookAt(0,0,-1); //origional look vector of object
	
	MercuryVector up = objLookAt.CrossProduct(objToEyeProj);
	float angleCos = objLookAt.DotProduct(objToEyeProj);

	float f = ACOS(angleCos);//*RADDEG;
	if (up[1] < 0) f *= -1;
	
	//needs to be the local axis to rotate around
	MercuryMatrix global(matrix);
	MQuaternion mtmp = MQuaternion::CreateFromAxisAngle(m_billboardAxis, f);
	
	//spherical below
	if ( m_sphere )
	{
		objToEye.NormalizeSelf();
		angleCos = objToEyeProj.DotProduct( objToEye );
		f = ACOS(angleCos);
		if (objToEye[1] < 0) f *= -1;
		if (angleCos < 0.99999) mtmp *= MQuaternion::CreateFromAxisAngle(MercuryVector(1,0,0), f);
	}
		
	global.Rotate( mtmp );
	
	return RenderableNode::ManipulateMatrix( global );
}

void BillboardNode::LoadFromXML(const XMLNode& node)
{
	RenderableNode::LoadFromXML(node);
	
	if ( !node.Attribute("billboardaxis").empty() )
		m_billboardAxis = MercuryVector::CreateFromString( node.Attribute("billboardaxis") );
	
	if ( !node.Attribute("spheremode").empty() )
		m_sphere = node.Attribute("spheremode") == "true"?true:false;
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
