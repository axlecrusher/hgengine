#include <GLHeaders.h>
#include <FullscreenQuad.h>

REGISTER_ASSET_TYPE(FullscreenQuad);

FullscreenQuad::FullscreenQuad()
{
	m_matrix = MercuryMatrix::Identity();
	m_matrix.Transotale(0,0,-1,0,0,0,2,2,0.01);
//	m_matrix.Scale(2,2,1);
	m_matrix.Transpose();
}

void FullscreenQuad::Render(const MercuryNode* node)
{
	GLCALL( glMatrixMode(GL_MODELVIEW) );
	GLCALL( glPushMatrix() );
//	GLCALL( glLoadIdentity() );
	GLCALL( glLoadMatrixf( m_matrix.Ptr() ) ); //this is OK

	GLCALL( glMatrixMode(GL_PROJECTION) );
	GLCALL( glPushMatrix() );
//	GLCALL( glLoadIdentity() );
	
	Quad::Render( node );
	
	GLCALL( glPopMatrix() );
	GLCALL( glMatrixMode(GL_MODELVIEW) );
	GLCALL( glPopMatrix() );
}

FullscreenQuad* FullscreenQuad::Generate()
{
	return new FullscreenQuad();
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
