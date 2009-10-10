#include "Terrain.h"

REGISTER_ASSET_TYPE(Terrain);

Terrain::Terrain()
	:base()
{
}

Terrain::~Terrain()
{
}

Terrain* Terrain::Generate()
{
	LOG.Write( "new Terrain" );
	return new Terrain();
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
	
	printf("%f %f %f\n", xMax, yMax, zMax);
	m_hash.Allocate(xMax, yMax, zMax, 1);
	
	for(uint16_t i = 0; i < length; i+=3)
	{
		MercuryVertex v1(vertice+(indice[i]*HGMDLMesh::STRIDE));
		MercuryVertex v2(vertice+(indice[i+1]*HGMDLMesh::STRIDE));
		MercuryVertex v3(vertice+(indice[i+2]*HGMDLMesh::STRIDE));
		MTriangle t( v1, v2, v3 );
		
		m_hash.Insert( v1.GetX(), v1.GetY(), v1.GetZ(), t );
		m_hash.Insert( v2.GetX(), v2.GetY(), v2.GetZ(), t );
		m_hash.Insert( v3.GetX(), v3.GetY(), v3.GetZ(), t );
	}
}


