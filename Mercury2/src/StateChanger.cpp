#include <StateChanger.h>
#include <MercuryNode.h>
#include <GLHeaders.h>

using namespace std;

//////////////////////////////////////STATE CHANGERS//////////////////////////////////////////

///State changer for color changing.
class ColorChange : public StateChange
{
public:
	ColorChange( const MVector< MString > & sParameters ) : StateChange( sParameters )
	{
		if( sParameters.size() < 3 )
		{
			LOG.Write( ssprintf( "Error: ColorChange state has invalid number of parameters(%d).", sParameters.size() ) );
			return;
		}

		r = StrToFloat( sParameters[0] );
		g = StrToFloat( sParameters[1] );
		b = StrToFloat( sParameters[2] );

		if( sParameters.size() > 3 )
			a = StrToFloat( sParameters[3] );
		else
			a = 1.;
	}

	void Stringify( MString & sOut )
	{
		sOut = ssprintf( "%f,%f,%f,%f", r,g,b,a );
	}

	void Activate()
	{
		GLCALL( glColor4f( r,g,b,a ) );
	}

	STATECHANGE_RTTI( ColorChange );
	float r,g,b,a;
};

REGISTER_STATECHANGE( ColorChange );

///State changer for enabling/disabling lighting
class LightingSwitch : public StateChange
{
public:
	LightingSwitch( const MVector< MString > & sParameters ) : StateChange( sParameters )
	{
		if( sParameters.size() < 1 )
		{
			LOG.Write( ssprintf( "Error: ColorChange state has invalid number of parameters(%d).", sParameters.size() ) );
			return;
		}

		bEnable = StrToBool( sParameters[0] );
	}

	void Stringify( MString & sOut )
	{
		sOut = (bEnable)?"1":"0";
	}

	void Activate()
	{
		if( bEnable )
		{
			GLCALL( glEnable( GL_LIGHTING ) );
		}
		else
		{
			GLCALL( glDisable( GL_LIGHTING ) );
		}
	}

	STATECHANGE_RTTI( LightingSwitch );
	bool bEnable;
};

REGISTER_STATECHANGE( LightingSwitch );

///State changer for enabling/disabling lighting
class DepthTest : public StateChange
{
public:
	DepthTest( const MVector< MString > & sParameters ) : StateChange( sParameters )
	{
		if( sParameters.size() < 1 )
		{
			LOG.Write( ssprintf( "Error: DepthTest state has invalid number of parameters(%d).", sParameters.size() ) );
			return;
		}

		bEnable = StrToBool( sParameters[0] );
	}

	void Stringify( MString & sOut )
	{
		sOut = (bEnable)?"1":"0";
	}

	void Activate()
	{
		if( bEnable )
		{
			GLCALL( glEnable( GL_DEPTH_TEST ) );
		}
		else
		{
			GLCALL( glDisable( GL_DEPTH_TEST ) );
		}
	}

	STATECHANGE_RTTI( DepthTest );
	bool bEnable;
};

REGISTER_STATECHANGE( DepthTest );

class DepthWrite : public StateChange
{
public:
	DepthWrite( const MVector< MString > & sParameters ) : StateChange( sParameters )
	{
		if( sParameters.size() < 1 )
		{
			LOG.Write( ssprintf( "Error: DepthWrite state has invalid number of parameters(%d).", sParameters.size() ) );
			return;
		}

		bEnable = StrToBool( sParameters[0] );
	}

	void Stringify( MString & sOut )
	{
		sOut = (bEnable)?"1":"0";
	}

	void Activate()
	{
		GLCALL( glDepthMask( bEnable ) );
	}

	STATECHANGE_RTTI( DepthWrite );
	bool bEnable;
};

REGISTER_STATECHANGE( DepthWrite );

class FaceCulling : public StateChange
{
public:
	FaceCulling( const MVector< MString > & sParameters ) : StateChange( sParameters )
	{
		if( sParameters.size() < 1 )
		{
			LOG.Write( ssprintf( "Error: DepthWrite state has invalid number of parameters(%d).", sParameters.size() ) );
			return;
		}
		if( sParameters[0].compare( "front" ) == 0 )
			iWhich = 1;
		if( sParameters[0].compare( "back" ) == 0 )
			iWhich = 2;
		else
			iWhich = 0;
	}

	void Stringify( MString & sOut )
	{
		sOut = (iWhich)?((iWhich==1)?"front":"back"):"";
	}

	void Activate()
	{
		if( iWhich )
		{
			GLCALL( glEnable(GL_CULL_FACE) );
			if( iWhich == 1 )
				{GLCALL( glCullFace(GL_FRONT) );}
			else
				{GLCALL( glCullFace(GL_BACK) );}
		}
		else
		{
			GLCALL( glDisable(GL_CULL_FACE) );
		}
	}

	STATECHANGE_RTTI( FaceCulling );
	int iWhich;
};

REGISTER_STATECHANGE( FaceCulling );


class BlendFunc : public StateChange
{
public:
	BlendFunc( const MVector< MString > & sParameters ) : StateChange( sParameters )
	{
		if( sParameters.size() < 2 )
		{
			LOG.Write( ssprintf( "Error: BlendFunc state has invalid number of parameters(%d).", sParameters.size() ) );
			return;
		}
		
		m_src = StrToBlend(sParameters[0] );
		m_dest = StrToBlend(sParameters[1] );
	}

	void Stringify( MString & sOut )
	{
		sOut = BlendToString(m_src) + "," + BlendToString(m_dest);
	}

#define STRTOGL(x,s) if (x==#s) return GL_##s;
	int StrToBlend(const MString& s)
	{
		STRTOGL(s, ZERO);
		STRTOGL(s, ONE);
		STRTOGL(s, SRC_COLOR);
		STRTOGL(s, ONE_MINUS_SRC_COLOR);
		STRTOGL(s, DST_COLOR);
		STRTOGL(s, ONE_MINUS_DST_COLOR);
		STRTOGL(s, SRC_ALPHA);
		STRTOGL(s, ONE_MINUS_SRC_ALPHA);
		STRTOGL(s, DST_ALPHA);
		STRTOGL(s, ONE_MINUS_DST_ALPHA);
		STRTOGL(s, CONSTANT_COLOR);
		STRTOGL(s, ONE_MINUS_CONSTANT_COLOR);
		STRTOGL(s, CONSTANT_ALPHA);
		STRTOGL(s, ONE_MINUS_CONSTANT_ALPHA);
		STRTOGL(s, SRC_ALPHA_SATURATE);
		return -1;
	}
	
	#define GLTOSTR(x,s) case GL_##s: return #s;
	MString BlendToString(int blend)
	{
		switch (blend)
		{
			GLTOSTR(blend, ZERO);
			GLTOSTR(blend, ONE);
			GLTOSTR(blend, SRC_COLOR);
			GLTOSTR(blend, ONE_MINUS_SRC_COLOR);
			GLTOSTR(blend, DST_COLOR);
			GLTOSTR(blend, ONE_MINUS_DST_COLOR);
			GLTOSTR(blend, SRC_ALPHA);
			GLTOSTR(blend, ONE_MINUS_SRC_ALPHA);
			GLTOSTR(blend, DST_ALPHA);
			GLTOSTR(blend, ONE_MINUS_DST_ALPHA);
			GLTOSTR(blend, CONSTANT_COLOR);
			GLTOSTR(blend, ONE_MINUS_CONSTANT_COLOR);
			GLTOSTR(blend, CONSTANT_ALPHA);
			GLTOSTR(blend, ONE_MINUS_CONSTANT_ALPHA);
			GLTOSTR(blend, SRC_ALPHA_SATURATE);
		};
	}
	
	void Activate()
	{
		GLCALL( glBlendFunc(m_src,m_dest) );
	}

	STATECHANGE_RTTI( BlendFunc );
	int m_src, m_dest;
};

REGISTER_STATECHANGE( BlendFunc );

class AlphaFunc : public StateChange
{
public:
	AlphaFunc( const MVector< MString > & sParameters ) : StateChange( sParameters )
	{
		if( sParameters.size() < 2 )
		{
			LOG.Write( ssprintf( "Error: AlphaFunc state has invalid number of parameters(%d).", sParameters.size() ) );
			return;
		}
		
		m_func = StrToAlpha(sParameters[0] );
		m_ref = StrToFloat(sParameters[1] );
	}

	void Stringify( MString & sOut )
	{
		sOut = AlphaToStr(m_func) + ssprintf( ",%f", m_ref );
	}

	#define STRTOGL(x,s) if (x==#s) return GL_##s;
	int StrToAlpha(const MString& s)
	{
		STRTOGL(s, NEVER);
		STRTOGL(s, LESS);
		STRTOGL(s, EQUAL);
		STRTOGL(s, LEQUAL);
		STRTOGL(s, GREATER);
		STRTOGL(s, NOTEQUAL);
		STRTOGL(s, GEQUAL);
		STRTOGL(s, ALWAYS);
		return -1;
	}
	
	#define GLTOSTR(x,s) case GL_##s: return #s;
	MString AlphaToStr(int blend)
	{
		switch (blend)
		{
			GLTOSTR(blend, NEVER);
			GLTOSTR(blend, LESS);
			GLTOSTR(blend, EQUAL);
			GLTOSTR(blend, LEQUAL);
			GLTOSTR(blend, GREATER);
			GLTOSTR(blend, NOTEQUAL);
			GLTOSTR(blend, GEQUAL);
			GLTOSTR(blend, ALWAYS);
		};
	}
	
	void Activate()
	{
		GLCALL( glEnable( GL_ALPHA_TEST ) );
		GLCALL( glAlphaFunc(m_func,m_ref) );
	}

	STATECHANGE_RTTI( AlphaFunc );
	int m_func;
	float m_ref;
};

REGISTER_STATECHANGE( AlphaFunc );

//////////////////////////////////////STATE CHANGE CHUNK//////////////////////////////////////

StateChangeRegister & StateChangeRegister::Instance()
{
	static StateChangeRegister* m_Instance = NULL;
	if( !m_Instance )
		m_Instance = new StateChangeRegister();
	return *m_Instance;
}

int StateChangeRegister::RegisterGenerator( const MString & name, StateChange*(*gn)( const MVector< MString > &sParameters ) )
{
	m_Generators[name] = gn;
	m_hStateIDs[name] = m_iStateCount++;
	return m_iStateCount-1;
}

MAutoPtr< StateChange > StateChangeRegister::Create( const MString & name, const MVector< MString > & sParameters )
{
	StateChange*(**tgn)( const MVector< MString > &sParameters );
	if( !( tgn = m_Generators.get( name ) ) )
		return 0;

	return (**tgn)(sParameters );
}

int StateChangeRegister::GetStateID( const MString & spar )
{
	int * r = m_hStateIDs.get( spar );
	if( r )
		return *r;
	else
		return 0;
}


//////////////////////////////////////STATE CHANGER ASSET/////////////////////////////////////

REGISTER_ASSET_TYPE(StateChanger);

StateChanger::StateChanger( const MString & key, bool bInstanced )
	:MercuryAsset( key, bInstanced ),  m_isEnduring(0)
{
	//Make sure our state stack is correctly sized
	if( m_StateSet.size() < (unsigned)StateChangeRegister::Instance().GetStateCount() )
		m_StateSet.resize( StateChangeRegister::Instance().GetStateCount() );

	LoadInternal( key );
}

StateChanger::~StateChanger()
{
}

void StateChanger::Render(const MercuryNode* node)
{
	for( unsigned i = 0; i < m_vStates.size(); i++ )
	{
		MAutoPtr< StateChange > & k = m_vStates[i];
		k->Activate();
		if( !m_isEnduring )
			m_StateSet[k->sID].push_back( k );
	}
}

void StateChanger::PostRender(const MercuryNode* node)
{
	if( !m_isEnduring )
		for( unsigned i = 0; i < m_vStates.size(); i++ )
		{
			MAutoPtr< StateChange > & k = m_vStates[i];
			MVector< MAutoPtr< StateChange > > & l = m_StateSet[k->sID];

			unsigned ilpos = l.size() - 1;

			if( ilpos <= 0 )
				continue;

			l.resize( ilpos-- );

			if( ilpos >= 0 )
				l[ilpos]->Activate();
		}
}

bool StateChanger::ChangeKey( const MString & sFile )
{
	if( m_path == sFile )
		return true;

	LoadInternal( sFile );

	return MercuryAsset::ChangeKey( sFile );
}

bool StateChanger::LoadInternal( const MString & sFile )
{
	int f = sFile.find( ":", 0 );
	if( f <= 0 )
	{
		LOG.Write( ssprintf( "Error loading new StateChanger node.  File: \"%s\" improperly formatted, required \"[type]:[parameters]\".", sFile.c_str() ) );
		return false;
	}

	MString sType = sFile.substr( 0, f );
	MString sParameters = sFile.substr( f+1 );
	MVector< MString > vsParameters;

	SplitStrings( sParameters, vsParameters, ",", " ", 1, 1 );

	MAutoPtr< StateChange > s = StateChangeRegister::Instance().Create( sType, vsParameters );
	if( s.Ptr() )
		m_vStates.push_back( s );
	else
	{
		LOG.Write( ssprintf( "Error: Could not make new StateChanger from: \"%s\"", sFile.c_str() ) );
		return false;
	}

	return true;
}

void StateChanger::LoadFromXML(const XMLNode& node)
{
	MercuryAsset::LoadFromXML(node);
	if ( !node.Attribute("file").empty() )
	{
		MString sFile = node.Attribute("file");
		ChangeKey( sFile );
	}

	LOAD_FROM_XML( "enduring", m_isEnduring, StrToBool );
}

void StateChanger::SaveToXMLTag( MString & sXMLStream )
{
	if( m_vStates.size() )
	{
		MString sStr;
		m_vStates[0]->Stringify( sStr );
		sXMLStream += "file=\"" + sStr +  "\" ";
	}

	if( m_isEnduring )
		sXMLStream += "enduring=\"1\" ";

	MercuryAsset::SaveToXMLTag( sXMLStream );
}

MVector< MVector< MAutoPtr< StateChange > > > StateChanger::m_StateSet;

/****************************************************************************
 *   Copyright (C) 2008 - 2009 by Joshua Allen                              *
 *                                Charles Lohr                              *
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

