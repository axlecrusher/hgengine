#include <RenderableNode.h>
#include <assert.h>
#include <GLHeaders.h>
#include <TransformNode.h>
//#include <unistd.h>
#include <Viewport.h>

using namespace std;

REGISTER_NODE_TYPE(RenderableNode);

RenderableNode::RenderableNode()
	:m_hidden(false)
{
}

RenderableNode::~RenderableNode()
{
	m_prerender.clear();
	m_render.clear();
	m_postrender.clear();
}

void RenderableNode::Update(float dTime)
{
	/*
	MercuryMatrix m = FindGlobalMatrix();

	std::list< MAutoPtr< MercuryAsset > >::iterator i;
	for (i = m_assets.begin(); i != m_assets.end(); ++i )
	{
		const BoundingVolume* v = (*i)->GetBoundingVolume();
		if (v)
		{
			BoundingVolume* nv = v->SpawnClone();
			nv->Transform(m);
			SAFE_DELETE(nv);
			//do some stuff to figure out if this node is culled
		}
	}
	*/
}

void RenderableNode::PreRender(const MercuryMatrix& matrix)
{
	list< MercuryAsset* >::iterator i;
	for (i = m_prerender.begin(); i != m_prerender.end(); ++i )
		(*i)->PreRender(this);
}

void RenderableNode::Render(const MercuryMatrix& matrix)
{
	list< MercuryAsset* >::iterator i;
	for (i = m_render.begin(); i != m_render.end(); ++i )
		(*i)->Render(this);
}

void RenderableNode::PostRender(const MercuryMatrix& matrix)
{
	list< MercuryAsset* >::iterator i;
	for (i = m_postrender.begin(); i != m_postrender.end(); ++i )
		(*i)->PostRender(this);
}

const MercuryMatrix& RenderableNode::FindGlobalMatrix() const
{
	MercuryNode* n = NULL;
	TransformNode* tn;
	for (n = Parent(); n; n = n->Parent())
	{
		tn = TransformNode::Cast(n);
		if ( tn )
			return tn->GetGlobalMatrix();
	}

	return MercuryMatrix::Identity();
}

void RenderableNode::AddPreRender(MercuryAsset* asset)
{
#ifdef MCHECKASSETS
	if ( !IsInAssetList(asset) ) //yell and scream
		assert(!"Asset does not exist in list!");
#endif
		
	m_prerender.push_back(asset);
}

void RenderableNode::AddRender(MercuryAsset* asset)
{
#ifdef MCHECKASSETS
	if ( !IsInAssetList(asset) ) //yell and scream
		assert(!"Asset does not exist in list!");
#endif
	
	m_render.push_back(asset);
}
void RenderableNode::AddPostRender(MercuryAsset* asset)
{
#ifdef MCHECKASSETS
	if ( !IsInAssetList(asset) ) //yell and scream
		assert(!"Asset does not exist in list!");
#endif
	
	m_postrender.push_back(asset);
}

bool RenderableNode::IsInAssetList( MercuryAsset* asset ) const
{
	std::list< MAutoPtr< MercuryAsset > >::const_iterator i;
	for (i = m_assets.begin(); i != m_assets.end(); ++i )
		if ( (*i) == asset ) return true;
	return false;
}

void RenderableNode::LoadFromXML(const XMLNode& node)
{	
	if ( !node.Attribute("hidden").empty() )
		m_hidden = node.Attribute("hidden")=="true"?true:false;
	
	for (XMLNode child = node.Child(); child.IsValid(); child = child.NextNode())
	{
		if ( child.Name() == "asset" )
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
	
	MercuryNode::LoadFromXML( node );
}

bool RenderableNode::IsCulled(const MercuryMatrix& matrix)
{
	bool clip = false;
	
	std::list< MAutoPtr< MercuryAsset > >::iterator i;
	for (i = m_assets.begin(); i != m_assets.end(); ++i )
	{
		const BoundingVolume* bv = (*i)->GetBoundingVolume();
		if (bv)
		{
			BoundingVolume* v = bv->SpawnClone();
			v->Transform( matrix );
			clip = v->Clip( *FRUSTUM );
			delete v;
			if ( clip == false ) return false;
		}
		else
			return false;
	}
	return clip;
}

MercuryMatrix RenderableNode::ManipulateMatrix(const MercuryMatrix& matrix)
{
	return VIEWMATRIX * matrix;
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
