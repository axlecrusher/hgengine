#include <MercuryFBO.h>
#include <GLHeaders.h>

REGISTER_NODE_TYPE(MercuryFBO);

MercuryFBO::MercuryFBO()
	:m_fboID(0), m_initiated(false)
{
}

MercuryFBO::~MercuryFBO()
{
	if (m_fboID != 0) glDeleteFramebuffersEXT(1, &m_fboID);
}

void MercuryFBO::InitFBOBeforeRender()
{
	m_initiated = true;
	glGenFramebuffersEXT(1, &m_fboID);
}

void MercuryFBO::PreRender(const MercuryMatrix& matrix)
{
	if ( !m_initiated ) InitFBOBeforeRender();
	RenderableNode::PreRender(matrix);
}

void MercuryFBO::Render(const MercuryMatrix& matrix)
{
	if (m_lastRendered != m_fboID)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboID);
		m_lastRendered = m_fboID;
//		m_lastInStask = m_lastRendered;
	}
	
//	glPushAttrib(GL_VIEWPORT_BIT);
	//	glViewport(0,0,width, height);
	
	RenderableNode::Render(matrix);
}

void MercuryFBO::PostRender(const MercuryMatrix& matrix)
{
//	glPopAttrib();

//	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_lastInStask);
//	m_lastRendered = m_lastInStask;
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); //unbind
	m_lastRendered = 0;
	
	RenderableNode::PostRender(matrix);
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
