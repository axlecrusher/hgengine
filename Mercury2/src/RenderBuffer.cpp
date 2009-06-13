#include <RenderBuffer.h>
#include <GLHeaders.h>
#include <MercuryWindow.h>
#include <RenderableNode.h>

#include <Texture.h>

REGISTER_ASSET_TYPE(RenderBuffer);

RenderBuffer::RenderBuffer()
	:MercuryAsset(), m_bufferID(0), m_textureID(0), m_initiated(false), m_width(0), m_height(0), m_type(NONE), m_bound(false)
{
}

RenderBuffer::~RenderBuffer()
{
	if (m_bufferID != 0) glDeleteRenderbuffersEXT(1, &m_bufferID);
}

void RenderBuffer::Init(MercuryNode* node)
{
	MercuryAsset::Init( node );
	
	RenderableNode* rn = RenderableNode::Cast( node );
	if ( rn )
	{
		rn->AddPreRender( this );
		rn->AddPostRender( this );
	}
}

void RenderBuffer::PreRender(const MercuryNode* node)
{
	if ( !m_initiated ) InitRenderBuffer();
}

void RenderBuffer::Render(const MercuryNode* node)
{
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_bufferID);
	
	if ( NeedResize() ) AllocateSpace();

	if ( m_type == TEXTURE )
	{
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GLAttachPoint(), GL_TEXTURE_2D, m_textureID, 0);
	}
	else
	{
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GLAttachPoint(), GL_RENDERBUFFER_EXT, m_bufferID);
	}
	CHECKFBO;
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT );
	GLERRORCHECK;
}

void RenderBuffer::PostRender(const MercuryNode* node)
{
	GLERRORCHECK;
	if ( m_type == TEXTURE )
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
			
		GLERRORCHECK;

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		
		GLERRORCHECK;

//		printf("active %d\n", Texture::NumberActiveTextures() );
		
		//this works with a "normal" texture, FBO texture is still white
		glActiveTexture( GL_TEXTURE0 );
		glClientActiveTextureARB(GL_TEXTURE0);
		glEnable( GL_TEXTURE_2D );
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

		GLERRORCHECK;

		glBegin(GL_QUADS);
		glTexCoord2d(0,1);
		glVertex3i(-1, -1, -1);
		
		glTexCoord2d(1,1);
		glVertex3i(1, -1, -1);
		
		glTexCoord2d(1,0);
		glVertex3i(1, 1, -1);
		
		glTexCoord2d(0,0);
		glVertex3i(-1, 1, -1);
		
		glEnd();
		
		GLERRORCHECK;
		
		glBindTexture(GL_TEXTURE_2D, 0);
//		glActiveTexture( GL_TEXTURE0 );
//		glClientActiveTextureARB(GL_TEXTURE0);
//		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable( GL_TEXTURE_2D );

		GLERRORCHECK;

		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	
		GLERRORCHECK;
	}
}

void RenderBuffer::InitRenderBuffer()
{
	m_initiated = true;
	glGenRenderbuffersEXT(1, &m_bufferID);
	CHECKFBO;
	GLERRORCHECK;
	if (m_type == TEXTURE) glGenTextures(1, &m_textureID);
	CHECKFBO;
	GLERRORCHECK;
}

void RenderBuffer::AllocateSpace()
{
	m_width = MercuryWindow::GetCurrentWindow()->Width();
	m_height = MercuryWindow::GetCurrentWindow()->Height();	
	m_width = 512;
	m_height = 512;

	if (m_type == TEXTURE)
	{
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}
	else
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GLType(), m_width, m_height);
	GLERRORCHECK;
}

bool RenderBuffer::NeedResize()
{
	int width = MercuryWindow::GetCurrentWindow()->Width();
	int height = MercuryWindow::GetCurrentWindow()->Height();	
	return (width != m_width)||(height != m_height);
}

int RenderBuffer::GLType()
{
	switch( m_type )
	{
		case TEXTURE:
		case COLOR:
			return GL_RGBA;
		case DEPTH:
			return GL_DEPTH_COMPONENT;
		case STENCIL:
			return GL_STENCIL_INDEX;
		default:
			return 0;
	}
}

int RenderBuffer::GLAttachPoint()
{
	switch( m_type )
	{
		case TEXTURE:
		case COLOR:
			return GL_COLOR_ATTACHMENT0_EXT;
		case DEPTH:
			return GL_DEPTH_ATTACHMENT_EXT;
		case STENCIL:
			return GL_STENCIL_ATTACHMENT_EXT;
		default:
			return 0;
	}	
}

void RenderBuffer::LoadFromXML(const XMLNode& node)
{
	MString t = node.Attribute("buffertype");
	
	if ( t == "color" )
	{
		m_type = COLOR;
	}
	else if ( t == "depth" )
	{
		m_type = DEPTH;
	}
	else if ( t == "stencil" )
	{
		m_type = STENCIL;
	}
	else if ( t == "texture" )
	{
		m_type = TEXTURE;
	}
}

RenderBuffer* RenderBuffer::Generate()
{
	return new RenderBuffer();
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
