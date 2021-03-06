#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <RawImageData.h>
#include <MercuryFile.h>
#include <Callback.h>
#include <list>
#include <MercuryUtil.h>

#include <MercuryAsset.h>

class ImageLoader
{
	public:
		static ImageLoader& GetInstance();
		bool RegisterFactoryCallback(const MString& type, Callback1R< MercuryFile *, RawImageData* >);
		RawImageData* LoadImage(const MString& filename);
		void LoadImageThreaded(MercuryAsset* t);
		
	private:
		static void* ImageLoaderThread(void* d);
		std::list< std::pair< MString, Callback1R< MercuryFile*, RawImageData* > > > m_factoryCallbacks;		
};

static InstanceCounter<ImageLoader> ILcounter("ImageLoader");

#define REGISTER_IMAGE_TYPE(fingerprint,functor)\
	Callback1R< MercuryFile*, RawImageData* > factoryclbk##functor( functor ); \
	bool GlobalImageRegisterSuccess##functor = ImageLoader::GetInstance().RegisterFactoryCallback(#fingerprint, factoryclbk##functor);


#endif

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
