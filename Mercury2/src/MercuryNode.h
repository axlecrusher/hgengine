#ifndef MERCURYNODE_H
#define MERCURYNODE_H

#include <list>
#include <Callback.h>

/** This is the basic node of the scene graph.  It is not intended to be instanced.
	Each node exists as a single entity in the scene graph.
**/

class MercuryNode;

typedef Callback2< MercuryNode*, MercuryNode* > OnAddChild;
typedef Callback2< MercuryNode*, MercuryNode* > OnRemoveChild;

class MercuryNode
{
	public:
		MercuryNode();
		virtual ~MercuryNode();
		
		void AddChild(MercuryNode* n);
		void RemoveChild(MercuryNode* n);
		
		MercuryNode* Parent();
		MercuryNode* NextSibling();
		MercuryNode* PrevSibling();
		MercuryNode* NextChild(MercuryNode* n); ///Finds the next child in regards to n
		MercuryNode* PrevChild(MercuryNode* n); ///Finds the previous child in regards to n
		
		virtual void Update(float dTime) = 0;
		void RecursiveUpdate(float dTime);
		
		///Provides callback ability when a child node is added (parent, child) arguement order
		std::list< Callback2< MercuryNode*, MercuryNode* > > OnAddChild;
		
		///Provides callback ability when a child node is removed (parent, child) arguement order
		std::list< Callback2< MercuryNode*, MercuryNode* > > OnRemoveChild;
	
	private:
		MercuryNode* m_parent;
		std::list< MercuryNode* > m_children;
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
