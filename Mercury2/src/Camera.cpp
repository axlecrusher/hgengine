#include <Camera.h>
#include <MercuryMessageManager.h>
#include <MercuryInput.h>
#include <Viewport.h>

REGISTER_NODE_TYPE(CameraNode);

CameraNode::CameraNode()
	:TransformNode(), m_x(0), m_y(0)
{
	REGISTER_FOR_MESSAGE( INPUTEVENT_MOUSE );
}

void CameraNode::ComputeMatrix()
{
	m_tainted = false;
	
	MercuryMatrix local, parent(GetParentMatrix());
	
	MQuaternion r( GetRotation().normalize() );
	
	//compute the world space look vector  (broken if camera is in transform node)
	m_lookAt = MercuryVector(0,0,-1); //by default camera looks down world Z
	m_lookAt = m_lookAt.Rotate( r );
	m_lookAt.NormalizeSelf();
	LOOKAT = m_lookAt;
		
	r[MQuaternion::W] *= -1; //reverse angle for camera
	
	//rotate then translate since we are a camera
	r.toMatrix4( local );
	local.Translate( GetPosition()*-1 );
	
	m_globalMatrix = local * parent; //fold in any parent transform in reverse (correct rotation)
	
	//compute camera position in world space (broken if camera is in transform node)
	local = MercuryMatrix::Identity();
	local.Translate( GetPosition() );
	EYE = (parent * local) * MercuryVertex(0,0,0,1);
//	EYE.Print();
}

void CameraNode::HandleMessage(const MString& message, const MessageData* data)
{
	if (message == INPUTEVENT_MOUSE)
	{
		MouseInput* m = (MouseInput*)data;
		
		m_y += m->dy/1200.0f;
		m_x += m->dx/1200.0f;
		
		MercuryVector Xaxis = m_lookAt.CrossProduct( MercuryVector(0,1,0) );
		Xaxis.NormalizeSelf();
		
		MQuaternion qx = MQuaternion::CreateFromAxisAngle(Xaxis, m_y);
		MQuaternion qy = MQuaternion::CreateFromAxisAngle(MercuryVector(0,1,0), m_x);
		SetRotation(qx * qy);
	}
}

void CameraNode::Update(float dTime)
{
	MercuryVector p = GetPosition();
	float a = 0;
	
	if ( KeyboardInput::IsKeyDown(25) ) a += dTime*2;
	if ( KeyboardInput::IsKeyDown(39) ) a -= dTime*2;
	
	p += m_lookAt * a;
//	p.SetY(0); //lock to ground
	SetPosition( p );

	TransformNode::Update( dTime );
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
