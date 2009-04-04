#include <MercuryMatrix.h>
#include <RenderGraph.h>

#include <GL/gl.h>

void RenderGraphEntry::Render()
{
	MercuryMatrix m;
	
	if (m_node)
	{	
		if ( !(m_node->IsHidden() || m_node->IsCulled()) )
		{
			m = m_node->FindGlobalMatrix();
			m.Transpose();
			glLoadMatrixf( m.Ptr() );
			m_node->PreRender( m );
			m_node->Render( m );
		}
	}
	
	std::list< RenderGraphEntry >::iterator i;
	for (i = m_children.begin(); i != m_children.end(); ++i )
		i->Render();

	if (m_node)
	{
		if ( !(m_node->IsHidden() || m_node->IsCulled()) )
		{
			glLoadMatrixf( m.Ptr() );
			m_node->PostRender( m );
		}
	}
}

void RenderGraph::Build( MercuryNode* node )
{
	printf("rebuilding render graph\n");
	m_root = RenderGraphEntry();
	Build(node, m_root);
}

void RenderGraph::Build( MercuryNode* node, RenderGraphEntry& entry)
{
	RenderGraphEntry* lastEntry = &entry;
	RenderableNode* rn = RenderableNode::Cast(node);
	
	if ( rn )
	{
		//found a new renderable
		printf("Found renderable %p\n", rn);
		entry.m_children.push_back( RenderGraphEntry(rn) );
		lastEntry = &(entry.m_children.back());
	}
	
	for (MercuryNode* child = node->FirstChild(); child != NULL; child = node->NextChild(child))
		Build(child, *lastEntry);
	
	//coming back up the tree
//	entry = lastEntry;
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
