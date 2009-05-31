#include <TransformNode.h>

REGISTER_NODE_TYPE(TransformNode);
REGISTER_NODE_TYPE(RotatorNode);

TransformNode::TransformNode()
	:m_scale( MercuryVertex(1,1,1) ), m_rotation(1,0,0,0)
{
	SetTaint( true );  //taint because of the scale set above
}

void TransformNode::Update(float dTime)
{
	if (m_tainted) ComputeMatrix();
}

void TransformNode::SetScale( const MercuryVertex& scale )
{
	if (scale != m_scale)
	{
		m_scale = scale;
		SetTaint( true );
	}
}

void TransformNode::SetPosition( const MercuryVertex& position )
{
	if (position != m_position)
	{
		m_position = position;
		SetTaint( true );
	}
}

void TransformNode::SetRotation( const MQuaternion& rotation )
{
	if (rotation != m_rotation)
	{
		m_rotation = rotation;
		SetTaint( true );
	}
}

void TransformNode::SetTaint(bool taint)
{
	m_tainted = taint;
	RippleTaintDown();
}

void TransformNode::ComputeMatrix()
{
	m_tainted = false;
	
	MercuryMatrix local, t;
//	local.Identity();

	local.Translate( m_position );
	local.Rotate( m_rotation );
	local.Scale( m_scale );
//	m_rotation.toMatrix4(t);
//	local.Transotale( m_position[0], m_position[1], m_position[2],
//			  0.0f,0.0f,0.0f,
//			  m_rotation[MQuaternion::X], m_rotation[MQuaternion::Y], m_rotation[MQuaternion::Z],
//     m_scale[0], m_scale[1], m_scale[2] );
//	local = t * local;
	
	m_globalMatrix = GetParentMatrix() * local;
}

const MercuryMatrix& TransformNode::GetParentMatrix() const
{
	const MercuryNode* n = m_parent;
	const TransformNode* tn;
	while (n)
	{
		tn = TransformNode::Cast( n );
		if ( tn ) return tn->GetGlobalMatrix();
		n = n->Parent();
	}
	
	return MercuryMatrix::Identity();
}

void TransformNode::RippleTaintDown()
{
	if (m_tainted == true)
	{
		TransformNode* tn;
		std::list< MercuryNode* >::iterator i;
		
		for (i = m_children.begin(); i != m_children.end(); ++i )
		{
			tn = TransformNode::Cast(*i);
			if ( tn )
				tn->SetTaint( true );
		}
	}
}

void TransformNode::LoadFromXML(const XMLNode& node)
{
	MercuryVertex pos(m_position), scale(m_scale);
	MQuaternion rot(m_rotation);
	
	//only change the values that exist in the XML
	if ( !node.Attribute("rotx").empty() )
		rot *= MQuaternion::CreateFromAxisAngle(MercuryVector(1,0,0), StrToFloat( node.Attribute("rotx") ));

	if ( !node.Attribute("roty").empty() )
		rot *= MQuaternion::CreateFromAxisAngle(MercuryVector(0,1,0), StrToFloat( node.Attribute("roty") ));

	if ( !node.Attribute("rotz").empty() )
		rot *= MQuaternion::CreateFromAxisAngle(MercuryVector(0,0,1), StrToFloat( node.Attribute("rotz") ));
	
	if ( !node.Attribute("scalex").empty() )
		scale.SetX( StrToFloat( node.Attribute("scalex") ) );

	if ( !node.Attribute("scaley").empty() )
		scale.SetY( StrToFloat( node.Attribute("scaley") ) );

	if ( !node.Attribute("scalez").empty() )
		scale.SetZ( StrToFloat( node.Attribute("scalez") ) );
	
	if ( !node.Attribute("movx").empty() )
		pos.SetX( StrToFloat( node.Attribute("movx") ) );

	if ( !node.Attribute("movy").empty() )
		pos.SetY( StrToFloat( node.Attribute("movy") ) );

	if ( !node.Attribute("movz").empty() )
		pos.SetZ( StrToFloat( node.Attribute("movz") ) );
	
	SetRotation(rot);
	SetPosition(pos);
	SetScale(scale);
	
	MercuryNode::LoadFromXML( node );
}

void TransformNode::OnAdded()
{
	//Try to get the taint flag from the parent transform
	MercuryNode* n = m_parent;
	TransformNode* tn;
	while (n)
	{
		tn = TransformNode::Cast( n );
		if ( tn )
		{
			tn->RippleTaintDown();
			return;
		}
		n = n->Parent();
	}	
}

void RotatorNode::Update(float dTime)
{
	MQuaternion r = GetRotation();
	r[MQuaternion::X] += (dTime)*25;
	r[MQuaternion::Y] += (dTime)*75;
	
	SetRotation( r );
	
	TransformNode::Update(dTime);
}

/****************************************************************************
 *   Copyright (C) 2008 by Joshua Allen                                     *
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
