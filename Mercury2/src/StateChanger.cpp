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
		glColor4f( r,g,b,a );
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
		sOut = ssprintf( "%f", bEnable );
	}

	void Activate()
	{
		if( bEnable )
			glEnable( GL_LIGHTING );
		else
			glDisable( GL_LIGHTING );
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
		sOut = ssprintf( "%f", bEnable );
	}

	void Activate()
	{
		if( bEnable )
			glEnable( GL_DEPTH_TEST );
		else
			glDisable( GL_DEPTH_TEST );
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
		sOut = ssprintf( "%f", bEnable );
	}

	void Activate()
	{
		glDepthMask( bEnable );
	}

	STATECHANGE_RTTI( DepthWrite );
	bool bEnable;
};

REGISTER_STATECHANGE( DepthWrite );

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
	:MercuryAsset( key, bInstanced )
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
		m_StateSet[k->sID].push_back( k );
	}
}

void StateChanger::PostRender(const MercuryNode* node)
{
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
}

void StateChanger::SaveToXMLTag( MString & sXMLStream )
{
	if( m_vStates.size() )
	{
		MString sStr;
		m_vStates[0]->Stringify( sStr );
		sXMLStream += "file=\"" + sStr +  "\" ";
	}

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

