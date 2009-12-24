#include <MercuryVBO.h>
#include <GLHeaders.h>
#include <Texture.h>

REGISTER_ASSET_TYPE(MercuryVBO);

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

extern bool SHOWBOUNDINGVOLUME;
extern bool SHOWAXISES;

MercuryVBO::MercuryVBO( const MString & key, bool bInstanced, bool useVertexColor )
	:MercuryAsset( key,  bInstanced ), m_initiated(false), m_useVertexColor(useVertexColor)
{
	m_bufferIDs[0] = m_bufferIDs[1] = m_bufferIDs[2] = 0;
	m_bDirtyIndices = m_bDirtyVertices = m_bDirtyVertexColor = false;
}

MercuryVBO::~MercuryVBO()
{
	if (m_bufferIDs[0] > 0) { GLCALL( glDeleteBuffersARB(2, m_bufferIDs) ); }
	if (m_bufferIDs[2] > 0) { GLCALL( glDeleteBuffersARB(1, &m_bufferIDs[2]) ); }
	m_bufferIDs[0] = m_bufferIDs[1] = m_bufferIDs[2] = 0;
}

void MercuryVBO::Render(const MercuryNode* node)
{
	
	if ( !m_initiated ) InitVBO();

	if ( this != GetLastRendered() )
	{
		SetLastRendered(this);

		if ( m_bDirtyVertices ) UpdateVertices();
		if( m_bDirtyIndices ) UpdateIndices();
		if( m_bDirtyVertexColor ) UpdateVertexColor();

		GLCALL( glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_bufferIDs[0]) );
		GLCALL( glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_bufferIDs[1]) );
		GLCALL( glVertexPointer(3, GL_FLOAT, STRIDE*sizeof(float), BUFFER_OFFSET( VERTEX_OFFSET*sizeof(float) ) ) );

		if (m_useVertexColor)
		{
			GLCALL( glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_bufferIDs[2]) );
			GLCALL( glColorPointer(4, GL_FLOAT, 0, 0 ) );
		}

		++m_vboBinds;
	}
	
	GLCALL( glPushAttrib(GL_CURRENT_BIT) );
	GLCALL( glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT) );

	if (m_useVertexColor) { GLCALL( glEnableClientState( GL_COLOR_ARRAY ) ); }
	
	Texture::ApplyActiveTextures(STRIDE*sizeof(float), 0);
	
	GLCALL( glEnableClientState( GL_NORMAL_ARRAY ) );
	GLCALL( glNormalPointer(GL_FLOAT, STRIDE*sizeof(float), BUFFER_OFFSET(sizeof(float)*2)) );

	GLCALL( glDrawRangeElements(GL_TRIANGLES, 0, m_indexData.Length()-1, m_indexData.Length(), GL_UNSIGNED_SHORT, NULL) );
	IncrementBatches();
		
	if (m_boundingVolume && SHOWBOUNDINGVOLUME) m_boundingVolume->Render();

	GLCALL( glPopClientAttrib() );
	GLCALL( glPopAttrib() );

	if ( SHOWAXISES ) DrawAxes();
}

void MercuryVBO::InitVBO()
{
	if (!m_bufferIDs[0]) { GLCALL( glGenBuffersARB(2, m_bufferIDs) ); }
	if (m_useVertexColor) { GLCALL( glGenBuffersARB(1, &m_bufferIDs[2]) ); }

	UpdateIndices();
	UpdateVertices();
	UpdateVertexColor();

	GLCALL( glEnableClientState(GL_VERTEX_ARRAY) );

	m_initiated = true;
}

void MercuryVBO::UpdateIndices()
{
	GLCALL( glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_bufferIDs[1]) );
	GLCALL( glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_indexData.LengthInBytes(), m_indexData.Buffer(), GL_STATIC_DRAW_ARB) );
	m_bDirtyIndices = false;
}

void MercuryVBO::UpdateVertices()
{
	GLCALL( glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_bufferIDs[0]) );
	GLCALL( glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_vertexData.LengthInBytes(), m_vertexData.Buffer(), GL_STATIC_DRAW_ARB) );
	m_bDirtyVertices = false;
}

void MercuryVBO::UpdateVertexColor()
{
	if (m_useVertexColor)
	{
		GLCALL( glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_bufferIDs[2]) );
		GLCALL( glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_vertexColorData.LengthInBytes(), m_vertexColorData.Buffer(), GL_STATIC_DRAW_ARB) );
	}
	m_bDirtyVertexColor = false;
}

void MercuryVBO::AllocateVertexSpace(unsigned int count)
{
	m_vertexData.Allocate(count*8);
	if (m_useVertexColor) m_vertexColorData.Allocate(count*4);
}

void MercuryVBO::AllocateIndexSpace(unsigned int count)
{
	m_indexData.Allocate(count);
}

void MercuryVBO::SetLastRendered(void* p)
{
	m_lastVBOrendered = p;
	++m_vboBinds;
}

void* MercuryVBO::m_lastVBOrendered = NULL;
uint32_t MercuryVBO::m_vboBatches = 0;
uint32_t MercuryVBO::m_vboBinds = 0;

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
