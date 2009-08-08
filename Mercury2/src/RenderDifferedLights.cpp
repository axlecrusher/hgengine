#include <RenderDifferedLights.h>
#include <RenderGraph.h>

#include <Texture.h>
#include <GLHeaders.h>

REGISTER_ASSET_TYPE(RenderDifferedLights);
#define BUFFER_OFFSET(i) ((char*)NULL + (i))

RenderDifferedLights::RenderDifferedLights()
{
}

RenderDifferedLights::~RenderDifferedLights()
{
}

void RenderDifferedLights::Render(const MercuryNode* node)
{
	uint8_t numTextures = Texture::NumberActiveTextures();
	uint16_t stride = sizeof(float)*8;
	
	//apply all the active Textures
	for (uint8_t i = 0; i < numTextures; ++i)
	{
		glActiveTexture( GL_TEXTURE0+i );
		glClientActiveTextureARB(GL_TEXTURE0+i);
		glTexCoordPointer(2, GL_FLOAT, stride, BUFFER_OFFSET(sizeof(float)*0));
	}

	glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_POLYGON_BIT);
	glCullFace(GL_FRONT);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(false);
	glBlendFunc(GL_ONE, GL_ONE);

	CURRENTRENDERGRAPH->DoDifferedLightPass();
	
	glPopAttrib( );
}

RenderDifferedLights* RenderDifferedLights::Generate()
{
	return new RenderDifferedLights();
}


/****************************************************************************
 *   Copyright (C) 2009 by Joshua Allen                                     *
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
