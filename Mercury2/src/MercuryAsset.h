#ifndef MERCURYASSET_H
#define MERCURYASSET_H

#include <MAutoPtr.h>
#include <MessageHandler.h>
#include <MercuryMatrix.h>
#include <BoundingBox.h>
#include <MSemaphore.h>
#include <XMLParser.h>
#include <Callback.h>

#include <map>
#include <list>

enum LoadState
{
	LOADING,
	LOADED,
	NONE
};

class MercuryNode;

/* Assets are stored in renderable nodes with MAuto pointers.
The renderable nodes handle the memory management
*/
class MercuryAsset : public RefBase, MessageHandler
{
	public:
		MercuryAsset();
		virtual ~MercuryAsset();
		
		virtual void Init(MercuryNode* node);
		
		/** PreRender should be called before any real openGL render commands.
		It is used to handles things like frustum culling, and occlusion culling.
		Currently only occlusion culling test is run here.**/
		virtual void PreRender(const MercuryNode* node);
		
		virtual void Render(const MercuryNode* node) = 0;
		
		virtual void PostRender(const MercuryNode* node) {};
		
		///Loads an asset from an XMLAsset representing itself
		virtual void LoadFromXML(const XMLNode& node);
		
		virtual void LoadedCallback(); //thread safe
		
		inline void IsInstanced(bool b) { m_isInstanced = b; }

		inline BoundingVolume* GetBoundingVolume() const { return m_boundingVolume; }
		inline const MString& Path() const { return m_path; }

		///Retuns true if culled, also will initiate occlusion test
		virtual bool DoCullingTests(MercuryNode* n, const MercuryMatrix& matrix);
		void DrawAxes();
		
		inline void SetExcludeFromCull(bool t) { m_excludeFromCull = t; }
		inline bool ExcludeFromCull() const { return m_excludeFromCull; }
		
		inline bool IsCulled() const { return m_culled; }
	protected:
		void SetLoadState(LoadState ls); //thread safe
		LoadState GetLoadState(); //thread safe
		
		bool m_isInstanced;
		BoundingVolume* m_boundingVolume;
		MString m_path;
		bool m_culled;
	private:
		LoadState m_loadState;
		MSemaphore m_lock;
		bool m_excludeFromCull;
};

class LoaderThreadData
{
	public:
		LoaderThreadData(MercuryAsset* a)
		{
			asset = a;
		}
		
		//use and autoptr here to prevent crashes if asset is removed during load
		MAutoPtr< MercuryAsset > asset;
};

class AssetFactory
{
	public:
		static AssetFactory& GetInstance();
		bool RegisterFactoryCallback(const MString& type, Callback0R< MAutoPtr<MercuryAsset> >);
		MAutoPtr<MercuryAsset> Generate(const MString& type, const MString& key);
		
		void AddAssetInstance(const MString& key, MercuryAsset* asset);
		void RemoveAssetInstance(const MString& key);

	private:
		MercuryAsset* LocateAsset( const MString& key );

		std::list< std::pair< MString, Callback0R< MAutoPtr<MercuryAsset> > > > m_factoryCallbacks;
		
		//the actual storage point is in renderable nodes
		static std::map<MString, MercuryAsset*> m_assetInstances;

};

static InstanceCounter<AssetFactory> AFcounter("AssetFactory");

#define REGISTER_ASSET_TYPE(class)\
	MAutoPtr<MercuryAsset> FactoryFunct##class() { return MAutoPtr<MercuryAsset>(class::Generate()); } \
	Callback0R< MAutoPtr<MercuryAsset> > factoryclbk##class( FactoryFunct##class ); \
	bool GlobalRegisterSuccess##class = AssetFactory::GetInstance().RegisterFactoryCallback(#class, factoryclbk##class);

#define ADD_ASSET_INSTANCE(class, key, ptr)\
		AssetFactory::GetInstance().AddAssetInstance( ToUpper(#class)+key, ptr );

#define REMOVE_ASSET_INSTANCE(class, key)\
		AssetFactory::GetInstance().RemoveAssetInstance( ToUpper(#class)+key );

#endif

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
