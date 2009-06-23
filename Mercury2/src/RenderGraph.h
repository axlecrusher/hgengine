#ifndef RENDERGRAPH_H
#define RENDERGRAPH_H

#include <MercuryNode.h>
//#include <RenderableNode.h>
#include <PriorityQueue.h>

class RenderGraphEntry
{
	friend class RenderGraph;
	public:
		RenderGraphEntry()
		{
			m_node = NULL;
			m_matrix = &MercuryMatrix::Identity();
		}
		
		RenderGraphEntry(const MercuryMatrix* matrix, MercuryNode* node)
			:m_node(node), m_matrix(matrix)
		{}
		
		void AddChild(RenderGraphEntry entry);
		void Render();
	private:
		MercuryNode* m_node; //we don't own this, no new or free
		std::list< RenderGraphEntry > m_children;
		const MercuryMatrix* m_matrix;
};

/** Tries to get pointers to all assets needed to rebuild an accurate render state.
This will only work for restoring the render state within the same render loop.
**/
class StoreRenderState
{
	public:
		StoreRenderState();
		void Save();
		MercuryNode* Node;
		MercuryMatrix Matrix;
//	private:
		std::list< MercuryAsset* > Assets;
};

class RenderGraph
{
	public:
		void Build( MercuryNode* node );
		inline void Render() { m_root.Render(); }
		
		void AddAlphaNode( MercuryNode* node );
		void RenderAlpha();
	private:
		void Build( MercuryNode* node, RenderGraphEntry& entry );
		RenderGraphEntry m_root;
		
		//nodes that use alpha, ordered from farthest to nearest from the camera
		PriorityQueue<float, StoreRenderState > m_alphaNodesQueue;
};

extern RenderGraph* CURRENTRENDERGRAPH;

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
