#include <MercuryTheme.h>
#include <MercuryPrefs.h>
#include <XMLParser.h>

#define MAX_THEMES 100

MercuryThemeManager & MercuryThemeManager::GetInstance()
{
	static MercuryThemeManager * m_gTheme = 0;
	if( !m_gTheme )
		m_gTheme = new MercuryThemeManager();
	return *m_gTheme;
}

MercuryThemeManager::MercuryThemeManager()
{
	int i;

	m_vThemes.resize( 0 );

	for( i = 0; i < MAX_THEMES; i++ )
	{
		MString ThemeName = PREFSMAN.GetValueS( ssprintf( "Themes.Theme%d", i ) );
		if( !ThemeName.length() )
			break;

		m_vThemes.resize( m_vThemes.size() + 1 );
		if( !m_vThemes[m_vThemes.size()-1].Setup( ThemeName ) )
			m_vThemes.resize( m_vThemes.size() - 1 );
	}
	if( m_vThemes.size() == 0 )
	{
		FAIL( "Zero themes loaded.  This may be due to a misconfiguration in your preferences.ini" );
	}
}

MercuryThemeManager::~MercuryThemeManager()
{
	//no code
}

bool MercuryThemeManager::GetValue( const MString & sDataPointer, MString & sReturn )
{
	MString * cacheret;
	if( (cacheret = m_hCache.get( sDataPointer ) ) != 0 )
	{
		sReturn = *cacheret;
		return true;
	}

	MVector< MString > out;
	SplitStrings( sDataPointer, out, ".", " ", 1, 1 );

	for( unsigned i = out.size() - 1; i > 0; i-- )
	{
		MString sOut;
		for( int j = 0; j < (int)i; j++ )
			sOut += out[j] + '.';
		sOut += out[out.size()-1];

		for( int j = (int)m_vThemes.size() - 1; j >= 0; j-- )
		{
			if( m_vThemes[j].m_xNode->GetValue( sOut, sReturn ) )
			{
				m_hCache[sDataPointer] = sReturn;
				return true;
			}
		}
	}

	return false;
}

MercuryThemeManager::Theme::Theme()
{
	m_xDoc = 0;
	m_xNode = 0;
}

MercuryThemeManager::Theme::~Theme()
{
	SAFE_DELETE( m_xDoc );
	SAFE_DELETE( m_xNode );
}

bool MercuryThemeManager::Theme::Setup( const MString & sThemeName )
{
	sTheme = sThemeName;
	m_xDoc  = XMLDocument::Load(ssprintf("Themes/%s/metrics.xml",sTheme.c_str()));
	if( !m_xDoc )
	{
		printf( "Could not open: Themes/%s/metrics.xml\n",sTheme.c_str());
		return false;
	}

	m_xNode = new XMLNode();

	*m_xNode = m_xDoc->GetRootNode();

	if( !m_xNode->IsValid() )
	{
		printf( "Could not get root node in: Themes/%s/metrics.xml\n",sTheme.c_str());
		return false;
	}

	*m_xNode = m_xNode->Child();

	if( !m_xNode->IsValid() )
	{
		printf( "Could not get sub node in: Themes/%s/metrics.xml\n",sTheme.c_str());
		return false;
	}

	return true;
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

