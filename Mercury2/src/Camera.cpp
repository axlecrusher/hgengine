#include <Camera.h>
#include <MercuryMessageManager.h>
#include <MercuryInput.h>
#include <Viewport.h>

#include <Shader.h>

REGISTER_NODE_TYPE(CameraNode);

CameraNode::CameraNode()
	:TransformNode(), m_x(0), m_y(0)
{
	m_lookAt = MercuryVector(0,0,-1);
	REGISTER_FOR_MESSAGE( INPUTEVENT_MOUSE );
	REGISTER_FOR_MESSAGE( "SetCameraPosition" );
}

void CameraNode::PreRender(const MercuryMatrix& matrix)
{
	VIEWMATRIX = m_viewMatrix;
	TransformNode::PreRender(matrix);
}

void CameraNode::Render(const MercuryMatrix& matrix)
{
	VIEWMATRIX = m_viewMatrix;
	
	ShaderAttribute sa;
	sa.type = ShaderAttribute::TYPE_MATRIX;
	sa.value.matrix = m_viewMatrix.Ptr();
	Shader::SetAttribute("HG_ViewMatrix", sa);
	
	TransformNode::Render(matrix);
}

void CameraNode::ComputeMatrix()
{
	m_tainted = false;
	
	MercuryMatrix local, parent(GetParentMatrix());
	
	MQuaternion r( GetRotation() );
	
	//compute the world space look vector  (broken if camera is in transform node)
	m_lookAt = MercuryVector(0,0,-1); //by default camera looks down world Z
	m_lookAt = m_lookAt.Rotate( r );
	m_lookAt.NormalizeSelf();
	LOOKAT = m_lookAt;
	
	ShaderAttribute sa;
	sa.type = ShaderAttribute::TYPE_FLOATV4;
	sa.value.fFloatV4[0] = LOOKAT.GetX();
	sa.value.fFloatV4[1] = LOOKAT.GetY();
	sa.value.fFloatV4[2] = LOOKAT.GetZ();
	sa.value.fFloatV4[3] = 0;
	Shader::SetAttribute("HG_LookVector", sa);
		
	r.W() *= -1; //reverse angle for camera
	
	//rotate then translate since we are a camera
	r.toMatrix4( local );
	local.Translate( GetPosition()*-1 );
	
//	m_globalMatrix = local * parent; //fold in any parent transform in reverse (correct rotation)
	m_viewMatrix = local * parent;
	
	//compute camera position in world space (broken if camera is in transform node)
	local = MercuryMatrix::Identity();
	local.Translate( GetPosition() );
	EYE = (parent * local) * MercuryVertex(0,0,0,1);
	
	sa.type = ShaderAttribute::TYPE_FLOATV4;
	sa.value.fFloatV4[0] = EYE.GetX();
	sa.value.fFloatV4[1] = EYE.GetY();
	sa.value.fFloatV4[2] = EYE.GetZ();
	sa.value.fFloatV4[3] = 1.0f;
	Shader::SetAttribute("HG_EyePos", sa);

//	EYE.Print();
}

void CameraNode::HandleMessage(const MString& message, const MessageData& data)
{
	if (message == INPUTEVENT_MOUSE)
	{
		const MouseInput& m( dynamic_cast<const MouseInput&>( data ) );
		
		m_y += m.dy/1200.0f;
		m_x += m.dx/1200.0f;
		
		m_y = Clamp((-Q_PI/2.0f)+0.00001f, (Q_PI/2.0f)-0.00001f, m_y);

		MQuaternion qLeftRight = MQuaternion::CreateFromAxisAngle(MercuryVector(0,1,0), m_x);
		MercuryVector LocalX = MercuryVector( 1, 0, 0 );
		LocalX = LocalX.Rotate( qLeftRight );
		
		MQuaternion qUpDown = MQuaternion::CreateFromAxisAngle(LocalX, m_y);
		
//		qLeftRight.Print();
		
		SetRotation(qUpDown*qLeftRight);
//		GetRotation().Print();
//		POST_MESSAGE("QueryTerrainPoint", new MessageData(), 0);

	}
	else if (message == "SetCameraPosition")
	{
//		LOG.Write("SetCamPosition");
		const VertexDataMessage& m( dynamic_cast<const VertexDataMessage&>( data ) );
		SetPosition(m.Vertex);
//		Update(0);
//		ComputeMatrix();
//		m->Vertex.Print();
	}
}

void CameraNode::Update(float dTime)
{
//	MercuryVector p = GetPosition();
	MercuryVector p = m_origionalPosition;
	float a = 0;
	float b = 0;
	
	if ( KeyboardInput::IsKeyDown('w') ) a += dTime*2; //W
	if ( KeyboardInput::IsKeyDown('s') ) a -= dTime*2; //S
	
	if ( KeyboardInput::IsKeyDown('a') ) b -= dTime*2; //A
	if ( KeyboardInput::IsKeyDown('d') ) b += dTime*2; //D

	MercuryVector Xaxis = m_lookAt.CrossProduct( MercuryVector(0,1,0) );
	Xaxis.NormalizeSelf();
	
	p += m_lookAt * a;
	p += Xaxis * b;
//	p.SetY(0); //lock to ground
//	SetPosition( p );
	m_origionalPosition = p;
	TransformNode::Update( dTime );
	
	if (a != 0 || b != 0)
	{
		POST_MESSAGE("QueryTerrainPoint", new VertexDataMessage(p), 0);
	}

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
