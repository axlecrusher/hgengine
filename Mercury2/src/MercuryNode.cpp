#include <MercuryNode.h>
#include <MercuryUtil.h>

using namespace std;

MercuryNode::MercuryNode()
	:m_parent(NULL)
{
}

MercuryNode::~MercuryNode()
{
	m_parent = NULL;
	
	list< MercuryNode* >::iterator i;
	for (i = m_children.begin(); i != m_children.end(); ++i )
		SAFE_DELETE(*i);
	
	m_children.clear();
}

void MercuryNode::AddChild(MercuryNode* n)
{
	m_children.push_back(n);
	n->m_parent = this;
	
	list< Callback2< MercuryNode*, MercuryNode* > >::iterator i;
	for (i = OnAddChild.begin(); i != OnAddChild.end(); ++i )
		(*i)(this, n);
}

void MercuryNode::RemoveChild(MercuryNode* n)
{
	list< MercuryNode* >::iterator i;
	for (i = m_children.begin(); i != m_children.end(); ++i )
	{
		if (*i == n)
		{
			list< Callback2< MercuryNode*, MercuryNode* > >::iterator ic;
			for (ic = OnRemoveChild.begin(); ic != OnRemoveChild.end(); ++ic )
				(*ic)(this, n);
			
			m_children.erase(i);
			return;
		}
	}
}

MercuryNode* MercuryNode::Parent() const
{
	return m_parent;
}

MercuryNode* MercuryNode::NextSibling() const
{
	if (m_parent) return m_parent->NextChild(this);
	return NULL;
}

MercuryNode* MercuryNode::PrevSibling() const
{
	if (m_parent) return m_parent->PrevChild(this);
	return NULL;
}

MercuryNode* MercuryNode::NextChild(const MercuryNode* n) const
{
	list< MercuryNode* >::const_iterator i;
	for (i = m_children.begin(); i != m_children.end(); ++i )
		if (*i == n) return *i;
	return NULL;
}

MercuryNode* MercuryNode::PrevChild(const MercuryNode* n) const
{
	list< MercuryNode* >::const_iterator i;
	for (i = m_children.end(); i != m_children.begin(); --i )
		if (*i == n) return *i;
	return NULL;
}

void MercuryNode::RecursiveUpdate(float dTime)
{
	Update(dTime);
	
	list< MercuryNode* >::iterator i;
	for (i = m_children.begin(); i != m_children.end(); ++i )
		(*i)->RecursiveUpdate(dTime);
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
