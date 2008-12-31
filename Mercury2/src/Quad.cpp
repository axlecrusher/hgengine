#include <Quad.h>

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glext.h>

REGISTER_ASSET_TYPE(Quad);

Quad::Quad()
	:MercuryVBO()
{
	AllocateIndexSpace(6);
	AllocateVertexSpace(4);
	
//	float* buffer = m_vertexData.m_vertexData();
	int i = 0;
	
	m_vertexData[i++] = 0; m_vertexData[i++] = 1;
	m_vertexData[i++] = 0; m_vertexData[i++] = 0; m_vertexData[i++] = -1.0;
	m_vertexData[i++] = -0.5; m_vertexData[i++] = -0.5; m_vertexData[i++] = 0.0;
	
	m_vertexData[i++] = 1; m_vertexData[i++] = 1;
	m_vertexData[i++] = 0; m_vertexData[i++] = 0; m_vertexData[i++] = -1.0;
	m_vertexData[i++] = 0.5; m_vertexData[i++] = -0.5; m_vertexData[i++] = 0.0;
	
	m_vertexData[i++] = 1; m_vertexData[i++] = 0;
	m_vertexData[i++] = 0; m_vertexData[i++] = 0; m_vertexData[i++] = -1.0;
	m_vertexData[i++] = 0.5; m_vertexData[i++] = 0.5; m_vertexData[i++] = 0.0;
	
	m_vertexData[i++] = 0; m_vertexData[i++] = 0;
	m_vertexData[i++] = 0; m_vertexData[i++] = 0; m_vertexData[i++] = -1.0;
	m_vertexData[i++] = -0.5; m_vertexData[i++] = 0.5; m_vertexData[i++] = 0.0;
	
	m_indexData[5] = m_indexData[0] = 0;
	m_indexData[1] = 1;
	m_indexData[3] = m_indexData[2] = 2;
	m_indexData[4] = 3;
}

Quad::~Quad()
{
	REMOVE_ASSET_INSTANCE(Quad,"");
}

Quad* Quad::Generate()
{
	Quad *asset = new Quad();
	ADD_ASSET_INSTANCE(Quad,"",asset);
	printf("new quad\n");
	return asset;
}

/***************************************************************************
 *   Copyright (C) 2008 by Joshua Allen   *
 *      *
 *                                                                         *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: *
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. *
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. *
 *     * Neither the name of the <ORGANIZATION> nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission. *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR *
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR *
 *   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, *
 *   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,   *
 *   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR    *
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF *
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  *
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS    *
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.          *
 ***************************************************************************/
