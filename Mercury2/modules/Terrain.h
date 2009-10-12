#ifndef TERRAIN_H
#define TERRAIN_H

#include <MercuryAsset.h>
#include <HGMDLModel.h>
#include <MercuryFile.h>

#include <SpatialHash.h>
#include <MTriangle.h>

class Terrain : public HGMDLModel
{
	public:
		Terrain();
		~Terrain();

		static Terrain* Generate();
		virtual void LoadedCallback(); //thread safe
	
		virtual MercuryAssetInstance* GenerateInstanceData(MercuryNode* parentNode);
		MercuryVertex ComputePosition(const MercuryVertex& p);
		MercuryVertex ComputePositionLinear(const MercuryVertex& p);
		
	private:
		CLASS_HELPERS( HGMDLModel );
		void BuildHash();
		void ImportMeshToHash(const HGMDLMesh& mesh);
		
		SpatialHash<MTriangle> m_hash;
//		void LoadHGMDL( const MString& path );
//		static void* LoaderThread(void* d);
		
//		std::vector< MAutoPtr< HGMDLMesh > > m_meshes;
};

class TerrainAssetInstance : public MercuryAssetInstance
{
	public:
		TerrainAssetInstance(MercuryAsset* asset, MercuryNode* parentNode);
		~TerrainAssetInstance();
		virtual void HandleMessage(const MString& message, const MessageData* data);
	private:
		CLASS_HELPERS( MercuryAssetInstance );
};

#endif

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
