#include <MercuryMatrix.h>
#include <RenderGraph.h>
#include <MercuryCTA.h>
#include <GLHeaders.h>

#include <Shader.h>
#include <Viewport.h>
#include <Texture.h>

MercuryCTA AlphaHolderAllocator( sizeof(StoreRenderState), 8 );

RenderGraph* CURRENTRENDERGRAPH = NULL;



void RenderGraphEntry::Render()
{
	MercuryMatrix modelView;
	ShaderAttribute sa;
		
	if (m_node)
	{	
		m_node->PreRender( *m_matrix ); //calls on children assets
		modelView = m_node->ManipulateMatrix( *m_matrix );
		if ( m_node->IsHidden() || m_node->IsCulled() ) return;
		
		GLCALL( glLoadMatrix( modelView ) );
		
		sa.type = ShaderAttribute::TYPE_MATRIX;
		sa.value.matrix = m_matrix->Ptr();
		Shader::SetAttribute("HG_ModelMatrix", sa);
		
		m_node->Render( modelView ); //calls on children assets
	}
	
	//call render on other render graph entries under me
	std::list< RenderGraphEntry >::iterator i;
	for (i = m_children.begin(); i != m_children.end(); ++i )
		i->Render();

	if (m_node)
	{
		GLCALL( glLoadMatrix( modelView ) );
		Shader::SetAttribute("HG_ModelMatrix", sa);
		m_node->PostRender( modelView );  //calls on children assets
	}	
}

void RenderGraph::Build( MercuryNode* node )
{
//	printf("rebuilding render graph\n");
	m_root = RenderGraphEntry();
	Build(node, m_root);
}

void RenderGraph::Build( MercuryNode* node, RenderGraphEntry& entry)
{
	RenderGraphEntry* lastEntry = &entry;
//	MercuryNode* rn = MercuryNode::Cast(node);

	if ( node )
	{
		//found a new renderable
		entry.m_children.push_back( RenderGraphEntry(&(node->GetGlobalMatrix()), node) );
		lastEntry = &(entry.m_children.back());
	}

	unsigned short iPasses = 0;
	
	for (MercuryNode* child = node->FirstChild(); child != NULL; child = node->NextChild(child))
	{
		Build(child, *lastEntry);
		iPasses |= child->m_iPasses;
	}

	std::list< MercuryAssetInstance* >::iterator i;
	for (i = node->m_assets.begin(); i != node->m_assets.end(); ++i )
		iPasses |= (*i)->GetPasses();

	if( node->m_iForcePasses & (1<<15 ) )
		node->m_iPasses = node->m_iForcePasses;
	else
		node->m_iPasses = iPasses;

	//coming back up the tree
//	entry = lastEntry;
}

void RenderGraph::AddAlphaNode( MercuryNode* node )
{
	StoreRenderState * srs = new(AlphaHolderAllocator.Malloc()) StoreRenderState();

	srs->Save();
	srs->Matrix = node->GetGlobalMatrix();
	MercuryVertex p = srs->Matrix * MercuryVertex(0,0,0,1);
	srs->Node = node;
	//order from back to front (ensure furthest has lowest number and is first)
	srs->fDistance = (p - EYE).Length() * -1;

	//This makes the insertion linear time.  We fix this up before rendering.
	m_alphaNodesQueue.PushRaw( srs );
}

void RenderGraph::RenderAlpha()
{
	m_alphaNodesQueue.Fixup();

	while ( !m_alphaNodesQueue.Empty() )
	{
		StoreRenderState * srs = (StoreRenderState *)m_alphaNodesQueue.Pop();

		std::list< MercuryAsset* >::iterator i = srs->Assets.begin();
		for (;i != srs->Assets.end(); ++i)
		{
			(*i)->PreRender(srs->Node);
			(*i)->Render(srs->Node);
		}
		
		srs->Node->RecursiveRender();
	
		for (i = srs->Assets.begin();i != srs->Assets.end(); ++i)
			(*i)->PostRender(srs->Node);
		
		srs->~StoreRenderState();//explicitly call destructor since free() is used
		AlphaHolderAllocator.Free( srs );
	}
}

void RenderGraph::AddDifferedLight( Light* l )
{
	m_lights.push_back( l );
}

void RenderGraph::DoDifferedLightPass()
{
	std::list< Light* >::iterator i;
	for (i = m_lights.begin();i != m_lights.end(); ++i)
		(*i)->DifferedRender();
	
	m_lights.clear();
}


StoreRenderState::StoreRenderState()
	:Node(NULL)
{
}

bool StoreRenderState::Compare( void * Left, void * Right )
{
	return ((StoreRenderState*)Left)->fDistance < ((StoreRenderState*)Right)->fDistance;
}

void StoreRenderState::Save()
{
	//get assets for current textures
	const ArrayStack< Texture* >& textures = Texture::GetActiveTextures();
	for (unsigned short i = 0; i<textures.Depth(); ++i)
		Assets.push_back( textures[i] );

	//save the active shader
	Shader* s = Shader::GetCurrentShader();
	if (s) Assets.push_back(s);
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
