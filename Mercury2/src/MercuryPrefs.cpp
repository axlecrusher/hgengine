#include <MercuryPrefs.h>
#include <XMLParser.h>

MercuryPreferences & MercuryPreferences::GetInstance()
{
	static MercuryPreferences * m_gPreferences = 0;
	if( !m_gPreferences )
		m_gPreferences = new MercuryPreferences();
	return *m_gPreferences;
}

MercuryPreferences::MercuryPreferences()
{
	m_PrefsDoc  = XMLDocument::Load("preferences.xml");
	if( !m_PrefsDoc )
		FAIL( "Could not load preferences.xml." );

	m_PrefsNode = new XMLNode();

	*m_PrefsNode = m_PrefsDoc->GetRootNode();

	if( !m_PrefsNode->IsValid() )
		FAIL( "Could not get root node in Preferences." );

	*m_PrefsNode = m_PrefsNode->Child();

	if( !m_PrefsNode->IsValid() )
		FAIL( "Could not get Preferences node in Preferences." );
}

MercuryPreferences::~MercuryPreferences()
{
	delete m_PrefsDoc;
	delete m_PrefsNode;
}

bool MercuryPreferences::GetValue( const MString & sDataPointer, MString & sReturn )
{
	return m_PrefsNode->GetValue( sDataPointer, sReturn );
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