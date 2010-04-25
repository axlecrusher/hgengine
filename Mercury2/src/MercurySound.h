#ifndef _MERCURY_SOUND_H
#define _MERCURY_SOUND_H

#include <MercuryString.h>
#include <MercuryTimer.h>
#include <MAutoPtr.h>
#include <MercuryHash.h>
#include <MercuryVector.h>

class MercurySoundSource : public RefBase
{
public:
	MercurySoundSource( MercurySoundSource * chain = 0 );

	//This is actually called by Attach/Detach sound, you should not call it.
	void SampleHoldCalc();

	virtual void FillBuffer( float * cBufferToFill, int iCount ) = 0;  //Fill buffer - NOTE: You CANNOT Add or remove yourself from this function.
	virtual bool PostFill() = 0;	//If returns false, remove sound.
	virtual bool Load( const MString & sDescriptor ) = 0;
protected:
	//Tricky - We have this so we can do deterministic sound.
	unsigned long m_iSampleHold;

	MAutoPtr< MercurySoundSource > m_ChainedInput;
};


#define REGISTER_SOUND_SOURCE( sname, alias ) \
	MercurySoundSource * GenNewSoundSource##sname( MAutoPtr< MercurySoundSource > s) \
	{ return new sname( s ); } \
	int SoundSourceRegisterID##sname = SOUNDMAN->RegisterSoundSource( alias, &GenNewSoundSource##sname );


class MercurySoundDriver
{
public:
	virtual bool Init( const MString & sParameters ) = 0;
	virtual void Close() = 0;
};

class MercurySoundDriverConstructionPair
{
public:
	MercurySoundDriverConstructionPair( MercurySoundDriver * (*og)(), int Pri ) :
		OutputGenerator( og ), Priority( Pri ) { }

	MercurySoundDriver * (*OutputGenerator)();
	int Priority;
};

#define REGISTER_SOUND_DRIVER( SD, priority ) \
	MercurySoundDriver * MakeNew##SD() \
	{ \
		return (MercurySoundDriver*)new SD; \
	} \
	int SoundDriver##SD##No = MercurySoundManager::Instance()->RegisterDriver( MercurySoundDriverConstructionPair( MakeNew##SD, priority ) );

class MercurySoundManager
{
public:
	MercurySoundManager() { }
	~MercurySoundManager();

	static MercurySoundManager * Instance();

	void Init(const MString & sParameters);
	void AttachSound( MAutoPtr< MercurySoundSource > s, bool bSynchronize );
	void DetachSound( MAutoPtr< MercurySoundSource > s );


	//Useful tools
	inline float GetSecondsSinceLastFrame() { float tr = m_tLastTrip.Age(); return (tr>1.f)?1.f:tr; }
	inline unsigned int SamplesSinceLastFrame() { float tr = m_tLastTrip.Age(); return (unsigned int)(tr>1.)?fSPS:(tr*fSPS); }

	//For registering and creation of new sound sources...
	MAutoPtr< MercurySoundSource > LoadSoundSource( const MString & sSourceType, MAutoPtr< MercurySoundSource > Chain = 0 );
	void PlaySound( const MString &sSourceType, const MString & sSourceName, bool bSyncronize = false );
	int RegisterSoundSource( const char * SourceType, MercurySoundSource * (*)( MAutoPtr< MercurySoundSource > ) );

	//Section of functions only the output drivers call.
	//--------------------------------------------------
	int RegisterDriver( MercurySoundDriverConstructionPair s );

	///Generally only called by an output sound driver, acquires sound buffer.
	void FillBuffer( float * cBufferToFill, int iCount );

	//After the buffer has been shipped off to the sound card, we can call this to fix everything up.
	void PostFill();

	static int iChannels;
	static int iLastCountDifference;
	static float fSPS;	//0 if un-initialized output.
private:
	MVector< MAutoPtr< MercurySoundSource > > m_vpChildren;
	MHash< MercurySoundSource * (*)( MAutoPtr< MercurySoundSource > ) > m_SoundSourceCreators;
	MercurySoundDriver * m_SoundDriver;
	MVector< MercurySoundDriverConstructionPair > m_Drivers;

	MercuryTimer m_tLastTrip;
};

#define SOUNDMAN MercurySoundManager::Instance()

#endif

/****************************************************************************
 *   Copyright (C) 2009 by Charles Lohr                                     *
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


