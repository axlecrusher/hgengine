#include <MercuryFBO.h>
#include <GLHeaders.h>

REGISTER_NODE_TYPE(MercuryFBO);

MercuryFBO::MercuryFBO()
	:m_fboID(0), m_depthBufferID(0), m_initiated(false), m_useDepth(false), m_numTextures(0)
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

	if( m_useDepth ) glGenRenderbuffersEXT( 1, &m_depthBufferID );
	glGenFramebuffersEXT( 1, &m_fboID );
	
	for (uint8_t i = 0; i < m_numTextures; ++i)
	{
		m_textures[i] = Texture::LoadDynamicTexture(m_name);
		m_textures[i]->MakeDynamic(m_width, m_height,m_name);
	}
	
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

	CHECKFBO; //Incomplete FBO
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
	RenderableNode::PreRender(matrix);
}

void MercuryFBO::Render(const MercuryMatrix& matrix)
{
	if (m_lastRendered != m_fboID)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboID);
//		CHECKFBO; //Incomplete FBO
		GLERRORCHECK;
		m_lastRendered = m_fboID;
//		m_lastInStask = m_lastRendered;
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0,0,m_width, m_width);
	
	RenderableNode::Render(matrix);
}

void MercuryFBO::PostRender(const MercuryMatrix& matrix)
{
	glPopAttrib();

//	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_lastInStask);
//	m_lastRendered = m_lastInStask;
	
	RenderableNode::PostRender(matrix);
	GLERRORCHECK;
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); //unbind
	CHECKFBO;
	GLERRORCHECK;
	
	m_lastRendered = 0;	
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
	
	RenderableNode::LoadFromXML(node);
}

uint32_t MercuryFBO::m_lastRendered = NULL;

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
