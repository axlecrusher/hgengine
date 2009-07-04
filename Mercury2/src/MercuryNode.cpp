#include <MercuryNode.h>
#include <MercuryUtil.h>
#include <UpdateThreader.h>
#include <TransformNode.h>
#include <Viewport.h>


#include <GLHeaders.h>
#include <Shader.h>
#include <RenderGraph.h>

using namespace std;

REGISTER_NODE_TYPE(MercuryNode);

MercuryNode::MercuryNode()
	:m_parent(NULL), m_prevSibling(NULL), m_nextSibling(NULL), m_hidden(false), m_useAlphaPath(false)
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
//	list< MercuryNode* >::iterator last = m_children.end();
	
	n->m_prevSibling = NULL;
	n->m_nextSibling = NULL;
	if (m_children.end() != m_children.begin())
	{
		MercuryNode* last = m_children.back();
		last->m_nextSibling = n;
		n->m_prevSibling = last;
	}
	
	m_children.push_back(n);
	n->m_parent = this;
	
	OnAddChild();
	n->OnAdded();
	m_rebuildRenderGraph = true;
}

void MercuryNode::RemoveChild(MercuryNode* n)
{
	list< MercuryNode* >::iterator i;
	for (i = m_children.begin(); i != m_children.end(); ++i )
	{
		if (*i == n)
		{
			MercuryNode* next = n->m_nextSibling;
			MercuryNode* prev = n->m_prevSibling;

			n->OnRemoved();
			OnRemoveChild();
			
			if (prev) prev->m_nextSibling = next;
			if (next) next->m_prevSibling = prev;

			n->m_nextSibling = NULL;
			n->m_prevSibling = NULL;
			n->m_parent = NULL;
			
			m_children.erase(i);
			m_rebuildRenderGraph = true;
			
			return;
		}
	}
}

MercuryNode* MercuryNode::FirstChild() const
{
	if (m_children.begin() != m_children.end())
		return *(m_children.begin());
	return NULL;
}

MercuryNode* MercuryNode::NextChild(const MercuryNode* child) const
{
	if (child==NULL) return NULL;
	return child->NextSibling();
}

MercuryNode* MercuryNode::PrevChild(const MercuryNode* child) const
{
	if (child==NULL) return NULL;
	return child->PrevSibling();
}

MercuryNode* MercuryNode::FindChild( const MString & sNameOfNode, int depth )
{
	if( depth >= 0 )
	{
		for( std::list< MercuryNode* >::iterator i = m_children.begin(); i != m_children.end(); i++ )
		{
			if( (*i)->GetName().compare( sNameOfNode ) == 0 )
				return (*i);
		}

		for( std::list< MercuryNode* >::iterator i = m_children.begin(); i != m_children.end(); i++ )
		{
			MercuryNode * ret;
			if( ( ret = (*i)->FindChild( sNameOfNode, depth - 1 ) ) )
			{
				if( (*i)->GetName().compare( sNameOfNode ) == 0 )
					return ret;
			}
		}
	}
	return NULL;
}


void MercuryNode::RecursiveUpdate(float dTime)
{
	Update(dTime);
	
	for (MercuryNode* child = FirstChild(); child != NULL; child = NextChild(child))
		child->RecursiveUpdate(dTime);
}

void MercuryNode::RecursiveRender()
{
	MercuryMatrix modelView;
	ShaderAttribute sa;
	
	MercuryMatrix matrix = FindGlobalMatrix();
	
	PreRender( matrix ); //calls on children assets
	
	modelView = ManipulateMatrix( matrix );
//	if ( IsHidden() || IsCulled(modelView) ) return;
	if ( IsHidden() ) return;
	
	glLoadMatrix( modelView );
		
	sa.type = ShaderAttribute::TYPE_MATRIX;
	sa.value.matrix = matrix.Ptr();
	Shader::SetAttribute("HG_ModelMatrix", sa);
		
	Render( modelView ); //calls on children assets
	
	//call render on other render graph entries under me
	for (MercuryNode* child = FirstChild(); child != NULL; child = NextChild(child))
	{
		if ( child->m_useAlphaPath )
			CURRENTRENDERGRAPH->AddAlphaNode(child);
		else
			child->RecursiveRender();
	}

	glLoadMatrix( modelView );
	Shader::SetAttribute("HG_ModelMatrix", sa);
	PostRender( modelView );  //calls on children assets
}

void MercuryNode::ThreadedUpdate(float dTime)
{
	//XXX EXPERIMENTAL
	Update(dTime);
	
	list< MercuryNode* >::iterator i;
	for (i = m_children.begin(); i != m_children.end(); ++i )
		UpdateThreader::GetInstance().AddNode( *i );
}

void MercuryNode::LoadFromXML(const XMLNode& node)
{
	SetName( node.Attribute("name") );
	
	if ( !node.Attribute("hidden").empty() )
		m_hidden = node.Attribute("hidden")=="true"?true:false;

	if ( !node.Attribute("alphaPath").empty() )
		m_useAlphaPath = node.Attribute("alphaPath")=="true"?true:false;

	//Not much to do here except run through all the children nodes
	for (XMLNode child = node.Child(); child.IsValid(); child = child.NextNode())
	{
		if ( child.Name() == "node" )
		{
			MercuryNode* node = NodeFactory::GetInstance().Generate( child.Attribute("type") );
			if (!node) node = new MercuryNode();
			node->LoadFromXML( child );
			this->AddChild( node );
		}
		else if ( child.Name() == "asset" )
		{
			MString key = child.Attribute("file");
			MAutoPtr< MercuryAsset > asset( AssetFactory::GetInstance().Generate( child.Attribute("type"), key ) );
			if ( asset.IsValid() )
			{
				asset->LoadFromXML( child );
				this->AddAsset( asset );
				asset->Init( this );
			}
		}
	}
}


void MercuryNode::PreRender(const MercuryMatrix& matrix)
{
	list< MercuryAsset* >::iterator i;
	for (i = m_prerender.begin(); i != m_prerender.end(); ++i )
		(*i)->PreRender(this);
}

void MercuryNode::Render(const MercuryMatrix& matrix)
{
	bool cull;
	list< MercuryAsset* >::iterator i;
	for (i = m_render.begin(); i != m_render.end(); ++i )
	{
		cull = false;
		const BoundingVolume* bv = (*i)->GetBoundingVolume();
		if (bv) cull = bv->FrustumCull();
		if ( !cull ) (*i)->Render(this);
	}
}

void MercuryNode::PostRender(const MercuryMatrix& matrix)
{
	list< MercuryAsset* >::iterator i;
	for (i = m_postrender.begin(); i != m_postrender.end(); ++i )
		(*i)->PostRender(this);
}

const MercuryMatrix& MercuryNode::FindGlobalMatrix() const
{
	const MercuryNode* n = NULL;
	const TransformNode* tn;
	for (n = this; n; n = n->Parent())
	{
		tn = TransformNode::Cast(n);
		if ( tn )
			return tn->GetGlobalMatrix();
	}

	return MercuryMatrix::Identity();
}

void MercuryNode::AddPreRender(MercuryAsset* asset)
{
#ifdef MCHECKASSETS
	if ( !IsInAssetList(asset) ) //yell and scream
		assert(!"Asset does not exist in list!");
#endif
		
	m_prerender.push_back(asset);
}

void MercuryNode::AddRender(MercuryAsset* asset)
{
#ifdef MCHECKASSETS
	if ( !IsInAssetList(asset) ) //yell and scream
		assert(!"Asset does not exist in list!");
#endif
	
	m_render.push_back(asset);
}
void MercuryNode::AddPostRender(MercuryAsset* asset)
{
#ifdef MCHECKASSETS
	if ( !IsInAssetList(asset) ) //yell and scream
		assert(!"Asset does not exist in list!");
#endif
	
	m_postrender.push_back(asset);
}

bool MercuryNode::IsInAssetList( MercuryAsset* asset ) const
{
	std::list< MAutoPtr< MercuryAsset > >::const_iterator i;
	for (i = m_assets.begin(); i != m_assets.end(); ++i )
		if ( (*i) == asset ) return true;
	return false;
}

bool MercuryNode::IsCulled(const MercuryMatrix& matrix)
{
	bool clip = false;
/*	
	std::list< MAutoPtr< MercuryAsset > >::iterator i;
	for (i = m_assets.begin(); i != m_assets.end(); ++i )
	{
		clip = (*i)->IsCulled( matrix );
		if ( clip == false ) return false;
	}
	*/
	return clip;
}

MercuryMatrix MercuryNode::ManipulateMatrix(const MercuryMatrix& matrix)
{
	return VIEWMATRIX * matrix;
}


NodeFactory& NodeFactory::GetInstance()
{
	static NodeFactory* instance = NULL;
	if (!instance)
		instance = new NodeFactory;
	return *instance;

}

bool NodeFactory::RegisterFactoryCallback(const MString& type, Callback0R<MercuryNode*> functor)
{
	MString t = ToUpper( type );
	std::pair<MString, Callback0R<MercuryNode*> > pp(t, functor);
	m_factoryCallbacks.push_back( pp );
	return true;
}

MercuryNode* NodeFactory::Generate(const MString& type)
{
	MString t = ToUpper( type );
	std::list< std::pair< MString, Callback0R<MercuryNode*> > >::iterator i;
	for (i = m_factoryCallbacks.begin(); i != m_factoryCallbacks.end(); ++i)
		if (i->first == t) return i->second();
	printf("WARNING: Node type %s not found.\n", type.c_str());
	return NULL;
}

bool MercuryNode::m_rebuildRenderGraph = false;
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
