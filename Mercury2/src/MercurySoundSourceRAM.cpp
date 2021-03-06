#include <MercurySoundSourceRAM.h>

MercurySoundSourceRAM::MercurySoundSourceRAM( MercurySoundSource * chain ) : MercurySoundSource( chain ), m_iSoundPlace( 0 )
{
}

void MercurySoundSourceRAM::FillBuffer( float * cBufferToFill, int iCount )
{
	int iOffset = 0;

	if( m_iSampleHold > 0 )
	{
		iCount -= m_iSampleHold;

		//We're apparently advanced too far.
		if( iCount <= 0 )
			return;

		iOffset += m_iSampleHold * MercurySoundManager::iChannels;
	}
	int iFramesLeft = m_Sound->iSamples - m_iSoundPlace;	//First make it the frames left.

	int iCopyFrames = iFramesLeft;
	if( iFramesLeft > iCount )
		iCopyFrames = iCount;

	int placeBytes = m_iSoundPlace* MercurySoundManager::iChannels;

	for( int i = iOffset; i < iCopyFrames * MercurySoundManager::iChannels; i++ )
	{
		cBufferToFill[i] = m_Sound->fSound[placeBytes+i];
	}


	m_iSoundPlace += iCopyFrames;
}

bool MercurySoundSourceRAM::PostFill()
{
	return m_iSoundPlace < m_Sound->iSamples;
}

/****************************************************************************
 *   Copyright (C) 2009      by Charles Lohr                                *
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

