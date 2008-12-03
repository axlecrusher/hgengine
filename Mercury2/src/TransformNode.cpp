#include <TransformNode.h>

REGISTER_NODE_TYPE(TransformNode);

TransformNode::TransformNode()
	:m_scale( MercuryPoint(1,1,1) )
{
}

void TransformNode::Update(float dTime)
{
	if (m_tainted) ComputeMatrix();
}

void TransformNode::SetScale( const MercuryPoint& scale )
{
	m_scale = scale;
	SetTaint( true );
}

void TransformNode::SetPosition( const MercuryPoint& position )
{
	m_position = position;
	SetTaint( true );
}

void TransformNode::SetRotation( const MercuryPoint& rotation )
{
	m_rotation = rotation;
	SetTaint( true );
}

void TransformNode::SetTaint(bool taint)
{
	m_tainted = taint;
	RippleTaintDown();
}

void TransformNode::ComputeMatrix()
{
	m_tainted = false;
	
	m_localMatrix.Identity();

	m_localMatrix.Transotale( m_position.x, m_position.y, m_position.z,
							  m_rotation.x, m_rotation.y, m_rotation.z,
							  m_scale.x, m_scale.y, m_scale.z );
	
	m_globalMatrix = GetParentMatrix() * m_localMatrix;
}

const MercuryMatrix& TransformNode::GetParentMatrix() const
{
	const MercuryNode* n = m_parent;
	const TransformNode* tn;
	while (n)
	{
		if ( (tn = TransformNode::Cast( n )) ) return tn->GetGlobalMatrix();
		n = n->Parent();
	}
	
	return MercuryMath::IdentityMatrix;
}

void TransformNode::RippleTaintDown()
{
	if (m_tainted == true)
	{
		TransformNode* tn;
		std::list< MercuryNode* >::iterator i;
		
		for (i = m_children.begin(); i != m_children.end(); ++i )
		{
			if ( (tn = TransformNode::Cast(*i)) )
				tn->SetTaint( true );
		}
	}
}

/***************************************************************************
 *   Copyright (C) 2008 by Joshua Allen   *
 *      *
 *                                                                         *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: *
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. *
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. *
 *     * Neither the name of the <ORGANIZATION> nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission. *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR *
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR *
 *   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, *
 *   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,   *
 *   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR    *
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF *
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  *
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS    *
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.          *
 ***************************************************************************/
