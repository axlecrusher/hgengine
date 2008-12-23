#include <Quad.h>

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glext.h>

REGISTER_ASSET_TYPE(Quad);

Quad::Quad()
{
	AllocateIndexSpace(6);
	AllocateVertexSpace(4, sizeof(float)*5);
	
	float* buffer = (float*)Buffer();
	int i = 0;
	
	buffer[i++] = -0.5; buffer[i++] = -0.5; buffer[i++] = 0.0;
	buffer[i++] = 0; buffer[i++] = 1;
	
	buffer[i++] = 0.5; buffer[i++] = -0.5; buffer[i++] = 0.0;
	buffer[i++] = 1; buffer[i++] = 1;
	
	buffer[i++] = 0.5; buffer[i++] = 0.5; buffer[i++] = 0.0;
	buffer[i++] = 1; buffer[i++] = 0;
	
	buffer[i++] = -0.5; buffer[i++] = 0.5; buffer[i++] = 0.0;
	buffer[i++] = 0; buffer[i++] = 0;
	
	uint16_t* indice = IndexBuffer();
	indice[0] = 0;
	indice[1] = 1;
	indice[2] = 2;

	indice[3] = 2;
	indice[4] = 3;
	indice[5] = 0;
}

Quad::~Quad()
{
	m_myInstance = NULL;
}

Quad* Quad::Generate()
{
	if ( !m_myInstance )
		m_myInstance = new Quad();
	return m_myInstance;
}

Quad* Quad::m_myInstance = NULL;

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
