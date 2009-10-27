#include "BillboardNode.h"
#include <MercuryVertex.h>
#include <Viewport.h>

REGISTER_NODE_TYPE(BillboardNode);

BillboardNode::BillboardNode()
	:TransformNode(), m_sphere(false)
{
}

void BillboardNode::Update(float dTime)
{
	TransformNode::Update( dTime );
	
	//Compute the object's center point (position?) in world space
	MercuryVertex center(0,0,0,1);
	center = TransformNode::GetGlobalMatrix() * center;

	MercuryVector objToEye = (EYE - center);
	MercuryVector objToEyeProj( objToEye );
	
	//vector from object to eye projected on XZ
	objToEyeProj[1] = 0; objToEyeProj.NormalizeSelf();
	
	MercuryVector objLookAt(0,0,-1); //origional look vector of object
	
	MercuryVector up = objLookAt.CrossProduct(objToEyeProj);
	float angleCos = objLookAt.DotProduct(objToEyeProj);

	float f = ACOS(angleCos);//*RADDEG;
	if (up[1] < 0) f *= -1;

	bool bFlip = m_billboardAxis.DotProduct( MercuryVector(1,1,1) ) < 0 ;

	//needs to be the local axis to rotate around
	MercuryMatrix global( TransformNode::GetGlobalMatrix() );
	MQuaternion mtmp = MQuaternion::CreateFromAxisAngle(m_billboardAxis, ((bFlip)?(Q_PI-f):f) );
	
	//spherical below
	if ( m_sphere )
	{
		objToEye.NormalizeSelf();
		angleCos = objToEyeProj.DotProduct( objToEye );
		f = ACOS(angleCos);
		if (objToEye[1] < 0) f *= -1;
		if (angleCos < 0.99999) mtmp *= MQuaternion::CreateFromAxisAngle(MercuryVector(1,0,0), (bFlip)?-f:f );
	}
		
	global.Rotate( mtmp );
	
	m_billboardMatrix = global;
	
	//notify children that our global matrix has changed
	SetTaint(true);
}

const MercuryMatrix& BillboardNode::GetGlobalMatrix() const
{
	return m_billboardMatrix;
}

void BillboardNode::SaveToXMLTag( MString & sXMLStream )
{
	TransformNode::SaveToXMLTag( sXMLStream );
	sXMLStream += ssprintf( "billboardaxis=\"%f,%f,%f\" spheremode=\"%d\" ", 
		m_billboardAxis[0], m_billboardAxis[1], m_billboardAxis[2], m_sphere );
}

void BillboardNode::LoadFromXML(const XMLNode& node)
{
	TransformNode::LoadFromXML(node);
	
	if ( !node.Attribute("billboardaxis").empty() )
		m_billboardAxis = MercuryVector::CreateFromString( node.Attribute("billboardaxis") );
	
	if ( !node.Attribute("spheremode").empty() )
		m_sphere = StrToBool( node.Attribute("spheremode") );
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
