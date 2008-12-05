#include <MercuryAsset.h>
#include <RenderableNode.h>

void MercuryAsset::Init(MercuryNode* node)
{
	RenderableNode* rn;
	if ( (rn=RenderableNode::Cast( node )) )
		rn->AddRender(this);
}

AssetFactory& AssetFactory::GetInstance()
{
	static AssetFactory* instance = NULL;
	if (!instance)
		instance = new AssetFactory;
	return *instance;
}

bool AssetFactory::RegisterFactoryCallback(const std::string& type, Callback0R< MAutoPtr<MercuryAsset> > functor)
{
	string t = ToUpper( type );
	std::pair<std::string, Callback0R< MAutoPtr<MercuryAsset> > > pp(t, functor);
	m_factoryCallbacks.push_back( pp );
	return true;
}

MAutoPtr<MercuryAsset> AssetFactory::Generate(const std::string& type)
{
	string t = ToUpper( type );
	std::list< std::pair< std::string, Callback0R< MAutoPtr<MercuryAsset> > > >::iterator i;
	for (i = m_factoryCallbacks.begin(); i != m_factoryCallbacks.end(); ++i)
		if (i->first == t) return i->second();
	return NULL;
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
