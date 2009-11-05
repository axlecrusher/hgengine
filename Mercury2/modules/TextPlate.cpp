#include "TextPlate.h"
#include <TextNode.h>
#include <MercuryVertex.h>
#include <Viewport.h>
#include <Quad.h>
#include <GLHeaders.h>
#include <StateChanger.h>

REGISTER_NODE_TYPE(TextPlate);

TextPlate::TextPlate()
	:BillboardNode()
{
	m_TextNode = (TextNode*)NODEFACTORY.Generate( "TextNode" );
	AddChild( m_TextNode );
}

void TextPlate::Update(float dTime)
{
	BillboardNode::Update( dTime );
	m_billboardMatrix.Translate( m_fvOffset );
}

void TextPlate::SaveToXML( MString & sXMLStream, int depth )
{
	sXMLStream += ssprintf( "%*c<node ", depth*3, 32 );

	SaveBaseXMLTag( sXMLStream );
	SaveToXMLTag( sXMLStream );
	m_TextNode->SaveToXMLTag( sXMLStream );

	sXMLStream += "/>\n";
}

void TextPlate::SaveToXMLTag( MString & sXMLStream )
{
	BillboardNode::SaveToXMLTag( sXMLStream );
	if( m_fvOffset.Length() > 1e-9 )
		sXMLStream += ssprintf( "offset=\"%f,%f,%f\" ", m_fvOffset[0], m_fvOffset[1], m_fvOffset[2] );
}

void TextPlate::LoadFromXML(const XMLNode& node)
{
	BillboardNode::LoadFromXML(node);
	if ( !node.Attribute("offset").empty() )
		m_fvOffset = MercuryVector::CreateFromString( node.Attribute("offset") );
	m_TextNode->LoadFromXML(node);
	if ( !node.Attribute("text").empty() )
		SetText( node.Attribute("text") );
}

void TextPlate::SetText( const MString & sText )
{
	m_TextNode->SetText(sText);
	m_TextNode->RenderText();

	MString quadstr = ssprintf( "%f,%f,%f,%f,-.01",
		m_TextNode->GetRMinX() * 1.1 ,
		m_TextNode->GetRMinY() * 1.1 ,
		m_TextNode->GetRMaxX() * 1.1 ,
		m_TextNode->GetRMaxY() * 1.1 );
	if( !m_BackPlane )
	{

		m_BackgroundColor = ASSETFACTORY.Generate( "StateChanger", "ColorChange:1,1,0,1" );
		((StateChanger*)m_BackgroundColor.Ptr())->ChangeKey( "ColorChange:1,1,0,1" );
		AddAsset( m_BackgroundColor );

		m_BackPlane = ASSETFACTORY.Generate( "Quad", quadstr );
		AddAsset( m_BackPlane );
	}
	else
		((Quad*)m_BackPlane.Ptr())->ChangeKey( quadstr );
}


/****************************************************************************
 *   Copyright (C) 2009 by Charles Lohr                                     *
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
