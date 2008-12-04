#ifndef MERCURYASSET_H
#define MERCURYASSET_H

#include <MAutoPtr.h>
#include <MercuryNode.h>

class MercuryAsset : public RefBase
{
	public:
		virtual ~MercuryAsset() {};
		
		virtual void PreRender(MercuryNode* node) {};
		virtual void Render(MercuryNode* node) = 0;
		virtual void PostRender(MercuryNode* node) {};
		
		///Loads a node from an XMLNode representing itself
		virtual void LoadFromXML(const XMLNode& node) {};
};

class AssetFactory
{
	public:
		static AssetFactory& GetInstance();
		bool RegisterFactoryCallback(const std::string& type, Callback0R< MAutoPtr<MercuryAsset> >);
		MAutoPtr<MercuryAsset> Generate(const std::string& type);
	
	private:
		std::list< std::pair< std::string, Callback0R< MAutoPtr<MercuryAsset> > > > m_factoryCallbacks;
};

static InstanceCounter<AssetFactory> AFcounter("AssetFactory");

#define REGISTER_ASSET_TYPE(class)\
	MAutoPtr<MercuryAsset> FactoryFunct##class() { return MAutoPtr<MercuryAsset>(new class()); } \
	Callback0R< MAutoPtr<MercuryAsset> > factoryclbk##class( FactoryFunct##class ); \
	bool GlobalRegisterSuccess##class = AssetFactory::GetInstance().RegisterFactoryCallback(#class, factoryclbk##class);


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
