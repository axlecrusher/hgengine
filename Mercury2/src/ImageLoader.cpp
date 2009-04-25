#include <ImageLoader.h>
#include <MercuryUtil.h>
#include <Texture.h>

using namespace std;

void* ImageLoader::ImageLoaderThread(void* d)
{
	ThreadData *pd = (ThreadData*)d;
	ThreadData data = *pd;
	delete pd;
	RawImageData* imageData = data.imageloader->LoadImage( data.filename );
	((Texture*)data.asset)->SetRawData(imageData);
	data.asset->LoadedCallback();
	return 0;
}

ImageLoader& ImageLoader::GetInstance()
{
	static ImageLoader* instance = NULL;
	if (!instance)
		instance = new ImageLoader;
	return *instance;
}

bool ImageLoader::RegisterFactoryCallback(const MString& type, Callback1R< MercuryFile*, RawImageData* > functor)
{
	MString t = ToUpper( type );
	std::pair<MString, Callback1R< MercuryFile*, RawImageData* > > pp(t, functor);
	m_factoryCallbacks.push_back( pp );
	return true;
}

RawImageData* ImageLoader::LoadImage(const MString& filename)
{
	MercuryFile* f = FILEMAN.Open( filename );
	char fingerprint[4];
	fingerprint[3] = 0;
	
	if( !f )
	{
		printf( "Error opening image: %s\n", filename.c_str() );
		return 0;
	}

	f->Read( fingerprint, 3 );
	f->Seek( 0 );
	
	MString t(fingerprint);// = ToUpper( type );
	std::list< std::pair< MString, Callback1R< MercuryFile*, RawImageData* > > >::iterator i;
	for (i = m_factoryCallbacks.begin(); i != m_factoryCallbacks.end(); ++i)
	{
		if (i->first == t)
		{
			RawImageData* d = i->second(f);
			delete f;
			return d;
		}
	}
	delete f;
	return NULL;
}

void ImageLoader::LoadImageThreaded(MercuryAsset* t, const MString& filename)
{
	ThreadData* data = new ThreadData(this, t, filename);
	MercuryThread loaderThread;
	loaderThread.HaltOnDestroy(false);
	loaderThread.Create( ImageLoader::ImageLoaderThread, data, true );
}

/****************************************************************************
 *   Copyright (C) 2008 by Joshua Allen                                     *
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
