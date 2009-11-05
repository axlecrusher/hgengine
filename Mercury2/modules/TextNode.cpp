#include "TextNode.h"
#include <MercuryVertex.h>
#include <Viewport.h>
#include <MercuryFile.h>
#include <MercuryVBO.h>
#include <Texture.h>
#include <vector>

#define BIG_NUMBER  1e20

REGISTER_NODE_TYPE(TextNode);

TextNode::TextNode()
	:MercuryNode(),m_fSize(1.),
	m_bDirty(false),m_pThisFont(NULL),
	m_alignment( LEFT ),
	m_fTextWidth( INFINITY ),
	m_kVBO(0), m_kTEX(0),
	m_fRMinX(0),m_fRMinY(0),
	m_fRMaxX(0),m_fRMaxY(0)
{
}

void TextNode::Update(float dTime)
{
	if( m_bDirty )
	{
		RenderText();
	}

	MercuryNode::Update( dTime );
}

void TextNode::SaveToXML( MString & sXMLStream, int depth )
{
	sXMLStream += ssprintf( "%*c<node ", depth*3, 32 );

	SaveBaseXMLTag( sXMLStream );
	SaveToXMLTag( sXMLStream );

	sXMLStream += "/>\n";
}

void TextNode::SaveToXMLTag( MString & sXMLStream )
{
	MercuryNode::SaveToXMLTag( sXMLStream );
	if( m_sFont.length() )
		sXMLStream += ssprintf( "font=\"%s\" ", m_sFont.c_str() );
	sXMLStream += ssprintf( "size=\"%f\" ", m_fSize );
	if( m_sText.length() )
		sXMLStream += ssprintf( "text=\"%s\" ", m_sText.c_str() );

	if( m_fTextWidth < 1e9 )
		sXMLStream += ssprintf( "width=\"%f\" ", m_fTextWidth );

	sXMLStream += "alignment=\"";
	switch( m_alignment )
	{
		case LEFT:
			sXMLStream += "LEFT\" ";
			break;
		case RIGHT:
			sXMLStream += "RIGHT\" ";
			break;
		case CENTER:
			sXMLStream += "CENTER\" ";
			break;
		case FIT:
			sXMLStream += "FIT\" ";
			break;
		case FIT_FULL:
			sXMLStream += "FIT_FULL\" ";
			break;
	}
}

void TextNode::LoadFromXML(const XMLNode& node)
{
	MercuryNode::LoadFromXML(node);

	if ( !node.Attribute("font").empty() )
		LoadFont( node.Attribute("font") );
	if ( !node.Attribute("size").empty() )
		SetSize( StrToFloat( node.Attribute("size") ) );
	if ( !node.Attribute("text").empty() )
		SetText( node.Attribute("text") );
	if ( !node.Attribute("width").empty() )
		SetWidth( StrToFloat( node.Attribute("width") ) );
	if ( !node.Attribute("alignment").empty() )
	{
		MString sAlign = node.Attribute("alignment");
		if( sAlign == "RIGHT" )		SetAlignment( RIGHT );
		if( sAlign == "CENTER" )	SetAlignment( CENTER );
		if( sAlign == "FIT" )		SetAlignment( FIT );
		if( sAlign == "FIT_FULL" ) 	SetAlignment( FIT_FULL );
	}
}

void TextNode::RenderText()
{
	std::vector< DChar > chars;
	float xps = 0;
	float yps = 0;
	bool  bWasBlank = true;

	if( !m_pThisFont )
		return;

	if( !m_kTEX )
	{
		m_kTEX = MAutoPtr< MercuryAsset >( Texture::LoadFromFile( m_pThisFont->m_sImage ) );
		if( !m_kTEX )
		{
			fprintf( stderr, "Could not create Texture for text.\n" );
			return;
		}
		AddAsset( m_kTEX );
	}

	//Setup FBO and Texture
	if( !m_kVBO )
	{
		m_kVBO = new MercuryVBO( GetName() + "VBO", true );
		if( !m_kVBO )
		{
			fprintf( stderr, "Could not create VBO for text.\n" );
			return;
		}
		AddAsset( m_kVBO );
	}

	//Pass 1: Figure out actual char positions and count
	for( const char * c = &m_sText[0]; *c != 0; c++ )
	{
		if( *c == 9 )
		{
			xps += m_pThisFont->m_fTab;
			bWasBlank = true;
		}
		else if( *c == 10 )
		{
			xps = 0;
			yps += m_pThisFont->m_fHeight;
			bWasBlank = true;
		}
		else if( *c == 32 )
		{
			xps += m_pThisFont->m_fSpace;
			bWasBlank = true;
		}
		else
		{
			if( m_pThisFont->m_mGlyphs.find( *c ) != m_pThisFont->m_mGlyphs.end() )
			{
				Glyph & g = m_pThisFont->m_mGlyphs[*c];

				chars.push_back( DChar( &g, *c, xps, yps, g.ilx+m_pThisFont->m_fBlank, bWasBlank ) );
				//Here is the right place to do line-splitting if need be.

				xps += g.ilx+m_pThisFont->m_fBlank;

				bWasBlank = false;

				//Check wrapping
				if( xps > m_fTextWidth )
				{
					int i, j;
					//Track backward, and dump current word on next line.
					for( i = (int)chars.size()-1; i >= 0; i-- )
					{
						if( chars[i].bWordStart ) break;
					}

					//If word begins at the beginning of the line
					if( chars[i].xps == 0 )
					{
						//advance "This" letter to the next line
						chars[chars.size()-1].xps = 0;
						chars[chars.size()-1].yps += m_pThisFont->m_fHeight;
						chars[chars.size()-1].bWordStart = true;

						xps = g.ilx+m_pThisFont->m_fBlank;
						yps += m_pThisFont->m_fHeight;
					} else
					{
						//break to new line
						float fPosStart = chars[i].xps;
						for( j = i; j < (int)chars.size(); j++ )
						{
							chars[j].xps -= fPosStart;
							xps = chars[j].xps;
							chars[j].yps += m_pThisFont->m_fHeight;
						}
						xps += g.ilx+m_pThisFont->m_fBlank;
						yps += m_pThisFont->m_fHeight;
					}
				}
			}
		}
	}

	//Stage 2: Handle alignment
	if( m_alignment != LEFT )
	{
		float fLinePos = 0;
		int iLineStart = 0;
		int iLettersOnLine = 0;
		int iWordsOnLine = 0;
		for( unsigned i = 0; i <= chars.size(); i++ )
		{
			bool bNewLine = false;
			if( i < chars.size() )
			{
				DChar & dc = chars[i];
				if( dc.yps != fLinePos )
				{
					bNewLine = true;
				}
				fLinePos = dc.yps;
			} else
				bNewLine = true;

			iLettersOnLine++;

			if( bNewLine && i > 0 )
			{
				float fEndOfLine = chars[i-1].xps + chars[i-1].width;

				iLettersOnLine--;

				if( m_alignment == RIGHT )
				{
					float offset = m_fTextWidth - fEndOfLine;
					if( m_fTextWidth > BIG_NUMBER )
						offset = -fEndOfLine;
					for( unsigned j = iLineStart; j < i; j++ )
					{
						chars[j].xps += offset;
					}
				} else if( m_alignment == CENTER )
				{
					float offset = m_fTextWidth - fEndOfLine;
					offset/=2;
					if( m_fTextWidth > BIG_NUMBER )
						offset = -fEndOfLine/2.;
					for( unsigned j = iLineStart; j < i; j++ )
						chars[j].xps += offset;
				} else if( m_alignment == FIT_FULL )
				{
					float offset = m_fTextWidth - fEndOfLine;
					offset/=float(iLettersOnLine);
					if( m_fTextWidth > BIG_NUMBER )
						offset = 0;

					float letter = 0;

					for( unsigned j = iLineStart; j < i; j++ )
					{
						chars[j].xps += offset*letter;
						letter++;
					}
				} else if( m_alignment == FIT )
				{
					float offset = m_fTextWidth - fEndOfLine;

					if( m_fTextWidth > BIG_NUMBER )
						offset = 0;

					if( iWordsOnLine != 0 )
					{
						offset/=float(iWordsOnLine-1);

						float fTOffset = 0;
	
						for( unsigned j = iLineStart; j < i; j++ )
						{
							if( chars[j].bWordStart && j != (unsigned)iLineStart )
								fTOffset += offset;
							chars[j].xps += fTOffset ;
						}
					}
				}

				iLineStart = i;
				iLettersOnLine = 0;
				iWordsOnLine = 0;
			}

			if( i < chars.size() )
			{
				if( chars[i].bWordStart || i == 0 ) iWordsOnLine++;
			}
		}
	}

	m_fRMinX = 0;
	m_fRMinY = 0;
	m_fRMaxX = 0;
	m_fRMaxY = 0;

	//Stage 3: Actually generate the geometry.
	((MercuryVBO*)m_kVBO.Ptr())->AllocateIndexSpace((unsigned)chars.size()*6);
	((MercuryVBO*)m_kVBO.Ptr())->AllocateVertexSpace((unsigned)chars.size()*4);

	for( unsigned i = 0; i < chars.size(); i++ )
	{
		DChar & dc = chars[i];
		Glyph * g = dc.glyph;
		float * vd = ((MercuryVBO*)m_kVBO.Ptr())->GetVertexHandle();
		short unsigned int * id = ((MercuryVBO*)m_kVBO.Ptr())->GetIndexHandle();

		float sx = (-g->iox       + dc.xps)*m_fSize;
		float sy = -(-g->ioy        + dc.yps)*m_fSize;
		float ex = (g->ilx-g->iox + dc.xps)*m_fSize;
		float ey = -(g->ily-g->ioy + dc.yps)*m_fSize;

		//Vertex1: Upper Left
		vd[(i*4+0)*8+0] = g->fsx;
		vd[(i*4+0)*8+1] = g->fsy;	//U,V
		vd[(i*4+0)*8+2] = 0;
		vd[(i*4+0)*8+3] = 0;
		vd[(i*4+0)*8+4] = -1;	//Normal <0,0,-1>
		vd[(i*4+0)*8+5] = sx;
		vd[(i*4+0)*8+6] = sy;
		vd[(i*4+0)*8+7] = 0;

		//Vertex 2: Upper Right
		vd[(i*4+1)*8+0] = g->fex;
		vd[(i*4+1)*8+1] = g->fsy;
		vd[(i*4+1)*8+2] = 0;
		vd[(i*4+1)*8+3] = 0;
		vd[(i*4+1)*8+4] = -1;	//Normal <0,0,-1>
		vd[(i*4+1)*8+5] = ex;
		vd[(i*4+1)*8+6] = sy;
		vd[(i*4+1)*8+7] = 0;

		//Vertex 3: Lower Right
		vd[(i*4+2)*8+0] = g->fex;
		vd[(i*4+2)*8+1] = g->fey;
		vd[(i*4+2)*8+2] = 0;
		vd[(i*4+2)*8+3] = 0;
		vd[(i*4+2)*8+4] = -1;	//Normal <0,0,-1>
		vd[(i*4+2)*8+5] = ex;
		vd[(i*4+2)*8+6] = ey;
		vd[(i*4+2)*8+7] = 0;

		//Vertex 4: Lower Left
		vd[(i*4+3)*8+0] = g->fsx;
		vd[(i*4+3)*8+1] = g->fey;
		vd[(i*4+3)*8+2] = 0;
		vd[(i*4+3)*8+3] = 0;
		vd[(i*4+3)*8+4] = -1;	//Normal <0,0,-1>
		vd[(i*4+3)*8+5] = sx;
		vd[(i*4+3)*8+6] = ey;
		vd[(i*4+3)*8+7] = 0;

		id[i*6+0] = i * 4 + 2;
		id[i*6+1] = i * 4 + 1;
		id[i*6+2] = i * 4 + 0;
		id[i*6+3] = i * 4 + 0;
		id[i*6+4] = i * 4 + 3;
		id[i*6+5] = i * 4 + 2;

		if( sy > m_fRMaxY ) m_fRMaxY = sy;
		if( ex > m_fRMaxX ) m_fRMaxX = ex;
		if( ey < m_fRMinY ) m_fRMinY = ey;
		if( sx < m_fRMinX ) m_fRMinX = sx;
	}

	((MercuryVBO*)m_kVBO.Ptr())->DirtyVertices();
	((MercuryVBO*)m_kVBO.Ptr())->DirtyIndices();

	m_bDirty = false;
}

bool TextNode::LoadFont( const MString & sFont )
{
	if( g_AllFonts.get( sFont ) == 0 )
	{
		m_pThisFont = &g_AllFonts[sFont];
		if( !m_pThisFont->LoadFromFile( sFont ) )
		{
			fprintf( stderr, "Error: Could not load font: \"%s\".", sFont.c_str() );
			g_AllFonts.remove( sFont );
			//This has to be set to null if there is a failure because this is what RenderText uses as a check
			m_pThisFont = NULL;
			return false;
		}
	} else
		m_pThisFont = &g_AllFonts[sFont];
	SetDirtyText();
	m_sFont = sFont;
	return true;
}

void TextNode::SetSize( float fSize )
{
	m_fSize = fSize;
	SetDirtyText();
}

void TextNode::SetText( const MString & sText )
{
	m_sText = ConvertToUnformatted( sText );
	SetDirtyText();
}


bool TextNode::Font::LoadFromFile( const MString & fName )
{
	int line = 0;
	MString sLine;

	MercuryFile * f = FILEMAN.Open( fName );
	if( !f )
		return false;

	if( !f->ReadLine( m_sImage ) )
	{
		delete f;
		return false;
	}

	if( !f->ReadLine( sLine ) )
	{
		delete f;
		return false;
	}

	if( sscanf( sLine.c_str(), "%f %f %f %f", &m_fHeight, &m_fBlank, &m_fSpace, &m_fTab ) != 4 )
	{
		fprintf( stderr, "Malformatted font size.\n" );
		delete f;
		return false;
	}

	while( !f->Eof() )
	{
		int res;
		line++;

		Glyph g;
		int ch;

		f->ReadLine( sLine );
		if( ( res = sscanf( sLine.c_str(), "%d %f %f %f %f %f %f %f %f", &ch,
			&g.fsx, &g.fsy, &g.fex, &g.fey, &g.ilx, &g.ily, &g.iox, &g.ioy ) ) != 9 )
		{
			if( res == 0 || res == -1 ) continue;
			fprintf( stderr, "Error on line %d of font \"%s\" (invalid number of args) (%d args).\n", line, fName.c_str(), res );
			delete f;
			return false;
		}

		//Insert the glyph into the map.
		m_mGlyphs[ch] = g;
	}

	delete f;
	return true;
}

MHash< TextNode::Font > TextNode::g_AllFonts;


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
