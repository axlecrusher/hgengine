#ifndef _MERCURY_SOUND_SOURCE_VORBIS
#define _MERCURY_SOUND_SOURCE_VORBIS

#include <MercurySoundSourceRAM.h>

///Class for small, cached sounds - once a sound is played with this, it will be stored in
///an uncompressed library.
class MercurySoundSourceVorbisRAM : public MercurySoundSourceRAM
{
public:
	MercurySoundSourceVorbisRAM( MercurySoundSource * chain = 0 );

	virtual bool Load( const MString & sDescriptor );
protected:
	static MHash< MAutoPtr< HGRawSound > > g_SoundLibrary;
};

/*
class OggVorbis_File;
class vorbis_info;
class MercuryFile;

///Traditional vorbis decoder, more geared for music or background sound.
class MercurySoundSourceVorbis : public MercurySoundSource
{
public:
	MercurySoundSourceVorbis( MercurySoundSource * chain = 0 );
	~MercurySoundSourceVorbis( );


	virtual void FillBuffer( float * cBufferToFill, int iCount );
	virtual bool PostFill();
	virtual bool Load( const MString & sDescriptor );
protected:
	MercuryFile * m_File;
	OggVorbis_File * vorbisFile;
	vorbis_info * info
};
*/

#endif

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


