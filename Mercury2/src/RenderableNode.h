#ifndef RENDERABLENODE_H
#define RENDERABLENODE_H

#include <MercuryNode.h>
#include <MAutoPtr.h>
#include <MercuryAsset.h>
#include <MercuryMatrix.h>
#include <MSemaphore.h>

#define MCHECKASSETS

class RenderableNode : public MercuryNode
{
	public:
		RenderableNode();
		~RenderableNode();
		
		virtual void Render();
		virtual void Update(float dTime);
				
		///Returnes true if N is of type RenderableNode
//		static bool IsMyType( MercuryNode* n );
		
		inline void AddAsset(MAutoPtr< MercuryAsset > asset) { m_assets.push_back(asset); }
		
		void AddPreRender(MercuryAsset* asset);
		void AddRender(MercuryAsset* asset);
		void AddPostRender(MercuryAsset* asset);
		
		static void RecursiveRender( MercuryNode* n );
		virtual void LoadFromXML(const XMLNode& node);
		
		const MercuryMatrix& FindGlobalMatrix() const;
		
		virtual bool IsCulled() { return false; }

		GENRTTI(RenderableNode);
	protected:
		bool m_hidden;
	private:
		bool IsInAssetList(MercuryAsset* asset) const;
		inline void Spinlock( unsigned long value ) { while (m_semaphore.ReadValue() != value); }
		
		std::list< MAutoPtr< MercuryAsset > > m_assets;	///serves as a holder for memory
		
		//we will just use normal pointers here because we don't want to waste too much time
		//dereferencing the autopointer. As a precaution when assets are added to these lists,
		//they must exist in m_assets.
		std::list< MercuryAsset* > m_prerender;
		std::list< MercuryAsset* > m_render;
		std::list< MercuryAsset* > m_postrender;
		
		MSemaphore m_semaphore;
};

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
