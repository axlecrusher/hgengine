#include <Viewport.h>
#include <GLHeaders.h>
#include <MercuryWindow.h>
#include <TransformNode.h>

#include <Shader.h>

REGISTER_NODE_TYPE(Viewport);
const Frustum* FRUSTUM;
MercuryMatrix VIEWMATRIX;
MercuryVertex EYE;
MercuryVector LOOKAT;

#include <RenderGraph.h>

Viewport::Viewport()
	:m_xFactor(1), m_yFactor(0.5), m_minx(0), m_miny(0)
{
}

void Viewport::GoAll( const float fDtime )
{
	MercuryNode * n;
	int depth = 0;

	if( NeedsRebuild() )
	{
		printf( "Xxxx\n" );
	}

	//Update pass
	n = this;
	while( n )
	{
		n->Update( fDtime );
		n = n->TraversalNextNode( this, depth );
	}

	//Prerender pass
	n = this;
	while( n )
	{
		const MercuryMatrix& matrix = n->FindGlobalMatrix();

//		TransformNode * tn = dynamic_cast< TransformNode * >( n );
//		if( tn )
//			tn->m_modelView = tn->ManipulateMatrix( matrix );

		n->PreRender( matrix );
		n = n->TraversalNextNode( this, depth );
	}

	n = this;
	while( n )
	{
		const MercuryMatrix& matrix = n->FindGlobalMatrix();
		const MercuryMatrix& modelView = n->FindModelViewMatrix(); //get the one computed in the last transform

		glLoadMatrix( modelView );

		ShaderAttribute sa;
		sa.type = ShaderAttribute::TYPE_MATRIX;
		sa.value.matrix = matrix.Ptr();
		Shader::SetAttribute("HG_ModelMatrix", sa);
	
		n->Render( modelView );

		glLoadMatrix( modelView );
		Shader::SetAttribute("HG_ModelMatrix", sa);

		n->PostRender( modelView );

		n = n->TraversalNextNode( this, depth );
	}


}


void Viewport::PreRender(const MercuryMatrix& matrix)
{
	FRUSTUM = &m_frustum;
	
	MercuryWindow* w = MercuryWindow::GetCurrentWindow();
	glViewport(m_minx, m_miny, (GLsizei)(w->Width()*m_xFactor), (GLsizei)(w->Height()*m_yFactor));

	//Load the frustum into the projection
	//"eye" position does not go into projection
	glMatrixMode(GL_PROJECTION);
	glLoadMatrix( m_frustum.GetMatrix() );
	
	glMatrixMode(GL_MODELVIEW);
	
	//Sets up the clipping frustum
	m_frustum.LookAt(EYE, LOOKAT, MercuryVertex(0,1,0));
}

void Viewport::Render(const MercuryMatrix& matrix)
{
	FRUSTUM = &m_frustum;
	
	MercuryWindow* w = MercuryWindow::GetCurrentWindow();
	glViewport(m_minx, m_miny, (GLsizei)(w->Width()*m_xFactor), (GLsizei)(w->Height()*m_yFactor));

	//Load the frustum into the projection
	//"eye" position does not go into projection
	glMatrixMode(GL_PROJECTION);
	glLoadMatrix( m_frustum.GetMatrix() );
	
	glMatrixMode(GL_MODELVIEW);
	
	ShaderAttribute sa;
	sa.type = ShaderAttribute::TYPE_INT4;
	glGetIntegerv(GL_VIEWPORT, sa.value.iInts);
	Shader::SetAttribute("HG_ViewPort", sa);

	sa.type = ShaderAttribute::TYPE_FLOATV4;
	sa.value.fFloatV4[0] = m_frustum.ZNear();
	sa.value.fFloatV4[1] = m_frustum.ZFar();
	sa.value.fFloatV4[2] = m_frustum.DepthRange();
	sa.value.fFloatV4[3] = 1.0f/m_frustum.DepthRange();
	Shader::SetAttribute("HG_DepthRange", sa);
	
	sa.type = ShaderAttribute::TYPE_FLOATV4;
	sa.value.fFloatV4[0] = m_frustum.ZNear();
	sa.value.fFloatV4[1] = m_frustum.ZFar();
	sa.value.fFloatV4[2] = m_frustum.DepthRange();
	sa.value.fFloatV4[3] = 1.0f/m_frustum.DepthRange();
	Shader::SetAttribute("HG_DepthRange", sa);

	sa.type = ShaderAttribute::TYPE_FLOATV4;
	m_frustum.GetPlane(PNEAR).GetCenter().ConvertToVector3( sa.value.fFloatV4 );
	Shader::SetAttribute("HG_NearClip", sa);
	
	sa.type = ShaderAttribute::TYPE_FLOATV4;
	m_frustum.GetPlane(PFAR).GetCenter().ConvertToVector3( sa.value.fFloatV4 );
	Shader::SetAttribute("HG_FarClip", sa);

	sa.type = ShaderAttribute::TYPE_FLOATV4;
	sa.value.fFloatV4[0] = m_frustum.NearWidth()*0.5f;
	sa.value.fFloatV4[1] = m_frustum.NearHeight()*0.5f;
	sa.value.fFloatV4[2] = m_frustum.FarWidth()*0.5f;
	sa.value.fFloatV4[3] = m_frustum.FarHeight()*0.5f;
	Shader::SetAttribute("HG_ClipExtends", sa);
}

void Viewport::PostRender(const MercuryMatrix& matrix)
{
	glPushMatrix();
//	glLoadIdentity();
	glLoadMatrix( matrix );
	CURRENTRENDERGRAPH->RenderAlpha();
	glPopMatrix();
	MercuryNode::PostRender(matrix);
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
	
	MercuryNode::LoadFromXML(node);
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
