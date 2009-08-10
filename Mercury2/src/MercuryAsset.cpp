#include <MercuryAsset.h>
//#include <RenderableNode.h>
#include <MercuryNode.h>
#include <GLHeaders.h>

extern bool DOOCCLUSIONCULL;

MercuryAsset::MercuryAsset()
	:m_isInstanced(false), m_boundingVolume(NULL), m_loadState(NONE), m_culled(false)
{
}

MercuryAsset::~MercuryAsset()
{
	SAFE_DELETE(m_boundingVolume);
}

void MercuryAsset::Init(MercuryNode* node)
{
//	RenderableNode* rn;
	if ( node ) node->AddPreRender(this);
	if ( node ) node->AddRender(this);
}

void MercuryAsset::SetLoadState(LoadState ls)
{
	MSemaphoreLock lock( &m_lock );
	m_loadState = ls;
}

LoadState MercuryAsset::GetLoadState()
{
	MSemaphoreLock lock( &m_lock );
	return m_loadState;
}

void MercuryAsset::LoadedCallback()
{
	SetLoadState( LOADED );
}

bool MercuryAsset::DoCullingTests(MercuryNode* n, const MercuryMatrix& matrix)
{
	bool culled = false;
	if ( m_boundingVolume )
	{
		culled = m_boundingVolume->DoFrustumTest(matrix);
		if ( !culled && DOOCCLUSIONCULL)
			m_boundingVolume->DoOcclusionTest( n->GetOcclusionResult() );
	}
	m_culled = culled;
	return culled;
}

void MercuryAsset::PreRender(const MercuryNode* node)
{
	/*
	MercuryNode* n = const_cast<MercuryNode*>(node);
	if ( m_boundingVolume )
	{
//		n->SetCulled( m_boundingVolume->DoFrustumTest(matrix) );
		if ( !n->IsCulled() )
			m_boundingVolume->DoOcclusionTest( n->GetOcclusionResult() );
	}
	*/
}

void MercuryAsset::LoadFromXML(const XMLNode& node)
{
	
	if ( !node.Attribute("nocull").empty() )
	{
		SetExcludeFromCull( node.Attribute("nocull")=="true" );
	}
}

void MercuryAsset::DrawAxes()
{
	glBegin(GL_LINES);
	glColor3f(1,0,0);
	glVertex3f(0,0,0);
	glVertex3f(0.5,0,0);
	glColor3f(0,1,0);
	glVertex3f(0,0,0);
	glVertex3f(0,0.5,0);
	glColor3f(0,0,1);
	glVertex3f(0,0,0);
	glVertex3f(0,0,0.5);
	glColor3f(1,1,1);
	glEnd();
}

AssetFactory& AssetFactory::GetInstance()
{
	static AssetFactory* instance = NULL;
	if (!instance)
		instance = new AssetFactory;
	return *instance;
}

bool AssetFactory::RegisterFactoryCallback(const MString & type, Callback0R< MAutoPtr<MercuryAsset> > functor)
{
	MString t = ToUpper( type );
	std::pair<MString , Callback0R< MAutoPtr<MercuryAsset> > > pp(t, functor);
	m_factoryCallbacks.push_back( pp );
	return true;
}

MAutoPtr<MercuryAsset> AssetFactory::Generate(const MString& type, const MString& key)
{
	MString t = ToUpper( type );
	
	MercuryAsset *asset = LocateAsset(t+key);
	if ( asset ) return asset;
	
	std::list< std::pair< MString, Callback0R< MAutoPtr<MercuryAsset> > > >::iterator i;
	for (i = m_factoryCallbacks.begin(); i != m_factoryCallbacks.end(); ++i)
		if (i->first == t) return i->second();
	printf("WARNING: Asset type %s not found.\n", type.c_str());
	return NULL;
}

MercuryAsset* AssetFactory::LocateAsset( const MString& key )
{
	std::map<MString, MercuryAsset*>::iterator asset = m_assetInstances.find(key);
	if ( asset != m_assetInstances.end() ) return asset->second;
	return NULL;
}

void AssetFactory::AddAssetInstance(const MString& key, MercuryAsset* asset)
{
	asset->IsInstanced(true);
	m_assetInstances[key] = asset;
}

void AssetFactory::RemoveAssetInstance(const MString& key)
{
	std::map<MString, MercuryAsset*>::iterator asset = m_assetInstances.find(key);
	if ( asset != m_assetInstances.end() )
	{
		m_assetInstances.erase( asset );
		printf("removed asset %s\n", key.c_str());
	}
	
}

std::map<MString, MercuryAsset*> AssetFactory::m_assetInstances;

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
