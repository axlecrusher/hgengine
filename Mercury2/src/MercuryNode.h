#ifndef MERCURYNODE_H
#define MERCURYNODE_H

#include <list>
#include <Callback.h>
#include <typeinfo>
#include <XMLParser.h>
#include <MercuryUtil.h>
#include <MessageHandler.h>

/** This is the basic node of the scene graph.  It is not intended to be instanced.
	Each node exists as a single entity in the scene graph.
**/

#define GENRTTI(x) static const x* Cast(const MercuryNode* n) \
{ if (n==NULL) return NULL; return dynamic_cast<const x*>(n); } \
static x* Cast(MercuryNode* n) \
{ if (n==NULL) return NULL; return dynamic_cast<x*>(n); }

/*
#define GENRTTI(x) static bool IsMyType(const MercuryNode* n) \
{ const MercuryNode* tn = n; \
while(tn) { if (typeid(x) == typeid(*tn)) return true; tn = *n; } \
return false;}
*/
class MercuryNode : public MessageHandler
{
	public:
		MercuryNode();
		virtual ~MercuryNode();
		
		void AddChild(MercuryNode* n);
		void RemoveChild(MercuryNode* n);
		
		inline MercuryNode* Parent() const { return m_parent; }
		inline MercuryNode* NextSibling() const { return m_nextSibling; }
		inline MercuryNode* PrevSibling() const { return m_prevSibling; }
		MercuryNode* FirstChild() const;
		MercuryNode* NextChild(const MercuryNode* n) const; ///Finds the next child in regards to n
		MercuryNode* PrevChild(const MercuryNode* n) const; ///Finds the previous child in regards to n
		const std::list< MercuryNode* >& Children() const { return m_children; }
		
		virtual void Update(float dTime) {};
		void RecursiveUpdate(float dTime);
		void ThreadedUpdate(float dTime);
		
		///Run on parent when a child is added
		virtual void OnAddChild() {};
		
		///Run on parent when a child is added
		virtual void OnRemoveChild() {};
		
		///Loads a node from an XMLNode representing itself
		virtual void LoadFromXML(const XMLNode& node);
		
		///Run on a child when added to a parent
		virtual void OnAdded() {};
	
		///Run on a child when removed from a parent
		virtual void OnRemoved() {};

		GENRTTI(MercuryNode);
		
		inline static bool NeedsRebuild() { bool t=m_rebuildRenderGraph; m_rebuildRenderGraph = false; return t; }
	
	protected:
		std::list< MercuryNode* > m_children;	//These nodes are unique, not instanced
		MercuryNode* m_parent;
		MercuryNode* m_prevSibling;
		MercuryNode* m_nextSibling;

		static bool m_rebuildRenderGraph;

};

class NodeFactory
{
	public:
		static NodeFactory& GetInstance();
		bool RegisterFactoryCallback(const MString& type, Callback0R<MercuryNode*>);
		MercuryNode* Generate(const MString& type);
	
	private:
		std::list< std::pair< MString, Callback0R<MercuryNode*> > > m_factoryCallbacks;
};

static InstanceCounter<NodeFactory> NFcounter("NodeFactory");

#define REGISTER_NODE_TYPE(class)\
	MercuryNode* FactoryFunct##class() { return new class(); } \
	Callback0R<MercuryNode*> factoryclbk##class( FactoryFunct##class ); \
	bool GlobalRegisterSuccess##class = NodeFactory::GetInstance().RegisterFactoryCallback(#class, factoryclbk##class);

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
