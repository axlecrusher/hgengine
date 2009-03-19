#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <MAutoPtr.h>

struct _xmlNode;
typedef struct _xmlNode xmlNode;
struct _xmlDoc;
typedef struct _xmlDoc xmlDoc;

class XMLElement
{
	public:
	private:
};

class XMLNode
{
	public:
		XMLNode(xmlNode* node = NULL, xmlDoc* doc = NULL);
		XMLNode(const XMLNode& n);
		~XMLNode();
		
		XMLNode NextNode() const;
		XMLNode PreviousNode() const;
		XMLNode Child() const;
		
		MString Name() const;
		MString Content() const;
		MString Attribute(const MString & tag) const;

//		MString FindFallbackAttribute();
		XMLNode FindFallbackNode(const MString& path) const;
		
		inline bool IsValid() const { return m_node!=NULL; }
		
	private:
		xmlNode* m_node;
		xmlDoc* m_doc; //parent doc, don't free
};

class XMLDocument : public RefBase
{
	public:
		XMLDocument();
		virtual ~XMLDocument();
		
		static XMLDocument* Load(const MString& file);
		void LoadFromString(const MString& xml);

		
		XMLNode GetRootNode();
	
	private:
		void FreeDocument();
		xmlDoc *m_doc;
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
 *     * Neither the name of the Mercury developers nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission. *
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
