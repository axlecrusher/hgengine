#ifndef STATE_CHANGER_H
#define STATE_CHANGER_H

#include <MercuryAsset.h>
#include <RawImageData.h>


#define STATECHANGE_RTTI(x) const char * GetType() { return #x; }

class StateChange : public RefBase
{
public:
	StateChange( const MVector< MString > & sParameters ) {  }
	void Stringify( MString & sOut ) { }

	virtual void Activate() = 0;

	STATECHANGE_RTTI( StateChange );
	int sID;
};

#define REGISTER_STATECHANGE( x ) \
	extern int sID##x;\
	StateChange*CreateNew##x( const MVector< MString > & sParameters ) { x * ret = new x( sParameters ); ret->sID = sID##x; return ret; } \
	int sID##x = StateChangeRegister::Instance().RegisterGenerator( #x, CreateNew##x );

class StateChangeRegister
{
public:
	static StateChangeRegister & Instance();
	int RegisterGenerator( const MString & name, StateChange*(*gn)( const MVector< MString > &sParameters ) );
	MAutoPtr< StateChange > Create( const MString & name, const MVector< MString > & sParameters );

	int GetStateID( const MString & spar );
	int GetStateCount() { return m_iStateCount; }
private:
	MHash< StateChange*(*)(const MVector< MString > &sParameters) > m_Generators;
	static StateChangeRegister * m_Instance;
	MHash< int > m_hStateIDs;
	int m_iStateCount;
};

///State Changer Node
/**
    This node is for things like changing the color of everything, or turning on/off
    blending modes, etc. in the overall system.
    Note: While this node can handle multiple states at once - it is currently
    only set up to be able to load one from the incoming XML. */
class StateChanger : public MercuryAsset
{
public:
	StateChanger();
	virtual ~StateChanger();

	virtual void Render(const MercuryNode* node);
	virtual void PostRender(const MercuryNode* node);

	virtual void LoadFromXML(const XMLNode& node);
	virtual void SaveToXMLTag( MString & sXMLStream );
	static StateChanger* Generate();

	bool LoadFromString( const MString & sDescription );

	GENRTTI( StateChanger );
private:

	MVector< MAutoPtr< StateChange > > m_vStates;

	//Actually... It's faster if we use MercuryVectors here.
	static MVector< MVector< MAutoPtr< StateChange > > > m_StateSet;
};

#endif

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

