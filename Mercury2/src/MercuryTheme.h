#ifndef _MERCURY_THEME_H
#define _MERCURY_THEME_H

#include <MercuryNamedResource.h>
#include <MercuryHash.h>
#include <MercuryVector.h>
#include <InstanceCounter.h>

class XMLNode;
class XMLDocument;

class MercuryThemeManager : public MercuryNamedResource
{
public:
	MercuryThemeManager();
	~MercuryThemeManager();

	class Theme
	{
	public:
		Theme( );
		bool Setup( const MString & sThemeName );
		~Theme();
		MString sTheme;
		XMLNode *	m_xNode;
		XMLDocument  *	m_xDoc;
	};
	
	static MercuryThemeManager& GetInstance();

	virtual bool GetValue( const MString & sDataPointer, MString & sReturn );
	const MVector< Theme > & GetThemes() { return m_vThemes; }
private:
	MVector< Theme > m_vThemes;
	MHash< MString > m_hCache;
};

static InstanceCounter<MercuryThemeManager> MTMcounter("MercuryThemeManager");

#define THEME MercuryThemeManager::GetInstance()


#endif

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
