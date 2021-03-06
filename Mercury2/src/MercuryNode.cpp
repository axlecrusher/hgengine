#include <MercuryNode.h>
#include <MercuryUtil.h>
#include <UpdateThreader.h>
#include <TransformNode.h>
#include <Viewport.h>


#include <GLHeaders.h>
#include <Shader.h>
#include <RenderGraph.h>

#include <ModuleManager.h>

using namespace std;

REGISTER_NODE_TYPE(MercuryNode);

MercuryNode::MercuryNode()
	:m_parent(NULL), m_prevSibling(NULL),
	m_nextSibling(NULL), m_flags(SAVECHILDREN & ENABLESAVE),
	m_iPasses( DEFAULT_PASSES ), m_iForcePasses( 0 ),
	m_shaderAttrModelMatrix(NULL)
{
	m_pGlobalMatrix = &MercuryMatrix::IdentityMatrix;
	m_pModelViewMatrix = &MercuryMatrix::IdentityMatrix;
}

MercuryNode::~MercuryNode()
{

#ifdef INSTANCE_WATCH
	DEL_INSTANCE(this);
#endif

	m_parent = NULL;
	
	list< MercuryNode* >::iterator i;
	for (i = m_children.begin(); i != m_children.end(); ++i )
		SAFE_DELETE(*i);
	
	m_children.clear();
	
	ClearAssets();
}

void MercuryNode::AddAsset(MercuryAsset* asset)
{
	m_assets.push_back( asset->MakeAssetInstance( this ) );
}

void MercuryNode::ClearAssets()
{
	list< MercuryAssetInstance* >::iterator i;
	for (i = m_assets.begin(); i != m_assets.end(); ++i )
		SAFE_DELETE(*i);

	m_assets.clear( );
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

MercuryNode* MercuryNode::LastChild() const
{
	if( !m_children.empty() )
		return m_children.back();
	else
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
				if( ret )
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

void MercuryNode::RecursiveUpdate(float dTime, const MercuryMatrix& globalMatrix)
{
	m_pGlobalMatrix = &globalMatrix;
	if (m_parent) m_pModelViewMatrix = m_parent->m_pModelViewMatrix;

	Update(dTime);

	for (MercuryNode* child = FirstChild(); child != NULL; child = NextChild(child))
		child->RecursiveUpdate(dTime, *m_pGlobalMatrix);
}

void MercuryNode::RecursivePreRender()
{
	if ( IsHidden() ) return;
	
	const MercuryMatrix& matrix = GetGlobalMatrix();
	
	PreRender( matrix ); //calls on children assets

	for (MercuryNode* child = FirstChild(); child != NULL; child = NextChild(child))
	{
		child->RecursivePreRender();
		SetCulled( IsCulled() && child->IsCulled() );
	}
}

//#define WRITE_OUT_RENDERGARPH

void MercuryNode::RecursiveRender( )
{
#ifdef WRITE_OUT_RENDERGARPH
	static int depth;
	if ( IsHidden() || IsCulled() || ((! (m_iPasses & (1<<g_iPass))) &&  m_iForcePasses ) )
	{
		printf( "x%*c %p:%s (%d %d %d)\n", depth, 0, this, GetName().c_str(), IsHidden(), IsCulled(), (! (m_iPasses & (1<<g_iPass))) );
		return;
	}
	printf( "1%*c %p:%s\n", depth, 0, this, GetName().c_str() );
	depth++;
#else
	if ( IsHidden() || IsCulled() || ((! (m_iPasses & (1<<g_iPass))) &&  m_iForcePasses )  )
		return;
#endif

	const MercuryMatrix& matrix = GetGlobalMatrix();
	const MercuryMatrix& modelView = GetModelViewMatrix(); //get the one computed in the last transform
//	ShaderAttribute sa;

	//A lot of this stuff could be moved into the transform node, BUT
	//the alpha render path requires that all things things happen, so
	//it is just easier to leave it here than to duplicate this code in
	//RenderGraph::RenderAlpha

	GLCALL( glLoadMatrix( modelView ) );

	if (m_shaderAttrModelMatrix == NULL)
		m_shaderAttrModelMatrix = Shader::GetAttribute("HG_ModelMatrix");

	m_shaderAttrModelMatrix->type = ShaderAttribute::TYPE_MATRIX;
	m_shaderAttrModelMatrix->value.matrix = matrix.Ptr();

	Render( modelView ); //calls on children assets
	
	//call render on other render graph entries under me
	for (MercuryNode* child = FirstChild(); child != NULL; child = NextChild(child))
	{
		if ( child->GetUseAlphaPass() )
			CURRENTRENDERGRAPH->AddAlphaNode(child);
		else
			child->RecursiveRender();
	}

	GLCALL( glLoadMatrix( modelView ) );
	m_shaderAttrModelMatrix->value.matrix = matrix.Ptr();

	PostRender( modelView );  //calls on children assets

#ifdef WRITE_OUT_RENDERGARPH
	depth--;
#endif
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
	
	bool t;
	t = IsHidden(); LOAD_FROM_XML( "hidden", t, StrToBool ); SetHidden(t);
	t = GetUseAlphaPass(); LOAD_FROM_XML( "alphaPath", t, StrToBool ); SetUseAlphaPass(t);
	t = GetEnableSave(); LOAD_FROM_XML( "enableSave", t, StrToBool ); SetEnableSave(t);
	t = GetSaveChildren(); LOAD_FROM_XML( "enableSaveChildren", t, StrToBool );
	SetSaveChildren(t);


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

			MAutoPtr< MercuryAsset > asset( AssetFactory::GetInstance().Generate( child.Attribute("type"), key, true, &child ) );
			if ( asset.IsValid() )
			{
				asset->LoadFromXML( child );
				this->AddAsset( asset );
			}
		}
	}

	if ( !node.Attribute("setPasses").empty() )
	{
		MVector< MString > out;
		SplitStrings( node.Attribute("setPasses"), out, ",+", " \t", 2, 2 );
		m_iForcePasses = (1<<15);
		for( unsigned i = 0; i < out.size(); i++ )
			m_iForcePasses = m_iForcePasses | (1<<StrToInt( out[i] ) );
	}
}

void MercuryNode::SaveToXML( MString & sXMLStream, int depth )
{
	if( !GetEnableSave() ) return;
	sXMLStream += ssprintf( "%*c<node ", depth * 3, 32 );

	SaveBaseXMLTag( sXMLStream );
	SaveToXMLTag( sXMLStream );

	bool bNoChildren = true;

	if( GetSaveChildren() )
	{
		if( !m_assets.empty() )
		{
			//No children yet (but we have them, so terminate (>) )
			if( bNoChildren )
				sXMLStream += ">\n";
			bNoChildren = false;

			for( std::list< MercuryAssetInstance * >::iterator i = m_assets.begin(); i != m_assets.end(); i++ )
				(*i)->Asset().SaveToXML( sXMLStream, depth + 1 );
		}

		if( ! m_children.empty() )
		{
			//No children yet (but we have them, so terminate (>) )
			for( std::list< MercuryNode * >::iterator i = m_children.begin(); i != m_children.end(); i++ )
			{
				if( (*i)->GetEnableSave() )
				{
					if( bNoChildren )
						sXMLStream += ">\n";
					bNoChildren = false;
					(*i)->SaveToXML( sXMLStream, depth + 1 );
				}
			}
		}
	}

	if( bNoChildren )
		sXMLStream += "/>\n";
	else
		sXMLStream += ssprintf( "%*c</node>\n", depth * 3, 32 );
}

void MercuryNode::SaveToXMLTag( MString & sXMLStream )
{
	//Don't actually do anything here
}

void MercuryNode::SaveBaseXMLTag( MString & sXMLStream )
{
	sXMLStream+= ssprintf( "type=\"%s\" ", GetType() );
	if( GetName().length() )
		sXMLStream += ssprintf( "name=\"%s\" ", GetName().c_str() );
	if( GetUseAlphaPass() )
		sXMLStream += "alphaPath=\"true\" ";
}

void MercuryNode::PreRender(const MercuryMatrix& matrix)
{
	SetCulled( false );
	bool culled = true;
	
	std::list< MercuryAssetInstance* >::iterator i;
	for (i = m_assets.begin(); i != m_assets.end(); ++i )
	{
		MercuryAssetInstance* mai = *i;
		MercuryAsset& a = mai->Asset();
		
		///NOTE: Things that ignore culling do not affect culling one way or the other
		if ( !a.IgnoreCull() )
		{
			mai->Culled( a.DoCullingTests( mai->GetOcclusionResult(), matrix ) );
			culled = culled && mai->Culled();
		}
		
		if ( !mai->Culled() ) a.PreRender(this);
	}
	SetCulled( culled );
}

void MercuryNode::Render(const MercuryMatrix& matrix)
{
	std::list< MercuryAssetInstance* >::iterator i;
	for (i = m_assets.begin(); i != m_assets.end(); ++i )
	{
		MercuryAssetInstance* mai = *i;
		if ( !(mai->Culled() || mai->GetOcclusionResult().IsOccluded()) ) mai->Asset().Render(this);
	}
}

void MercuryNode::PostRender(const MercuryMatrix& matrix)
{
	std::list< MercuryAssetInstance* >::iterator i;
	for (i = m_assets.begin(); i != m_assets.end(); ++i )
		(*i)->Asset().PostRender(this);
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
		if (i->first == t)
		{
			MercuryNode * n = i->second();
#ifdef INSTANCE_WATCH
			NEW_INSTANCE(n, n->GetType());
#endif
			return n;
		}
	LOG.Write( "WARNING: Node type " + type + " not found." );
	return NULL;
}

bool MercuryNode::m_rebuildRenderGraph = false;
__ThreadLocalStore int g_iViewportID = 0;
__ThreadLocalStore int g_iPass;

/****************************************************************************
 *   Copyright (C) 2008-2009 by Joshua Allen                                *
 *                              Charles Lohr                                *
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


