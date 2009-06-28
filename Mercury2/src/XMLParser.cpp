#include <XMLParser.h>
#include <MercuryFile.h>
#include <MercuryVector.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#if defined(WIN32)
#  if defined(_MSC_VER)
#    pragma comment(lib, "libxml2.lib")
#  endif
#endif

XMLNode::XMLNode(xmlNode* node, xmlDoc* doc)
	:m_node(node), m_doc(doc)
{
}

XMLNode::XMLNode(const XMLNode& n)
	:m_node(n.m_node), m_doc(n.m_doc)
{
}

XMLNode::~XMLNode()
{
	m_node = NULL;
	m_doc = NULL;
}

XMLNode XMLNode::NextNode() const
{
	if ( IsValid() )
	{
		for (xmlNode* node = m_node->next; node; node=node->next)
			if (node->type == XML_ELEMENT_NODE)
				return XMLNode(node,m_doc);

		XMLNode parent(m_node->parent, m_doc);
		XMLNode fall = parent.FindFallbackNode();
		return fall.Child();
	}
	return XMLNode();
}

XMLNode XMLNode::PreviousNode() const
{
	if ( IsValid() )
	{
		for (xmlNode* node = m_node->prev; node; node=node->prev)
			if (node->type == XML_ELEMENT_NODE)
				return XMLNode(node,m_doc);
	}
	return XMLNode();
}

XMLNode XMLNode::Child() const
{
	if ( IsValid() )
	{
		for (xmlNode* node = m_node->children; node; node=node->next)
			if (node->type == XML_ELEMENT_NODE) return XMLNode(node,m_doc);

		XMLNode fall = FindFallbackNode();
		return fall.Child();
	}
	return XMLNode();
}

MString XMLNode::Name() const
{
	if ( !IsValid() ) return "";
	return MString((const char*)m_node->name);
}

MString XMLNode::Content() const
{
	MString data;

	if ( !IsValid() ) return data;

//	xmlChar* d = xmlNodeListGetString(m_doc, m_node->xmlChildrenNode, 1);
	xmlChar* d = xmlNodeGetContent(m_node);
	
	if (d)
	{
		data = MString((const char*)d);
		xmlFree(d);
	}
	
	return data;
}

MString XMLNode::Attribute(const MString& tag) const
{
	MString data;

	if ( !IsValid() ) return data;

	xmlChar* d = xmlGetProp(m_node, (const xmlChar*)tag.c_str());

	if (d)
	{
		data = MString((const char*)d);
		xmlFree(d);
	}
	else
	{
		if (tag != "name") //probably don't want to fallback for names if a name does not exist
		{
			XMLNode fall = FindFallbackNode();
			data = fall.Attribute(tag);
		}
	}

	return data;
}

XMLNode XMLNode::FindFallbackNode() const
{
	xmlChar* d = xmlGetProp(m_node, (const xmlChar*)"fallback");
	XMLNode n;

	if (d)
	{
		MString path((const char*)d);

		int pos = path.find(".");
		MString name = pos<=0?path:path.substr(0, pos);
		MString rpath = pos<=0?"":path.substr(pos+1); //skip the period
		
		XMLNode parent = FindParentWithName( name );
		if ( !parent.IsValid() )
		{
			//nothing found, do search down from root
			parent = XMLNode( xmlDocGetRootElement(m_doc), m_doc );
			rpath = path;
		}

		//prevent infinite recursion on self
		if ( parent.m_node != m_node )
		{	
			n = parent.RecursiveFindFallbackNode( rpath );
		}
		xmlFree(d);
	}

	return n;
}

XMLNode XMLNode::FindParentWithName(const MString& name) const
{
	if ( (!name.empty()) && (m_node->parent) )
	{
		XMLNode parent(m_node->parent, m_doc);
		if (parent.Attribute("name") == name)
			return parent;
		return parent.FindParentWithName( name );
	}
	return XMLNode();
}


XMLNode XMLNode::RecursiveFindFallbackNode(const MString& path) const
{
	if ( !IsValid() ) return XMLNode();

	if (path.length() > 0)
	{
		int pos = path.find(".");
		MString name = pos<=0?path:path.substr(0, pos);
		MString rpath = pos<=0?"":path.substr(pos+1); //skip the period
		for (XMLNode n = this->Child(); n.IsValid(); n = n.NextNode())
			if (n.Attribute("name") == name)
				return n.RecursiveFindFallbackNode(rpath);
		
		return XMLNode();
	}
	return *this;
}

const XMLNode& XMLNode::operator=(const XMLNode& n)
{
	m_node = n.m_node;
	m_doc = n.m_doc;
	return *this;
}

bool XMLNode::GetValue( const MString & sDataPointer, MString & sReturn )
{
	MVector< MString > out;
	SplitStrings( sDataPointer, out, ".", " ", 1, 1 );
	//Out now contains the input in a parsed form;
	//a.b.c is now:
	//out[0] = "a"; out[1] = "b"; out[2] = "c";
	XMLNode & rthis = *this;
	for( unsigned i = 0; i < out.size() - 1; i++ )
	{
		while( rthis.Name() != out[i] && rthis.IsValid() )
			rthis = rthis.NextNode();

		if( !rthis.IsValid() )
			return false;

		if( i < out.size() - 2 )
			rthis = rthis.Child();

		if( !rthis.IsValid() )
			return false;
	}

	sReturn = rthis.Attribute( out[out.size()-1] );
	return true;
}

XMLDocument::XMLDocument()
	:m_doc(NULL)
{
}

XMLDocument::~XMLDocument()
{
	FreeDocument();
	xmlCleanupParser();  //XXX WTF am I supposed to do with this
}
	
void XMLDocument::FreeDocument()
{
	if (m_doc) xmlFreeDoc(m_doc);
}

XMLDocument* XMLDocument::Load(const MString& file)
{
	XMLDocument* xmldoc = new XMLDocument();
	
	xmlInitParser();  //XXX WTF am I supposed to do with this

	char * doc;

	int ret = FileToString( file, doc );

	if( ret == -1 )  //File error
	{
		printf( "Warning: Could not open XML File: \"%s\".\n", file.c_str() );
		return xmldoc;
	}
 
	if( (xmldoc->m_doc = xmlReadMemory( doc, ret, NULL, NULL, 0) ) == 0 )
	{
		printf( "Warning: Could not parse XML File: \"%s\".\n", file.c_str() );
	}

	free( doc );

	return xmldoc;
}

void XMLDocument::LoadFromString(const MString& xml)
{
	xmlInitParser();  //XXX WTF am I supposed to do with this
	m_doc = xmlReadMemory(xml.c_str(), xml.length(), "noname.xml", NULL, 0);
}

XMLNode XMLDocument::GetRootNode()
{
//	if (!m_doc) throw SMOException("XML Document NULL");
	return XMLNode(xmlDocGetRootElement(m_doc), m_doc);
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
