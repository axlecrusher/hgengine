#include <MercuryVBO.h>

#include <GLHeaders.h>


#include <Texture.h>

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

extern bool SHOWBOUNDINGVOLUME;

MercuryVBO::MercuryVBO()
	:MercuryAsset(), m_initiated(false)
{
	m_bufferIDs[0] = m_bufferIDs[1] = 0;
}

MercuryVBO::~MercuryVBO()
{
	if (m_bufferIDs[0]) glDeleteBuffersARB(2, m_bufferIDs);
	m_bufferIDs[0] = m_bufferIDs[1] = 0;
}

void MercuryVBO::Render(const MercuryNode* node)
{
	uint8_t numTextures = Texture::NumberActiveTextures();
	uint16_t stride = sizeof(float)*8;
	
	if ( m_initiated )
	{
		if ( this != m_lastVBOrendered)
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_bufferIDs[0]);
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_bufferIDs[1]);
		}
	}
	else
		InitVBO();
		
	if ( this != m_lastVBOrendered)
	{		
		glVertexPointer(3, GL_FLOAT, stride, BUFFER_OFFSET(sizeof(float)*5));
		++m_vboBinds;
	}
	
	//apply all the active Textures
	for (uint8_t i = 0; i < numTextures; ++i)
	{
		glActiveTexture( GL_TEXTURE0+i );
		glClientActiveTextureARB(GL_TEXTURE0+i);
		glTexCoordPointer(2, GL_FLOAT, stride, BUFFER_OFFSET(sizeof(float)*0));
	}

	glDrawRangeElements(GL_TRIANGLES, 0, m_indexData.Length()-1, m_indexData.Length(), GL_UNSIGNED_SHORT, NULL);
	m_vboBatches++;
	
	m_lastVBOrendered = this;
	
	if (m_boundingVolume && SHOWBOUNDINGVOLUME) m_boundingVolume->Render();
	DrawAxes();
}

void MercuryVBO::InitVBO()
{
	glGenBuffersARB(2, m_bufferIDs);
	
	//vertex VBO
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_bufferIDs[0]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_vertexData.LengthInBytes(), m_vertexData.Buffer(), GL_STATIC_DRAW_ARB);

	//indices VBO
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_bufferIDs[1]);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_indexData.LengthInBytes(), m_indexData.Buffer(), GL_STATIC_DRAW_ARB);
		
	glEnableClientState(GL_VERTEX_ARRAY);

	m_initiated = true;
}

void MercuryVBO::AllocateVertexSpace(unsigned int count)
{
	m_vertexData.Allocate(count*8);
}

void MercuryVBO::AllocateIndexSpace(unsigned int count)
{
	m_indexData.Allocate(count);
}

void* MercuryVBO::m_lastVBOrendered = NULL;
uint32_t MercuryVBO::m_vboBatches;
uint32_t MercuryVBO::m_vboBinds;

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
