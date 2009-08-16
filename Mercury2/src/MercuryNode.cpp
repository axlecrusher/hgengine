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
	:m_parent(NULL), m_prevSibling(NULL), m_nextSibling(NULL), m_hidden(false), m_useAlphaPath(false), m_culled(false)
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

void MercuryNode::AddAsset(MercuryAsset* asset)
{
	m_assets.push_back( MercuryAssetInstance(asset) );
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

MercuryNode* MercuryNode::FindParent( const MString & sNameOfNode, int depth )
{
	MercuryNode * ret = this;
	while( ret && ret->GetName() != sNameOfNode )
		ret = ret->Parent();
	return ret;
}

MercuryNode * MercuryNode::TraversalNextNode( MercuryNode * stopnode, int & iDepthDelta )
{
	if( !m_children.empty() )
	{
		iDepthDelta++;
		return *(m_children.begin());
	}
	else if( m_nextSibling )
		return m_nextSibling;
	else if( m_parent )
	{
		MercuryNode * ret = m_parent;
		iDepthDelta--;

		while( ret && ret != stopnode && !ret->m_nextSibling )
		{
			iDepthDelta--;
			ret = ret->m_parent;
		}

		if( !ret || ret == stopnode )
			return 0;
		return ret->m_nextSibling;
	}
	else
		return 0;
}

void MercuryNode::RecursiveUpdate(float dTime)
{
	Update(dTime);
	
	for (MercuryNode* child = FirstChild(); child != NULL; child = NextChild(child))
		child->RecursiveUpdate(dTime);
}

void MercuryNode::RecursivePreRender()
{
	if ( IsHidden() ) return;
	
	const MercuryMatrix& matrix = FindGlobalMatrix();
	
	PreRender( matrix ); //calls on children assets

	for (MercuryNode* child = FirstChild(); child != NULL; child = NextChild(child))
	{
		child->RecursivePreRender();
		m_culled = m_culled && child->IsCulled();
	}
}

void MercuryNode::RecursiveRender()
{
	if ( IsHidden() || IsCulled() ) return;
		
	const MercuryMatrix& matrix = FindGlobalMatrix();
	const MercuryMatrix& modelView = FindModelViewMatrix(); //get the one computed in the last transform
		
	
	//A lot of this stuff could be moved into the transform node, BUT
	//the alpha render path requires that all things things happen, so
	//it is just easier to leave it here than to duplicate this code in
	//RenderGraph::RenderAlpha
	glLoadMatrix( modelView );
		
	ShaderAttribute sa;
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
			}
		}
	}
}


void MercuryNode::PreRender(const MercuryMatrix& matrix)
{
	SetCulled( false );
	bool culled = true;
	
	std::list< MercuryAssetInstance >::iterator i;
	for (i = m_assets.begin(); i != m_assets.end(); ++i )
	{
		MercuryAssetInstance& mai = *i;
		MercuryAsset& a = mai.Asset();
		if ( !a.ExcludeFromCull() )
			culled = culled && mai.Culled( a.DoCullingTests( mai.GetOcclusionResult(), matrix ) );
		if ( !mai.Culled() ) a.PreRender(this);
	}
	SetCulled( culled );
}

void MercuryNode::Render(const MercuryMatrix& matrix)
{
	std::list< MercuryAssetInstance >::iterator i;
	for (i = m_assets.begin(); i != m_assets.end(); ++i )
		if ( !(i->Culled() || i->GetOcclusionResult().IsOccluded()) ) i->Asset().Render(this);
}

void MercuryNode::PostRender(const MercuryMatrix& matrix)
{
	std::list< MercuryAssetInstance >::iterator i;
	for (i = m_assets.begin(); i != m_assets.end(); ++i )
		i->Asset().PostRender(this);
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

const MercuryMatrix& MercuryNode::FindModelViewMatrix() const
{
	const MercuryNode* n = NULL;
	const TransformNode* tn;
	for (n = this; n; n = n->Parent())
	{
		tn = TransformNode::Cast(n);
		if ( tn )
			return tn->GetModelViewMatrix();
	}

	return MercuryMatrix::Identity();
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
