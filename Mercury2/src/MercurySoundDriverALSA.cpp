#include <MercuryThreads.h>
#include <MercuryUtil.h>
#include <MercuryLog.h>
#include "MercurySound.h"
#include <alsa/asoundlib.h>

//#define LOW_LATENCY

class MercurySoundDriverALSA
{
public:
	virtual bool Init( const MString & sParameters );
	virtual void Close();

	static snd_pcm_t *playback_handle;

	static void * playback_thread( void * );
	static int playback_function( int samples );
	MercuryThread tPlayback;
};

snd_pcm_t * MercurySoundDriverALSA::playback_handle;

bool MercurySoundDriverALSA::Init( const MString & sParameters )
{
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_sw_params_t *sw_params;
	int err;

	MString sDevice = sParameters.length()?sParameters:"default";
	unsigned int Samplerate = 44100;
	int Channels = 2;

	if ((err = snd_pcm_open (&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		fprintf (stderr, "cannot open audio device %s (%s)\n", 
			 sDevice.c_str(),
			 snd_strerror (err));
		return false;
	}
	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
			 snd_strerror (err));
		return false;
	}
			 
	if ((err = snd_pcm_hw_params_any (playback_handle, hw_params)) < 0) {
		fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
			 snd_strerror (err));
		return false;
	}

	if ((err = snd_pcm_hw_params_set_access (playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf (stderr, "cannot set access type (%s)\n",
			 snd_strerror (err));
		return false;
	}

	if ((err = snd_pcm_hw_params_set_format (playback_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
		fprintf (stderr, "cannot set sample format (%s)\n",
			 snd_strerror (err));
		return false;
	}

	if ((err = snd_pcm_hw_params_set_rate_near (playback_handle, hw_params, &Samplerate, 0)) < 0) {
		fprintf (stderr, "cannot set sample rate (%s)\n",
			 snd_strerror (err));
		return false;
	}

	if ((err = snd_pcm_hw_params_set_channels (playback_handle, hw_params, Channels)) < 0) {
		fprintf (stderr, "cannot set channel count (%s)\n",
			 snd_strerror (err));
		return false;
	}

	if ((err = snd_pcm_hw_params (playback_handle, hw_params)) < 0) {
		fprintf (stderr, "cannot set parameters (%s)\n",
			 snd_strerror (err));
		return false;
	}


	snd_pcm_hw_params_free (hw_params);

	//Time for software parameters:

	if ((err = snd_pcm_sw_params_malloc (&sw_params)) < 0) {
		fprintf (stderr, "cannot allocate software parameters structure (%s)\n",
			 snd_strerror (err));
		return false;
	}
	if ((err = snd_pcm_sw_params_current (playback_handle, sw_params)) < 0) {
		fprintf (stderr, "cannot initialize software parameters structure (%s)\n",
			 snd_strerror (err));
		return false;
	}
	if ((err = snd_pcm_sw_params_set_avail_min (playback_handle, sw_params, 4096)) < 0) {
		fprintf (stderr, "cannot set minimum available count (%s)\n",
			 snd_strerror (err));
		return false;
	}
	if ((err = snd_pcm_sw_params_set_stop_threshold(playback_handle, sw_params, 1024)) < 0) {
		fprintf (stderr, "cannot set minimum available count (%s)\n",
			 snd_strerror (err));
		return false;
	}
	if ((err = snd_pcm_sw_params_set_start_threshold(playback_handle, sw_params, 4096)) < 0) {
		fprintf (stderr, "cannot set minimum available count (%s)\n",
			 snd_strerror (err));
		return false;
	}
	if ((err = snd_pcm_sw_params (playback_handle, sw_params)) < 0) {
		fprintf (stderr, "cannot set software parameters (%s)\n",
			 snd_strerror (err));
		return false;
	}




	if ((err = snd_pcm_prepare (playback_handle)) < 0) {
		fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
			 snd_strerror (err));
		return false;
	}

	LOG.Write( ssprintf( "Got ALSA Device at %d SPS\n", Samplerate ) );

	MercurySoundManager::iChannels = Channels;
	MercurySoundManager::fSPS = Samplerate;

	tPlayback.Create( playback_thread, 0 );

	return true;
}

void MercurySoundDriverALSA::Close()
{
	tPlayback.Halt( true );
	if( playback_handle )
		snd_pcm_close (playback_handle);
}

void * MercurySoundDriverALSA::playback_thread( void * )
{
	int err;
	while (1) {
		int frames_to_deliver;		

#ifdef LOW_LATENCY
		usleep( 4000 );
#else
		if ((err = snd_pcm_wait (playback_handle, 1000)) < 0) {
		        fprintf (stderr, "poll failed (%s)\n", strerror (errno));
			if ((err = snd_pcm_prepare (playback_handle)) < 0) {
				fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
					 snd_strerror (err));
				break;
			}
		}
#endif

		if ((frames_to_deliver = snd_pcm_avail (playback_handle)) < 0) {
			if (frames_to_deliver == -EPIPE) {
				fprintf (stderr, "an xrun occured\n");
				if ((err = snd_pcm_prepare (playback_handle)) < 0) {
					fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
						 snd_strerror (err));
					break;
				}
			} else {
				fprintf (stderr, "unknown ALSA avail update return value (%d)\n", 
					 frames_to_deliver);
				break;
			}
		}

//This does some nasty tricks to try to get less delay.
#ifdef LOW_LATENCY
		snd_pcm_sframes_t delayp;
		snd_pcm_avail_update( playback_handle );
		snd_pcm_delay( playback_handle, &delayp );
		frames_to_deliver = 8192 - delayp;
#endif

		frames_to_deliver = frames_to_deliver > 4096 ? 4096 : frames_to_deliver;
		if( frames_to_deliver < 256 ) continue;

		if (playback_function(frames_to_deliver) != frames_to_deliver) {
		        fprintf (stderr, "playback callback failed\n");
			if ((err = snd_pcm_prepare (playback_handle)) < 0) {
				fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
					 snd_strerror (err));
				break;
			}
		}
	}

	return 0;
}

int MercurySoundDriverALSA::playback_function(int nrframes)
{
	int err;
	short buf[nrframes*2];
	float ibuf[nrframes*2];

	SOUNDMAN->FillBuffer( ibuf, nrframes );

	for( int i = 0; i < nrframes; ++i )
	{
		float fi = ibuf[i*2+0];
		float fb = ibuf[i*2+1];
		buf[i*2+0] = (fi <= -1)?(-32767):( (fi >= 1 )?32767: (fi*32767) );
		buf[i*2+1] = (fb <= -1)?(-32767):( (fb >= 1 )?32767: (fb*32767) );
	}

	if ((err = snd_pcm_writei (playback_handle, buf, nrframes)) < 0) {
		fprintf (stderr, "write failed (%s)\n", snd_strerror (err));
	}

	SOUNDMAN->PostFill();

	return err;
}


REGISTER_SOUND_DRIVER( MercurySoundDriverALSA, 5 );

/****************************************************************************
 *   Copyright (C) 2008-2009 by Charles Lohr                                *
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

