#include <Camera.h>
#include <MercuryMessageManager.h>
#include <MercuryInput.h>

REGISTER_NODE_TYPE(CameraNode);

CameraNode::CameraNode()
	:TransformNode()
{
	REGISTER_FOR_MESSAGE( INPUTEVENT_MOUSE );
}

void CameraNode::ComputeMatrix()
{
	m_tainted = false;
	
	MercuryMatrix local;
	
	local.RotateXYZ( m_rotation*-1 );
	local.Translate( m_position*-1 );
	
	m_globalMatrix = GetParentMatrix() * local;
}

void CameraNode::HandleMessage(const MString& message, const MessageData* data)
{
	if (message == INPUTEVENT_MOUSE)
	{
		MouseInput* m = (MouseInput*)data;
		
		MercuryVector r = m_rotation;
		r[0] += m->dy/30.0f;
		r[1] += m->dx/30.0f;
		
		SetRotation(r);
	}
}

void CameraNode::Update(float dTime)
{
	MercuryVector p = GetPosition();
	
	if ( KeyboardInput::IsKeyDown(25) ) p[2] -= dTime*2;
	if ( KeyboardInput::IsKeyDown(38) ) p[0] -= dTime*2;
	if ( KeyboardInput::IsKeyDown(39) ) p[2] += dTime*2;
	if ( KeyboardInput::IsKeyDown(40) ) p[0] += dTime*2;

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
