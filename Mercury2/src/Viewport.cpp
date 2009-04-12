#include <Viewport.h>
#include <GL/gl.h>

REGISTER_NODE_TYPE(Viewport);
const Frustum* FRUSTUM;

void Viewport::Render(const MercuryMatrix& matrix)
{
	FRUSTUM = &m_frustum;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	
	MercuryMatrix f = m_frustum.GetMatrix();
	f.Transpose();

	glLoadMatrixf( (matrix * f).Ptr() );
	//The following 2 are equivelent to above
//	glLoadMatrixf( m_frustum.Ptr() );
//	glMultMatrixf( m.Ptr() );
	
	glMatrixMode(GL_MODELVIEW);
}

void Viewport::LoadFromXML(const XMLNode& node)
{
	m_frustum.SetPerspective( StrToFloat(node.Attribute("fov")),
							  StrToFloat(node.Attribute("aspect")),
							  StrToFloat(node.Attribute("near")),
							  StrToFloat(node.Attribute("far")));
	
	m_frustum.LookAt(MercuryVertex(), MercuryVertex(0,0,1), MercuryVertex(0,1,0));
	
	RenderableNode::LoadFromXML(node);
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
