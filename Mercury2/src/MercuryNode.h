#ifndef MERCURYNODE_H
#define MERCURYNODE_H

#include <list>
#include <Callback.h>
#include <typeinfo>
#include <XMLParser.h>
#include <MercuryUtil.h>
#include <MessageHandler.h>
#include <MercuryLog.h>


#include <MercuryAsset.h>
#include <BoundingBox.h>

/** This is the basic node of the scene graph.  It is not intended to be instanced.
	Each node exists as a single entity in the scene graph.
**/

#include <Shader.h>

#define STANDARD_PASS	7
///Which passes, by default, should be run on all nodes.
#define DEFAULT_PASSES ( (1<<STANDARD_PASS) )

///The Global Viewport ID for this thread (to enable multi-threaded functioning for Viewports)
extern __ThreadLocalStore int g_iViewportID;

///The Global Pass Number (which Pass is currently doing Render)
extern __ThreadLocalStore int g_iPass;

enum MercuryNodeFlagMask
{
	HIDDEN = 1,
	CULLED,
	SAVECHILDREN,
	ENABLESAVE,
	ALPHAPATH
};

class MercuryNode : public MessageHandler
{
	public:
		MercuryNode();
		virtual ~MercuryNode();
		
		virtual void Init() {};
		
		virtual void AddChild(MercuryNode* n);
		virtual void RemoveChild(MercuryNode* n);
				
		inline MercuryNode* Parent() const { return m_parent; }
		inline MercuryNode* NextSibling() const { return m_nextSibling; }
		inline MercuryNode* PrevSibling() const { return m_prevSibling; }
		MercuryNode* FirstChild() const;
		MercuryNode* LastChild() const;
		MercuryNode* NextChild(const MercuryNode* n) const; ///Finds the next child in regards to n
		MercuryNode* PrevChild(const MercuryNode* n) const; ///Finds the previous child in regards to n
		const std::list< MercuryNode* >& Children() const { return m_children; }

		///Find a child node that has the name matching sNameOfNode.
		/** The search order is breadth-first, however this may change without notice! */
		MercuryNode* FindChild( const MString & sNameOfNode, int depth = MAXINT );

		///Find a parent node that has the name matching sNameOfNode.
		/** Traversal is from the closest parent on upward */
		MercuryNode* FindParent( const MString & sNameOfNode, int depth = MAXINT );

		///Get the next node in an in-order traversal
		/** In the traversal, stopnode indicates the node that when passing
		    by on returning up the tree haults traversal. */
		MercuryNode * TraversalNextNode( MercuryNode * stopnode, int & iDepthDelta );

		virtual void Update(float dTime) {};
		virtual void RecursiveUpdate(float dTime, const MercuryMatrix& globalMatrix);
		void ThreadedUpdate(float dTime);
		
		
		virtual void RecursivePreRender();
		virtual void RecursiveRender();

		///Run on parent when a child is added
		virtual void OnAddChild() {};
		
		///Run on parent when a child is added
		virtual void OnRemoveChild() {};
		
		///Loads a node from an XMLNode representing itself
		virtual void LoadFromXML(const XMLNode& node);

		///Saves the main body of an XML node.
		/** This, in the base class sets up the stream (i.e. <node and invokes the
		    SaveToXML tag then outputs the termination  (i.e. /> or > ) it then outputs
		    all of the various Assets and Nodes that are attached as children.  It handles
		    closing the argument as well.  Unless you intend to modify the behavior, do not
		    override this.  In addition, it is generally best to completely re-write it if
		    you intend to modify the behavior. */
		virtual void SaveToXML( MString & sXMLStream, int depth = 0 );

		///Saves just the tag portion of an XML node.
		/** The update process here is tricky.  This is invoked by the base
		    MercuryNode class.  It does not handle setting it up, i.e. type=... name=...
		    The various abstracted classes must append on arguments they require. */
		virtual void SaveToXMLTag( MString & sXMLStream );

		void SaveBaseXMLTag( MString & sXMLStream );

		///Run on a child when added to a parent
		virtual void OnAdded() {};
	
		///Run on a child when removed from a parent
		virtual void OnRemoved() {};

		GENRTTI(MercuryNode);
		
		inline static bool NeedsRebuild() { bool t=m_rebuildRenderGraph; m_rebuildRenderGraph = false; return t; }
		inline void SetName(const MString& name) { m_name = name; }
		inline MString GetName() const { return m_name; }
		
		void AddAsset(MercuryAsset* asset);
		void ClearAssets();
		
		virtual void PreRender(const MercuryMatrix& matrix);
		virtual void Render(const MercuryMatrix& matrix);
		virtual void PostRender(const MercuryMatrix& matrix);
		
		virtual void SetHidden( bool bHide ) { m_flags = (uint8_t)SetBit(m_flags,HIDDEN,bHide); } //is there anyway to make this not virtual??
		inline bool IsHidden() { return GetBit(m_flags,HIDDEN); }
		
		inline void SetCulled(bool t) { m_flags = (uint8_t)SetBit(m_flags,CULLED,t); }
		inline bool IsCulled() const { return GetBit(m_flags,CULLED); }

		inline void SetSaveChildren(bool t) { m_flags = (uint8_t)SetBit(m_flags,SAVECHILDREN,t); }
		inline bool GetSaveChildren() const { return GetBit(m_flags,SAVECHILDREN); }

		inline void SetEnableSave(bool t) { m_flags = (uint8_t)SetBit(m_flags,ENABLESAVE,t); }
		inline bool GetEnableSave() const { return GetBit(m_flags,ENABLESAVE); }

		inline void SetUseAlphaPass(bool t) { m_flags = (uint8_t)SetBit(m_flags,ALPHAPATH,t); }
		inline bool GetUseAlphaPass() const { return GetBit(m_flags,ALPHAPATH); }

		virtual MercuryMatrix ManipulateMatrix(const MercuryMatrix& matrix);

		const MercuryMatrix & GetGlobalMatrix() const { return m_pGlobalMatrix[g_iViewportID]; }
		const MercuryMatrix & GetModelViewMatrix() const { return m_pModelViewMatrix[g_iViewportID]; }

		inline unsigned short GetPasses() const { return m_iPasses; }

	protected:
		std::list< MercuryNode* > m_children;	//These nodes are unique, not instanced
		MercuryNode* m_parent;
		MercuryNode* m_prevSibling;
		MercuryNode* m_nextSibling;

		static bool m_rebuildRenderGraph;
		MString m_name;
		
		unsigned char m_flags;
		bool IsInAssetList(MercuryAsset* asset) const;

		unsigned short m_iPasses;
		unsigned short m_iForcePasses;	//If (1<<15) is set, then, we know the force is enabled.

		//The asset is actually stored here
		std::list< MercuryAssetInstance* > m_assets;
		
		//we will just use normal pointers here because we don't want to waste too much time
		//dereferencing the autopointer. As a precaution when assets are added to these lists,
		//they must exist in m_assets.
//		std::list< MercuryAsset* > m_prerender;
//		std::list< MercuryAsset* > m_render;
//		std::list< MercuryAsset* > m_postrender;

		const MercuryMatrix * m_pGlobalMatrix;
		const MercuryMatrix * m_pModelViewMatrix;

		ShaderAttribute* m_shaderAttrModelMatrix;
		friend class RenderGraph;
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

#define NODEFACTORY NodeFactory::GetInstance()

static InstanceCounter<NodeFactory> NFcounter("NodeFactory");


///Register a new instance of the node with the main Mercury Node Registration System.
#define REGISTER_NODE_TYPE(class)\
	MercuryNode* FactoryFunct##class() { return new class(); } \
	Callback0R<MercuryNode*> factoryclbk##class( FactoryFunct##class ); \
	bool GlobalRegisterSuccess##class = NodeFactory::GetInstance().RegisterFactoryCallback(#class, factoryclbk##class); \
	extern "C" { void * Install##class() { LOG.Write("Installing "#class ); \
		NodeFactory::GetInstance().RegisterFactoryCallback(#class, factoryclbk##class); \
		class * t = new class(); \
		void * vtable = *((void**)t); \
		delete t; \
		return vtable; \
	 } }


///Load a variable from XML (safely) - this loads a variable of type name into variable using the transform function.
#define LOAD_FROM_XML( name, variable, function ) \
	if ( !node.Attribute( name ).empty() ) \
		variable = function ( node.Attribute(name) ); 

///Call callee if attribute name exists in XML - attribute can be transformed using function
#define LOAD_FROM_XML_CALL( name, callee, function ) \
	if ( !node.Attribute( name ).empty() ) \
		callee( function ( node.Attribute(name) ) );


#endif

/****************************************************************************
 *   Copyright (C) 2008-2009 by Joshua Allen                                *
 *                              Charles Lohr                                *
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


