#include <Viewport.h>
#include <GLHeaders.h>
#include <MercuryWindow.h>

REGISTER_NODE_TYPE(Viewport);
const Frustum* FRUSTUM;
MercuryMatrix VIEWMATRIX;
MercuryVertex EYE;
MercuryVector LOOKAT;

Viewport::Viewport()
	:m_xFactor(1), m_yFactor(0.5), m_minx(0), m_miny(0)
{
}

void Viewport::PreRender(const MercuryMatrix& matrix)
{
	FRUSTUM = &m_frustum;
	
	MercuryWindow* w = MercuryWindow::GetCurrentWindow();
	glViewport(m_minx, m_miny, (GLsizei)(w->Width()*m_xFactor), (GLsizei)(w->Height()*m_yFactor));

	//Load the frustum into the projection
	//"eye" position does not go into projection
	glMatrixMode(GL_PROJECTION);
	MercuryMatrix f = m_frustum.GetMatrix();
	f.Transpose();
	glLoadMatrixf( f.Ptr() );
	
	glMatrixMode(GL_MODELVIEW);
	
	VIEWMATRIX = matrix;
	EYE[0] = matrix[0][3];
	EYE[1] = matrix[1][3];
	EYE[2] = matrix[2][3];
	EYE *= -1;
	
	MercuryVector l(0,0,1);
//	l += EYE;
	LOOKAT = (matrix * l).Normalize();
//	LOOKAT.Print();
}

void Viewport::LoadFromXML(const XMLNode& node)
{
	m_xFactor = StrToFloat(node.Attribute("xfactor"), 1.0f);
	m_yFactor = StrToFloat(node.Attribute("yfactor"), 1.0f);
	m_minx = StrToInt(node.Attribute("minx"), 0);
	m_miny = StrToInt(node.Attribute("miny"), 0);

	MercuryWindow* w = MercuryWindow::GetCurrentWindow();

	m_frustum.SetPerspective( StrToFloat(node.Attribute("fov")),
				  (w->Width()*m_xFactor)/(w->Height()*m_yFactor),
//							  StrToFloat(node.Attribute("aspect")),
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
