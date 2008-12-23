#include <MercuryVBO.h>

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glext.h>

#include <Texture.h>

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

MercuryVBO::MercuryVBO()
	:MercuryAsset(), m_vertexData(NULL), m_vMem(NULL), m_vertexIndexList(NULL), m_iMem(NULL), m_initiated(false)
{
	m_bufferIDs[0] = m_bufferIDs[1] = 0;
}

MercuryVBO::~MercuryVBO()
{
	SAFE_FREE(m_vMem);
	SAFE_FREE(m_iMem);
	
	if (m_bufferIDs[0]) glDeleteBuffersARB(2, m_bufferIDs);
	m_bufferIDs[0] = m_bufferIDs[1] = 0;
}

void MercuryVBO::Render(MercuryNode* node)
{
//	unsigned short numTextures = Texture::NumberActiveTextures();
	unsigned short stride = sizeof(float)*5;
	
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
		glVertexPointer(3, GL_FLOAT, stride, 0);
	
	//XXX This seems to apply texture coordinates to all active texture units
	glTexCoordPointer(2, GL_FLOAT, stride, BUFFER_OFFSET(sizeof(float)*3));

	glDrawRangeElements(GL_QUADS, 0, m_bufferLength[1], m_bufferLength[1], GL_UNSIGNED_SHORT, NULL);
	
	m_lastVBOrendered = this;
}

void MercuryVBO::InitVBO()
{
	glGenBuffersARB(2, m_bufferIDs);
	
	//vertex VBO
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_bufferIDs[0]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_bufferLength[0], m_vertexData, GL_STATIC_DRAW_ARB);

	//indices VBO
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_bufferIDs[1]);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(uint16_t)*m_bufferLength[1], m_vertexIndexList, GL_STATIC_DRAW_ARB);
	
	SAFE_FREE(m_vMem);
	SAFE_FREE(m_iMem);
	
	glEnableClientState(GL_VERTEX_ARRAY);

	m_initiated = true;
}

void MercuryVBO::AllocateVertexSpace(unsigned int count, unsigned int elementSize)
{
	SAFE_FREE(m_vMem);
	void* mem = NULL;
	m_vertexData = (char*)mmemalign(32, elementSize*count, mem);
	m_vMem = (char*)mem;
	m_bufferLength[0] = elementSize*count;
}

void MercuryVBO::AllocateIndexSpace(unsigned int count)
{
	SAFE_FREE(m_iMem);
	void* mem = NULL;
	m_vertexIndexList = (uint16_t*)mmemalign(32, sizeof(uint16_t)*count, mem);
	m_iMem = (uint16_t*)mem;
	m_bufferLength[1] = count;
}

void* MercuryVBO::m_lastVBOrendered = NULL;
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
