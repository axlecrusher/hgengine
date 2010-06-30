#include "Terrain.h"
#include <MercuryMessageManager.h>
#include <MercuryNode.h>

REGISTER_ASSET_TYPE(Terrain);

Terrain::Terrain( const MString & key, bool bInstanced )
	: base( key, bInstanced )
{
}

Terrain::~Terrain()
{
}

void Terrain::LoadedCallback()
{
	BuildHash();
	base::LoadedCallback();
}

void Terrain::BuildHash()
{
	for( unsigned int i = 0; i < m_meshes.size(); ++i)
	{
		MAutoPtr< HGMDLMesh > mesh = m_meshes[i];
		const HGMDLMesh& m = *mesh;
		ImportMeshToHash( m );
	}
}

bool Terrain::ChangeKey( const MString & sNewKey )
{
	HGMDLModel::ChangeKey( sNewKey );
	if( GetLoadState() == LOADED )
		BuildHash();

	return true;
}


void Terrain::ImportMeshToHash(const HGMDLMesh& mesh)
{
	const float* vertice = mesh.GetVertexHandle() + MercuryVBO::VERTEX_OFFSET;
	const short unsigned int* indice = mesh.GetIndexHandle();
	
	uint16_t length = mesh.IndiceCount();
	
	float xMax, yMax, zMax;
	xMax = yMax = zMax = 0;
	
	for(uint16_t i = 0; i < length; ++i)
	{
		MercuryVertex v(vertice+(indice[i]*HGMDLMesh::STRIDE));
		xMax = MAX<float>(v.GetX(),xMax);
		yMax = MAX<float>(v.GetY(),yMax);
		zMax = MAX<float>(v.GetZ(),zMax);
	}
	m_hash.Allocate(10, 1);
	
	for(uint16_t i = 0; i < length; i+=3)
	{
		MercuryVertex v1(vertice+(indice[i]*HGMDLMesh::STRIDE), 0);
		MercuryVertex v2(vertice+(indice[i+1]*HGMDLMesh::STRIDE), 0);
		MercuryVertex v3(vertice+(indice[i+2]*HGMDLMesh::STRIDE), 0);
		MTriangle t( v1, v2, v3 );
		
		m_hash.Insert( v1.GetX(), v1.GetY(), v1.GetZ(), t );
		m_hash.Insert( v2.GetX(), v2.GetY(), v2.GetZ(), t );
		m_hash.Insert( v3.GetX(), v3.GetY(), v3.GetZ(), t );
	}
}

MercuryVertex Terrain::ComputePositionLinear(const MercuryVertex& p)
{
	HGMDLMesh& mesh = *m_meshes[0];
	const float* vertice = mesh.GetVertexHandle() + MercuryVBO::VERTEX_OFFSET;
	const short unsigned int* indice = mesh.GetIndexHandle();
	
	MercuryVertex l( p );
	l[2] = 0;
	
	MercuryVector result = l;
	
	uint16_t length = mesh.IndiceCount();
	uint16_t foundCount = 0;
	for(uint16_t i = 0; i < length; i+=3)
	{
		MercuryVertex v1(vertice+(indice[i]*HGMDLMesh::STRIDE));
		MercuryVertex v2(vertice+(indice[i+1]*HGMDLMesh::STRIDE));
		MercuryVertex v3(vertice+(indice[i+2]*HGMDLMesh::STRIDE));
		
		MTriangle t( v1, v2, v3 );
		MTriangle tflat(t);
		
		tflat.m_verts[0][2] = 0;
		tflat.m_verts[1][2] = 0;
		tflat.m_verts[2][2] = 0;

		if ( tflat.IsInTriangle(l) )
		{
			++foundCount;
			MercuryVector b = tflat.Barycentric(l);
			
			MercuryVector pos( t.InterpolatePosition(b) );
			result = pos;
//			return result;
		}
	}
	
	if (foundCount > 1) LOG.Write( ssprintf("!!!!!! Found %d triangles !!!!!!", foundCount) );
	return result;
}

MercuryVertex Terrain::ComputePosition(const MercuryVertex& p)
{
	//discard camera height
	MercuryVertex l( p );
	l[2] = 0;
	
	std::list<MTriangle> triangles = m_hash.FindByXY(p[0], p[1]);
	
	MercuryVector result = l;

	if (!triangles.empty())
	{
		std::list<MTriangle>::iterator i = triangles.begin();
		for(;i != triangles.end(); ++i)
		{
			//comput against a flat triangle since we don't care if we are at the correct height.
			MTriangle flatTriangle = *i;
			flatTriangle.m_verts[0][2] = 0;
			flatTriangle.m_verts[1][2] = 0;
			flatTriangle.m_verts[2][2] = 0;
			if ( flatTriangle.IsInTriangle(l) )
			{
				MercuryVector b = flatTriangle.Barycentric(l);
				MercuryVector pos( i->InterpolatePosition(b) );
				return pos;
			}
		}
	}
	
	return result;
}

MercuryAssetInstance * Terrain::MakeAssetInstance( MercuryNode * ParentNode )
{
	return new TerrainAssetInstance( this, ParentNode );
}


TerrainAssetInstance::TerrainAssetInstance(MercuryAsset* asset, MercuryNode* parentNode)
	:base(asset, parentNode)
{
	REGISTER_FOR_MESSAGE("QueryTerrainPoint");
}

TerrainAssetInstance::~TerrainAssetInstance()
{
	UNREGISTER_FOR_MESSAGE("QueryTerrainPoint");
}

void TerrainAssetInstance::HandleMessage(const MString& message, const MessageData& data)
{
	if (message == "QueryTerrainPoint")
	{
		if( Asset().GetLoadState() == LOADING )
		{
			POST_MESSAGE( "QueryTerrainPoint", new VertexDataMessage( dynamic_cast<const VertexDataMessage&>(data) ), 0.0001f );
			return;
		}
		const VertexDataMessage& v( dynamic_cast<const VertexDataMessage&>(data) );
		
		//compute local space position
		MercuryMatrix m = m_parentNode->GetGlobalMatrix();
		m.Transpose();
		MercuryVertex local = m * v.Vertex;
		local[3] = 1; //no W
		
		Terrain* t = (Terrain*)m_asset.Ptr();
//		local = t->ComputePositionLinear( local );
		local = t->ComputePosition( local );
		local[2] += 0.75; //height of player
		
		local = m_parentNode->GetGlobalMatrix() * local;
		POST_MESSAGE("SetCameraPosition", new VertexDataMessage(local), 0);
	}
}


