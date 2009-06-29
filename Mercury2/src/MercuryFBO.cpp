#include <MercuryFBO.h>
#include <GLHeaders.h>
#include <MercuryWindow.h>
#include <assert.h>

REGISTER_NODE_TYPE(MercuryFBO);

MercuryFBO::MercuryFBO()
	:m_fboID(0), m_depthBufferID(0), m_initiated(false), m_useDepth(false),m_useScreenSize(false), m_width(0),m_height(0), m_numTextures(0)
{
	for (uint8_t i = 0; i < 4; ++i) m_textures[i] = NULL;
}

MercuryFBO::~MercuryFBO()
{
	Clean();
}

void MercuryFBO::Clean()
{
	if (m_fboID != 0) glDeleteFramebuffersEXT(1, &m_fboID);
	if (m_depthBufferID != 0) glDeleteRenderbuffersEXT( 1, &m_depthBufferID );
	m_fboID = m_depthBufferID = 0;
	m_initiated = false;
	for (uint8_t i = 0; i < 4; ++i) m_textures[i] = NULL;
}

void MercuryFBO::Setup()
{
	Clean();
	
	m_initiated = true;

	GenerateFBO();
	Bind();

//	CHECKFBO; //Incomplete FBO
}

void MercuryFBO::GenerateFBO()
{
	if( m_useDepth ) glGenRenderbuffersEXT( 1, &m_depthBufferID );
	glGenFramebuffersEXT( 1, &m_fboID );
	
	for (uint8_t i = 0; i < m_numTextures; ++i)
	{
		MString n = ssprintf("%s_%d", m_name.c_str(), i);
		m_textures[i] = Texture::LoadDynamicTexture(n);
		m_textures[i]->MakeDynamic(m_width, m_height,n);
	}
}

void MercuryFBO::Bind()
{
	if( m_useDepth )
	{
		glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, m_fboID );
		glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, m_width, m_height );
	}
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_fboID );
	
	for (uint8_t i = 0; i < m_numTextures; ++i)
		glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, m_textures[i]->TextureID(), 0 );

	if( m_useDepth )
		glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_depthBufferID );
}


/*
void MercuryFBO::InitFBOBeforeRender()
{
	m_initiated = true;
	glGenFramebuffersEXT(1, &m_fboID);
	CHECKFBO;
	GLERRORCHECK;
}
*/
void MercuryFBO::PreRender(const MercuryMatrix& matrix)
{
	if ( !m_initiated ) Setup();
	
	if ( m_useScreenSize )
	{
		int w = MercuryWindow::GetCurrentWindow()->Width();
		int h = MercuryWindow::GetCurrentWindow()->Height();
		if ((m_width != w) || (m_height != h))
		{	
			m_width = w; m_height = h;
			for (uint8_t i = 0; i < m_numTextures; ++i)
			{
				MString n = ssprintf("%s_%d", m_name.c_str(), i);
				m_textures[i]->MakeDynamic(m_width, m_height,n);
			}
		}
		Bind();
		CHECKFBO; //Incomplete FBO
		GLERRORCHECK;
	}

	MercuryNode::PreRender(matrix);
}

void MercuryFBO::Render(const MercuryMatrix& matrix)
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboID);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLERRORCHECK;

	const GLenum buffers[8] = {   GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT,
		GL_COLOR_ATTACHMENT3_EXT, GL_COLOR_ATTACHMENT4_EXT, GL_COLOR_ATTACHMENT5_EXT,
		GL_COLOR_ATTACHMENT6_EXT, GL_COLOR_ATTACHMENT7_EXT };

	glDrawBuffersARB( m_numTextures, buffers );

	glPushAttrib(GL_VIEWPORT_BIT);
	
	if ( !m_useScreenSize ) glViewport(0,0,m_width, m_height);
	
	GLERRORCHECK;
	MercuryNode::Render(matrix);
	GLERRORCHECK;
}

void MercuryFBO::PostRender(const MercuryMatrix& matrix)
{
	glPopAttrib();
	
	MercuryNode::PostRender(matrix);
	
//	for( uint8_t i = 0; i < m_numTextures; i++ )
//	{
//		glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, 0, 0 );
//		glActiveTextureARB( GL_TEXTURE0_ARB + i );
//		glDisable( GL_TEXTURE_2D );
//	}
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
//	glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, 0 );

	CHECKFBO;
	GLERRORCHECK;
	
//	m_lastRendered = 0;	
}

void MercuryFBO::LoadFromXML(const XMLNode& node)
{
	if ( !node.Attribute("width").empty() )
		SetWidth( StrToInt(node.Attribute("width")) );

	if ( !node.Attribute("height").empty() )
		SetHeight( StrToInt(node.Attribute("height")) );

	if ( !node.Attribute("depth").empty() )
		SetUseDepth( node.Attribute("depth") == "true"?true:false );

	if ( !node.Attribute("tnum").empty() )
		SetNumTextures( StrToInt(node.Attribute("tnum")) );
	
	if ( !node.Attribute("usescreensize").empty() )
		m_useScreenSize = node.Attribute("usescreensize") == "true"?true:false;
	
	MercuryNode::LoadFromXML(node);
}

//uint32_t MercuryFBO::m_lastRendered = NULL;

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
