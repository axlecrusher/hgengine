#include <XMLParser.h>
//#include <SMOException.h>

XMLNode::XMLNode(xmlNode* node, xmlDoc* doc)
	:m_node(node), m_doc(doc)
{
}

XMLNode::XMLNode(const XMLNode& n)
	:m_node(n.m_node)
{
}

XMLNode::~XMLNode()
{
	m_node = NULL;
}

XMLNode XMLNode::NextNode() const
{
	for (xmlNode* node = m_node->next; node; node=node->next)
		if (node->type == XML_ELEMENT_NODE)
			return XMLNode(node,m_doc);
	return XMLNode();
}

XMLNode XMLNode::PreviousNode() const
{
	for (xmlNode* node = m_node->prev; node; node=node->prev)
		if (node->type == XML_ELEMENT_NODE)
			return XMLNode(node,m_doc);
	return XMLNode();
}

XMLNode XMLNode::Child() const
{
	for (xmlNode* node = m_node->children; node; node=node->next)
		if (node->type == XML_ELEMENT_NODE) return XMLNode(node,m_doc);
	return XMLNode();
}

std::string XMLNode::Name() const
{
	return std::string((const char*)m_node->name); //XXX fix utf8
}

std::string XMLNode::Content() const
{
	string data;
//	xmlChar* d = xmlNodeListGetString(m_doc, m_node->xmlChildrenNode, 1);
	xmlChar* d = xmlNodeGetContent(m_node);
	
	if (d)
	{
		data = string((const char*)d);
		xmlFree(d);
	}
	
	return data;
}

std::string XMLNode::Attribute(const std::string& tag) const
{
	string data;
	xmlChar* d = xmlGetProp(m_node, (const xmlChar*)tag.c_str());

	if (d)
	{
		data = string((const char*)d);
		xmlFree(d);
	}
	return data;
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

XMLDocument* XMLDocument::Load(const std::string& file)
{
	XMLDocument* xmldoc = new XMLDocument();
	
	xmlInitParser();  //XXX WTF am I supposed to do with this
	xmldoc->m_doc = xmlReadFile(file.c_str(), NULL, 0);
	
	return xmldoc;
}

void XMLDocument::LoadFromString(const std::string& xml)
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