#ifndef MERCURYASSET_H
#define MERCURYASSET_H

#include <MAutoPtr.h>
#include <MessageHandler.h>
#include <MercuryMatrix.h>
#include <BoundingBox.h>
#include <MSemaphore.h>
#include <XMLParser.h>
#include <Callback.h>
#include <MercuryLog.h>

#include <MercuryHash.h>
#include <list>

enum LoadState
{
	LOADING,
	LOADED,
	NONE
};

class MercuryNode;
class MercuryAssetInstance;

/* Assets are stored in renderable nodes with MAuto pointers.
The renderable nodes handle the memory management
*/
class MercuryAsset : public RefBase, public MessageHandler
{
	public:
		MercuryAsset();
		virtual ~MercuryAsset();
		
		virtual void Init();
		
		/** PreRender should be called before any real openGL render commands.
		It is used to handles things like frustum culling, and occlusion culling.
		Currently only occlusion culling test is run here.**/
		virtual void PreRender(const MercuryNode* node);
		
		virtual void Render(const MercuryNode* node) = 0;
		
		virtual void PostRender(const MercuryNode* node) {};
		
		///Loads an asset from an XMLAsset representing itself
		virtual void LoadFromXML(const XMLNode& node);

		///Saves the main body of an XML node.
		/** This behaves very similarly to MercuryNode::SaveToXML
		    The most notable difference is it doesn't handle children.*/
		virtual void SaveToXML( MString & sXMLStream, int depth = 0 );

		///Saves individual portions into the tag.
		virtual void SaveToXMLTag( MString & sXMLStream );

		virtual void LoadedCallback(); //thread safe
		
		inline void IsInstanced(bool b) { m_isInstanced = b; }

		inline BoundingVolume* GetBoundingVolume() const { return m_boundingVolume; }
		inline const MString& Path() const { return m_path; }

		///Retuns true if culled, also will initiate occlusion test
		virtual bool DoCullingTests(OcclusionResult& occlusion, const MercuryMatrix& matrix);
		void DrawAxes();
		
		inline void SetIgnoreCull(bool t) { m_ignoreCull = t; }
		inline bool IgnoreCull() const { return m_ignoreCull; }

		inline unsigned short GetPasses() { return m_iPasses; }
		inline void SetPasses( unsigned short p ) { m_iPasses = p; }
		
		virtual MercuryAssetInstance* GenerateInstanceData(MercuryNode* parentNode);

		LoadState GetLoadState(); //thread safe

		GENRTTI( MercuryAsset );
	protected:
		void SetLoadState(LoadState ls); //thread safe
		
		bool m_isInstanced;
		BoundingVolume* m_boundingVolume;
		MString m_path;
	private:
		LoadState m_loadState;
		MSemaphore m_lock;
		bool m_ignoreCull;
		unsigned short m_iPasses;
};

/** This holds the per-instance data for each asset instance.
Used in MercuryNode. */
class MercuryAssetInstance : public MessageHandler
{
	public:
		MercuryAssetInstance(MercuryAsset* asset, MercuryNode* parentNode);
		virtual ~MercuryAssetInstance() {};
		
		inline MercuryAsset& Asset() { return *m_asset; }
		inline const MercuryAsset& Asset() const { return *m_asset; }
		inline const MercuryAsset* AssetPtr() const { return m_asset; }
		
		inline bool Culled() const { return m_isCulled; }
		inline bool Culled(bool t) { m_isCulled = t; return m_isCulled; }
		
		inline OcclusionResult& GetOcclusionResult() { return m_occlusionResult; }

		inline unsigned short GetPasses() { return m_iPasses; }
		inline void SetPasses( unsigned short p ) { m_iPasses = p; }
	protected:
		MercuryNode* m_parentNode;
		MAutoPtr< MercuryAsset > m_asset; //actual asset storage
		
	private:
		OcclusionResult m_occlusionResult;
		bool m_isCulled;

		//We need to keep our own copy, since it may be changed in software from what the original one has.
		unsigned short m_iPasses;
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
		MercuryAsset* LocateAsset( const MString& key ) { MercuryAsset ** a = m_assetInstances.get( key ); return a?(*a):0; }

		std::list< std::pair< MString, Callback0R< MAutoPtr<MercuryAsset> > > > m_factoryCallbacks;
		
		//the actual storage point is in MercuryAssetInstance
		static MHash< MercuryAsset*> m_assetInstances;

};

#define ASSETFACTORY (AssetFactory::GetInstance())

static InstanceCounter<AssetFactory> AFcounter("AssetFactory");

#define REGISTER_ASSET_TYPE(class)\
	MAutoPtr<MercuryAsset> FactoryFunct##class() { return MAutoPtr<MercuryAsset>(class::Generate()); } \
	Callback0R< MAutoPtr<MercuryAsset> > factoryclbk##class( FactoryFunct##class ); \
	bool GlobalRegisterSuccess##class = AssetFactory::GetInstance().RegisterFactoryCallback(#class, factoryclbk##class);

#define ADD_ASSET_INSTANCE(class, key, ptr)\
		AssetFactory::GetInstance().AddAssetInstance( ToUpper(#class)+key, ptr );

#define REMOVE_ASSET_INSTANCE(class, key)\
		AssetFactory::GetInstance().RemoveAssetInstance( ToUpper(#class)+key );

#define CLASS_HELPERS(baseClass)\
		typedef baseClass base;

#endif

/****************************************************************************
 *   Copyright (C) 2009 - 2009 by Joshua Allen                              *
 *                                Charles Lohr                              *
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

