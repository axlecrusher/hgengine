#include <Quad.h>

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glext.h>

#include <Texture.h>

REGISTER_ASSET_TYPE(Quad);

void Quad::Render(MercuryNode* node)
{
	unsigned short numTextures = Texture::NumberActiveTextures();
	unsigned int i;
	
	glBegin(GL_QUADS);
	
	for (i=0; i < numTextures; ++i)
		glMultiTexCoord2f(GL_TEXTURE0+i, 0, 0);
	glVertex3f(-0.5f, -0.5f,  0.0f);
	
	for (i=0; i < numTextures; ++i)
		glMultiTexCoord2f(GL_TEXTURE0+i, 1, 0);
	glVertex3f( 0.5f, -0.5f,  0.0f);
	
	for (i=0; i < numTextures; ++i)
		glMultiTexCoord2f(GL_TEXTURE0+i, 1, 1);
	glVertex3f( 0.5f,  0.5f,  0.0f);
	
//	glTexCoord2f(0,1);
	for (i=0; i < numTextures; ++i)
		glMultiTexCoord2f(GL_TEXTURE0+i, 0, 1);
	glVertex3f(-0.5f,  0.5f,  0.0f);

	glEnd();
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
