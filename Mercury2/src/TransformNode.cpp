#include <TransformNode.h>
#include <GLHeaders.h>
#include <Shader.h>

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

void TransformNode::RecursivePreRender()
{
	if ( IsHidden() ) return;
	
	const MercuryMatrix& matrix = GetGlobalMatrix();
	m_modelView = ManipulateMatrix( matrix );
	
	GLCALL( glLoadMatrix( m_modelView ) );

	MercuryNode::RecursivePreRender();
}

void TransformNode::HandleMatrixOperations()
{
	if ( IsHidden() ) return;
	
	const MercuryMatrix& matrix = GetGlobalMatrix();
	m_modelView = ManipulateMatrix( matrix );
	
	GLCALL( glLoadMatrix( m_modelView ) );
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
	RippleTaintDown(this);
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

void TransformNode::RippleTaintDown(MercuryNode* node)
{
	TransformNode* tn;

	for (MercuryNode* n = node->FirstChild(); n != NULL; n = node->NextChild(n))
	{
		tn = TransformNode::Cast(n);
		if (tn)
			//stop this recursion here on this branch SetTaint will start a new taint recursion
			tn->SetTaint(true);
		else
			RippleTaintDown( n );
	}
}

void TransformNode::SaveToXMLTag( MString & sXMLStream )
{
	MercuryNode::SaveToXMLTag( sXMLStream );

	const MercuryVertex & scale = GetScale();
	if( scale[0] != 1 ) sXMLStream += ssprintf( "scalex=\"%f\" ", scale[0] );
	if( scale[1] != 1 ) sXMLStream += ssprintf( "scaley=\"%f\" ", scale[1] );
	if( scale[2] != 1 ) sXMLStream += ssprintf( "scalez=\"%f\" ", scale[2] );

	const MercuryVertex & pos = GetPosition();
	if( pos[0] != 0 ) sXMLStream += ssprintf( "movx=\"%f\" ", pos[0] );
	if( pos[1] != 0 ) sXMLStream += ssprintf( "movy=\"%f\" ", pos[1] );
	if( pos[2] != 0 ) sXMLStream += ssprintf( "movz=\"%f\" ", pos[2] );

	MercuryVertex r;
	GetRotation().ToEuler( r );
	if( r[0] != 0 ) sXMLStream += ssprintf( "rotx=\"%f\" ", r[0] * RADDEG );
	if( r[1] != 0 ) sXMLStream += ssprintf( "roty=\"%f\" ", r[1] * RADDEG );
	if( r[2] != 0 ) sXMLStream += ssprintf( "rotz=\"%f\" ", r[2] * RADDEG );

}

void TransformNode::LoadFromXML(const XMLNode& node)
{
	MercuryVertex pos(m_position), scale(m_scale);
	MQuaternion rot(m_rotation);
	
	//only change the values that exist in the XML
	if ( !node.Attribute("rotx").empty() )
		rot *= MQuaternion::CreateFromAxisAngle(MercuryVector(1,0,0), StrToFloat( node.Attribute("rotx") )*DEGRAD );

	if ( !node.Attribute("roty").empty() )
		rot *= MQuaternion::CreateFromAxisAngle(MercuryVector(0,1,0), StrToFloat( node.Attribute("roty") )*DEGRAD );

	if ( !node.Attribute("rotz").empty() )
		rot *= MQuaternion::CreateFromAxisAngle(MercuryVector(0,0,1), StrToFloat( node.Attribute("rotz") )*DEGRAD );
	
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
			RippleTaintDown(tn);
			return;
		}
		n = n->Parent();
	}	
}

const MercuryMatrix&  TransformNode::GetGlobalMatrix() const
{
	return m_globalMatrix;
}

void RotatorNode::Update(float dTime)
{
	MQuaternion r = GetRotation();
	r.X() += (dTime)*25;
	r.Y() += (dTime)*75;
	
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
