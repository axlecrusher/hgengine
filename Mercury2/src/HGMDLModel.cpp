#include <HGMDLModel.h>

REGISTER_ASSET_TYPE(HGMDLModel);

HGMDLModel::HGMDLModel()
	:MercuryAsset()
{
}

HGMDLModel::~HGMDLModel()
{
	REMOVE_ASSET_INSTANCE(HGMDLModel, m_path);
}

void HGMDLModel::LoadFromXML(const XMLNode& node)
{
	MString path = node.Attribute("file");
	LoadHGMDL( path );
}

void HGMDLModel::LoadModel(MercuryFile* hgmdl)
{
	char fingerPrint[5];
	fingerPrint[4] = 0;

	hgmdl->Read( fingerPrint, 4 );

	MString p(fingerPrint);
	if (p != "MBMF")
	{
		printf("Not a HGMDL file.\n");
		return;
	}
	
	uint32_t version;
	//fread(&version, 4, 1, hgmdl);
	hgmdl->Read( &version, 4 );
	
	if (version != 200)
	{
		printf("Invalid HGMDL version %d\n", version);
		return;
	}
	
	uint16_t numMeshes;

	hgmdl->Read( &numMeshes, sizeof( uint16_t ) );
	for (uint16_t i = 0; i < numMeshes; ++i)
	{
		MAutoPtr< HGMDLMesh > mesh( new HGMDLMesh() );
		mesh->LoadFromFile( hgmdl );
		m_meshes.push_back(mesh);
	}
}

void HGMDLModel::Render(MercuryNode* node)
{
	for(uint16_t i = 0; i < m_meshes.size(); ++i)
		m_meshes[i]->Render(node);
}

void HGMDLModel::LoadHGMDL( const MString& path )
{
	if ( m_isInstanced ) return;
	if ( !path.empty() )
	{
		ADD_ASSET_INSTANCE(HGMDLModel, path, this);
		m_path = path;
		MercuryFile * f = FILEMAN.Open( path );
		if( !f )
		{
			printf( "Could not open file: \"%s\" for model\n", path.c_str() );
			return;
		}
		LoadModel( f );
		delete f;
	}
}

HGMDLModel* HGMDLModel::Generate()
{
	printf("new HGMDL\n");
	return new HGMDLModel();
}

/****************************************************************************
 *   Copyright (C) 2008 by Joshua Allen                                     *
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
