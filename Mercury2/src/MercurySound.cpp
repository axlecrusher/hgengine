#include <MercuryUtil.h>
#include <MercurySound.h>
#include <MercuryLog.h>
#include <MercuryMath.h>
#include <set>


MercurySoundSource::MercurySoundSource( MercurySoundSource * chain ) :
	m_iSampleHold( 0 ), m_ChainedInput( chain )
{
}

void MercurySoundSource::SampleHoldCalc()
{
	m_iSampleHold = SOUNDMAN.SamplesSinceLastFrame();
}


MercurySoundManager & MercurySoundManager::GetInstance()
{
	static MercurySoundManager * tm;
	if( !tm )
	{
		tm = new MercurySoundManager();
		tm->m_SoundDriver = 0;
	}
	return *tm;
}

MercurySoundManager::~MercurySoundManager()
{
	if( m_SoundDriver )
	{
		m_SoundDriver->Close();
		SAFE_DELETE( m_SoundDriver );
	}
}


void MercurySoundManager::Init( const MString & sParameters )
{
	MercurySoundDriverConstructionPair * dBest = 0;
	std::set< MercurySoundDriverConstructionPair * > scBlacklist;

	do
	{
		//First, select approprate sound driver.
		for( unsigned i = 0; i < m_Drivers.size(); i++ )
		{
			if( scBlacklist.find( &m_Drivers[i] ) != scBlacklist.end() )
				continue;

			if( (!dBest) || m_Drivers[i].Priority < dBest->Priority )
				dBest = &m_Drivers[i];
		}

		if( !dBest )
		{
			LOG.Write( "Sound card could not be initialized.\n" );
			return;
		}

		m_SoundDriver = (*(dBest->OutputGenerator))();

		if( m_SoundDriver->Init( sParameters ) )
			break;

		//Otherwise something went wrong.
		m_SoundDriver->Close();
		SAFE_DELETE( m_SoundDriver );
		scBlacklist.insert( dBest );
	} while( true );

}


MAutoPtr< MercurySoundSource > MercurySoundManager::LoadSoundSource( const MString & sSourceType, MAutoPtr< MercurySoundSource > Chain )
{
	MercurySoundSource * (**c)( MAutoPtr< MercurySoundSource > ) = m_SoundSourceCreators.get( sSourceType );
	if( c )
		return (*(*c))(Chain);
	else
		return 0;
}

void MercurySoundManager::PlaySound( const MString &sSourceType, const MString & sSourceName, bool bSyncronize )
{
	MAutoPtr< MercurySoundSource > ret = LoadSoundSource( sSourceType );
	if( !ret )
	{
		LOG.Write( ssprintf( "Could not create new sound source: [%s]:[%s]\n", sSourceType.c_str(), sSourceName.c_str() ) );
		return;
	}

	ret->Load( sSourceName );

	AttachSound( ret, bSyncronize );
}

int MercurySoundManager::RegisterSoundSource( const char * SourceType, MercurySoundSource * (*s)( MAutoPtr< MercurySoundSource > ) )
{
	static int soundsourceidx = 0;
	m_SoundSourceCreators[SourceType] = s;
	return soundsourceidx++;
}


void MercurySoundManager::AttachSound( MAutoPtr< MercurySoundSource > s, bool bSynchronize )
{
	if( !fSPS ) return;
	if( bSynchronize ) s->SampleHoldCalc();
	m_vpChildren.push_back( s );
}

void MercurySoundManager::DetachSound( MAutoPtr< MercurySoundSource > s )
{
	if( !fSPS ) return;
	for( unsigned i = 0; i < m_vpChildren.size(); i++ )
	{
		if( m_vpChildren[i] == s )
			m_vpChildren.remove( i );
	}
}

int MercurySoundManager::RegisterDriver( MercurySoundDriverConstructionPair s )
{
	static int driverno;
	m_Drivers.push_back( s );
	return driverno++;
}

void MercurySoundManager::FillBuffer( float * cBufferToFill, int iCount )
{
	//XXX: CONSIDER MUTEXING THIS AREA, AND USING THE SAME MUTEX IN SampleHoldCalcand Attac/Detach sound.

	float ftd = m_tLastTrip.Touch();
	iLastCountDifference = ftd * fSPS;

	for( int j = 0; j < iCount * iChannels; ++j )
		cBufferToFill[j] = 0;

	if( !m_vpChildren.size() )
		return;

	//If we have no sources, we have to clear the buffer.
	for( unsigned i = 0; i < m_vpChildren.size(); i++ )
	{
		MercurySoundSource * pc = m_vpChildren[i];

		pc->FillBuffer( cBufferToFill, iCount );
	}
}

void MercurySoundManager::PostFill()
{
	MVector< MAutoPtr< MercurySoundSource > > ChildrenCopy( m_vpChildren );

	for( unsigned i = 0; i < ChildrenCopy.size(); i++ )
	{
		if( !ChildrenCopy[i]->PostFill() )
			DetachSound( ChildrenCopy[i] );
	}
}


int MercurySoundManager::iChannels;
int MercurySoundManager::iLastCountDifference;
float MercurySoundManager::fSPS;


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


