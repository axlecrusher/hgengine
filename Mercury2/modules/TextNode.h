#ifndef TEXTNODE_H
#define TEXTNODE_H

#include <MercuryNode.h>
#include <MercuryHash.h>

class MercuryAsset;

class TextNode : public MercuryNode
{
public:
	TextNode();
	
	virtual void Update(float dTime);
	virtual void LoadFromXML(const XMLNode& node);
	virtual bool LoadFont( const MString & sFont );
	virtual void SetSize( float fSize );
	virtual void SetText( const MString & sText );

	inline void SetDirtyText() { m_bDirty = true; }

	virtual void RenderText();

	GENRTTI(TextNode);
private:
	class Font;

	float 	m_fSize;
	MString	m_sText;
	MString	m_sFont;
	bool	m_bDirty;
	Font *	m_pThisFont;

	MAutoPtr< MercuryAsset > m_kVBO;
	MAutoPtr< MercuryAsset > m_kTEX;

	//Font-class specific stuff
	class Glyph
	{
	public:
		Glyph() : fsx( 0 ), fsy( 0 ), fex( 0 ), fey( 0 ), ilx( 0 ), ily( 0 ), iox( 0 ), ioy( 0 ) { }
		float fsx, fsy; //U/V upper left coords
		float fex, fey; //U/V lower left coords
		float ilx, ily; //size x/y in px.
		float iox, ioy; //location x/y in cursor-center of char.
	};

	class Font
	{
	public:
		bool LoadFromFile( const MString & fName );

		std::map< int, Glyph >	m_mGlyphs;
		MString 		m_sImage;
		float			m_fHeight;
	};

	static MHash< Font > g_AllFonts;

	//Datastructure when formatting the text
	class DChar
	{
	public:
		DChar( Glyph * g, int c, float x, float y ) : glyph(g), character(c), xps(x), yps(y) { }

		Glyph * glyph;
		int character;
		float xps;	//aka pos on line
		float yps;	//aka line
	};
};

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
