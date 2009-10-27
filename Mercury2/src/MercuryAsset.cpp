#include <MercuryAsset.h>
//#include <RenderableNode.h>
#include <MercuryNode.h>
#include <GLHeaders.h>

extern bool DOOCCLUSIONCULL;

MercuryAsset::MercuryAsset()
	:m_isInstanced(false), m_boundingVolume(NULL), m_loadState(NONE), m_ignoreCull(false), m_iPasses( DEFAULT_PASSES )
{
}

MercuryAsset::~MercuryAsset()
{
	SAFE_DELETE(m_boundingVolume);
}

void MercuryAsset::Init()
{
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

bool MercuryAsset::DoCullingTests(OcclusionResult& occlusion, const MercuryMatrix& matrix)
{
	bool culled = false;
	if ( m_boundingVolume )
	{
		culled = m_boundingVolume->DoFrustumTest(matrix);
		if ( !culled && DOOCCLUSIONCULL) m_boundingVolume->DoOcclusionTest( occlusion );
	}
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
	
	if ( !node.Attribute("ignorecull").empty() )
	{
		SetIgnoreCull( StrToBool( node.Attribute("ignorecull") ) );
	}
	if ( !node.Attribute("setPasses").empty() )
	{
		MVector< MString > out;
		SplitStrings( node.Attribute("setPasses"), out, ",+", " \t", 2, 2 );
		m_iPasses = 0;
		for( unsigned i = 0; i < out.size(); i++ )
			m_iPasses = m_iPasses | (1<<StrToInt( out[i] ) );
	}
}

void MercuryAsset::SaveToXML( MString & sXMLStream, int depth )
{
	sXMLStream += ssprintf( "%*c<node type=\"%s\" ", depth*3, 32, GetType() );
	if( m_path.length() )
		sXMLStream += ssprintf( "file=\"%s\" ", m_path.c_str() );

	SaveToXMLTag( sXMLStream );
	sXMLStream += "/>\n";
}

void MercuryAsset::SaveToXMLTag( MString & sXMLStream )
{
	//Assets, generally do not actually have anything else to save...
}


void MercuryAsset::DrawAxes()
{
	GLCALL( glBegin(GL_LINES) );
	GLCALL( glColor3f(1,0,0) );
	GLCALL( glVertex3f(0,0,0) );
	GLCALL( glVertex3f(0.5,0,0) );
	GLCALL( glColor3f(0,1,0) );
	GLCALL( glVertex3f(0,0,0) );
	GLCALL( glVertex3f(0,0.5,0) );
	GLCALL( glColor3f(0,0,1) );
	GLCALL( glVertex3f(0,0,0) );
	GLCALL( glVertex3f(0,0,0.5) );
	GLCALL( glColor3f(1,1,1) );
	GLCALL( glEnd() );
}

MercuryAssetInstance* MercuryAsset::GenerateInstanceData(MercuryNode* parentNode)
{
	return new MercuryAssetInstance(this, parentNode);
}

MercuryAssetInstance::MercuryAssetInstance(MercuryAsset* asset, MercuryNode* parentNode)
	:m_parentNode(parentNode), m_asset( asset ), m_isCulled( false ), m_iPasses( asset->GetPasses() )
{
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
	LOG.Write( "WARNING: Asset type " + type + " not found." );
	return NULL;
}

void AssetFactory::AddAssetInstance(const MString& key, MercuryAsset* asset)
{
	asset->IsInstanced(true);
	m_assetInstances[key] = asset;
}

void AssetFactory::RemoveAssetInstance(const MString& key)
{
	m_assetInstances.remove( key );
	LOG.Write( "removed asset "+key );
}

MHash< MercuryAsset*> AssetFactory::m_assetInstances;

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
