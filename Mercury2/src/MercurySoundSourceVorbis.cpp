#include <MercurySoundSourceVorbis.h>
#include <vorbis/vorbisfile.h>
#include <MercuryFile.h>
#include <MercuryLog.h>
#include <ogg/ogg.h>


REGISTER_SOUND_SOURCE( MercurySoundSourceVorbisRAM, "VorbisRAM" );

size_t vorbis_read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	MercuryFile * f = (MercuryFile*)datasource;
	return f->Read( ptr, size * nmemb );
}

int vorbis_seek_func(void *datasource, ogg_int64_t offset, int whence)
{
	MercuryFile * f = (MercuryFile*)datasource;
	switch( whence )
	{
	case SEEK_SET:
		return !f->Seek( offset );
	case SEEK_END:
		return !f->Seek( f->Length() - offset - 1 );
	case SEEK_CUR:
		return !f->Seek( f->Tell() + offset );
	default:
		LOG.Write( ssprintf( "Warning: OGG Decided to seek with invalid whence!" ) );
	}
	return -1;
}

int vorbis_close_func(void *datasource)
{
	return 0;
}

long vorbis_tell_func(void *datasource)
{
	MercuryFile * f = (MercuryFile*)datasource;
	return f->Tell();
}


MercurySoundSourceVorbisRAM::MercurySoundSourceVorbisRAM( MercurySoundSource * chain ) :
	MercurySoundSourceRAM( chain )
{
}

bool MercurySoundSourceVorbisRAM::Load( const MString & sDescriptor )
{
	unsigned Vorbistotal_bytes_read = 0;
	int VorbisAbstream;

	MAutoPtr< HGRawSound > r;
	MAutoPtr< HGRawSound > * g;
	if( ( g = g_SoundLibrary.get( sDescriptor ) ) )
	{
		m_Sound = *g;
		return true;
	}

	MercuryFile * f = FILEMAN.Open( sDescriptor );
	
	OggVorbis_File * vorbisFile = new OggVorbis_File;

	ov_callbacks vorbisCallbacks;
	vorbisCallbacks.read_func  = vorbis_read_func;
	vorbisCallbacks.seek_func  = vorbis_seek_func;
	vorbisCallbacks.close_func = vorbis_close_func;
	vorbisCallbacks.tell_func  = vorbis_tell_func;

	ov_open_callbacks( f, vorbisFile, NULL, 0, vorbisCallbacks );

	vorbis_info* info = ov_info(vorbisFile, -1);
	unsigned VorbisChannels = info->channels;
//	unsigned VorbisSamplerate = info->rate;
	unsigned VorbisSamples = ov_pcm_total( vorbisFile, 0 );

	unsigned Vorbisbytes_read;	

	if( VorbisSamples <= 0 )
	{
		delete f;
		delete vorbisFile;
		return false;
	}

	short * VorbisData = new short[VorbisSamples*VorbisChannels];

	while( (Vorbisbytes_read = ov_read(vorbisFile, ((char*)VorbisData) +
		Vorbistotal_bytes_read, VorbisSamples*VorbisChannels*2 - 
		Vorbistotal_bytes_read, 0, 2, 1, &VorbisAbstream)) > 0 )
	{
		if( VorbisAbstream == 0 )
			Vorbistotal_bytes_read+= Vorbisbytes_read;
	}

	r = new HGRawSound(new float[VorbisSamples*VorbisChannels],VorbisSamples);

	for( unsigned i = 0; i < VorbisSamples*VorbisChannels; i++ )
	{
		r->fSound[i] = ((float)VorbisData[i])/32768.0;
	}

	delete vorbisFile;
	delete f;

	m_Sound = r;
	g_SoundLibrary[sDescriptor] = r;
	return true;
}

MHash< MAutoPtr< HGRawSound > > MercurySoundSourceVorbisRAM::g_SoundLibrary;


//Now, the section on the regular MercurySoundSource
REGISTER_SOUND_SOURCE( MercurySoundSourceVorbis, "Vorbis" );

MercurySoundSourceVorbis::MercurySoundSourceVorbis( MercurySoundSource * chain ) :
	MercurySoundSource( chain ), iBufferSize( 32768 ), iBufferPlay(0), iBufferLoad(1)
{
	iBuffer = (short*)malloc( sizeof( short ) * iBufferSize * 2 );
}

MercurySoundSourceVorbis::~MercurySoundSourceVorbis()
{
	delete m_File;
	delete iBuffer;
	delete vorbisFile;

}

bool MercurySoundSourceVorbis::Load( const MString & sDescriptor )
{
	MAutoPtr< HGRawSound > r;
//	MAutoPtr< HGRawSound > * g;

	m_File = FILEMAN.Open( sDescriptor );
	
	vorbisFile = new OggVorbis_File;

	ov_callbacks vorbisCallbacks;
	vorbisCallbacks.read_func  = vorbis_read_func;
	vorbisCallbacks.seek_func  = vorbis_seek_func;
	vorbisCallbacks.close_func = vorbis_close_func;
	vorbisCallbacks.tell_func  = vorbis_tell_func;

	ov_open_callbacks( m_File, vorbisFile, NULL, 0, vorbisCallbacks );

//	vorbis_info* info = ov_info(vorbisFile, -1);
//	unsigned VorbisChannels = info->channels;
//	unsigned VorbisSamplerate = info->rate;

	unsigned VorbisSamples = ov_pcm_total( vorbisFile, 0 );

//	unsigned Vorbisbytes_read;	

	if( VorbisSamples <= 0 )
	{
		delete m_File;
		delete vorbisFile;
		return false;

	}
	return true;

}

void MercurySoundSourceVorbis::FillBuffer( float * cBufferToFill, int iCount )
{
	//Don't worry our circular queue is threadsafe.
	for( int i = 0; i < iCount; i++ )
	{
		if( PlayLeft() <= 2 ) break;
		cBufferToFill[i*2+0] = float(iBuffer[iBufferPlay*2+0])/32768.;
		cBufferToFill[i*2+1] = float(iBuffer[iBufferPlay*2+1])/32768.;
		iBufferPlay=(iBufferPlay+1)%iBufferSize;
	}
	//If we run out... that's okay.  It'll just be silent and have time to fill.
}

bool MercurySoundSourceVorbis::PostFill()
{
	unsigned Vorbistotal_bytes_read = 0;
	unsigned Vorbisbytes_read;
	int VorbisAbstream;

	unsigned long BF = BufferFree();
	unsigned long BFL = BF;
	short * tibuf = new short[BF * 2];

	do
	{
		Vorbisbytes_read = ov_read(vorbisFile, ((char*)&(tibuf[0]) + Vorbistotal_bytes_read), 
			BFL * 4, 0, 2, 1, &VorbisAbstream);
		BFL -= Vorbisbytes_read / 4;
		Vorbistotal_bytes_read += Vorbisbytes_read;
	} while( Vorbisbytes_read > 0 && BFL );
	if( Vorbisbytes_read < 0 )
	{
		delete tibuf;
		return false;
	}

	for( unsigned i = 0; i < BF; i++ )
	{
		iBuffer[iBufferLoad*2+0] = tibuf[i*2+0];
		iBuffer[iBufferLoad*2+1] = tibuf[i*2+1];
		iBufferLoad=(iBufferLoad+1)%iBufferSize;
	}

	if( Vorbisbytes_read == 0 && PlayLeft() == 0 )
	{
		delete tibuf;
		return false;
	}
	else
	{
		delete tibuf;
		return true;
	}
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


