#include <Light.h>
#include <BoundingBox.h>
#include <MercuryNode.h>

#include <RenderGraph.h>
#include <GLHeaders.h>

#include <Shader.h>

REGISTER_NODE_TYPE(Light);

Light::Light()
	:MercuryNode(),  m_fullScreenQuad( "", true ), m_boundingVolume( NULL )
{
	m_atten[0] = m_atten[1] = m_atten[2] = 0.0f;
	m_color[0] = m_color[1] = m_color[2] = 1.0f;
	m_radius = 0.0f;
	m_fullscreen = false;
	m_power = 1.0f;
}

Light::~Light()
{
}

void Light::PreRender(const MercuryMatrix& matrix)
{
	SetCulled( m_boundingVolume->DoFrustumTest( matrix ) );
}

void Light::Render(const MercuryMatrix& matrix)
{
	m_worldPosition = FindModelViewMatrix();
	m_worldPosition2 = FindGlobalMatrix();
	CURRENTRENDERGRAPH->AddDifferedLight( this );
//	m_boundingVolume->Render();
//	m_parent = node;
}

void Light::LoadFromXML(const XMLNode& node)
{
	if ( !node.Attribute("atten").empty() )
		StrTo3Float(node.Attribute("atten"), m_atten);
	
	ComputeRadius();

	if ( !node.Attribute("power").empty() )
		m_power = StrToFloat(node.Attribute("power"), 1.0);

	if ( !node.Attribute("fullscreen").empty() )
		m_fullscreen = node.Attribute("fullscreen")=="true"?true:false;

	if ( !node.Attribute("radius").empty() )
		m_radius = StrToFloat(node.Attribute("radius"), 1.0);

	if ( !node.Attribute("shader").empty() )
	{
		MString key = node.Attribute("shader");
		MAutoPtr< MercuryAsset > asset( AssetFactory::GetInstance().Generate( "shader", key ) );
		if ( asset.IsValid() )
		{
			Shader* shader = dynamic_cast<Shader*>( asset.Ptr() );
			shader->ChangeKey(key);
			SetShader( shader );
		}

		m_fullscreen = node.Attribute("fullscreen")=="true"?true:false;
	}	
	
	BuildBoundingBox();
	
	MercuryNode::LoadFromXML( node );
}

void Light::StrTo3Float(const MString& s, float* a)
{
	sscanf(s.c_str(), "%f,%f,%f", &a[0], &a[1], &a[2]);
}

Light* Light::Generate()
{
	return new Light();
}

void Light::ComputeRadius()
{
	//300 ensures that RGB of 255 reaches 0
	//at 50, 255 is about 5. Close enough for me
	const float maxDenom = 50;
	float a = m_atten[2]; //quadratic
	float b = m_atten[1]; //linear
	float c = m_atten[0]; //constant
	float d = 0;

	if ((a == 0)&&(b == 0))
	{
		d = 1000.0;
	}
	else if (a == 0)
	{
		d = (maxDenom - c)/b;
	}
	else if (b == 0)
	{
		d = SQRT((maxDenom - c)/a);
	}
	else
	{
		//reciprocal of the quadratic equation
		float bottom = 2*a;
		float s = SQRT((b*b)-(4*a*c));
		float x1 = ((-b) - s)/bottom;
		float x2 = ((-b) + s)/bottom;
		x1 = x1>=0?x1:-x1;
		x2 = x2>=0?x2:-x2;
		d = MAX<float>(x1,x2);
	}

	d = m_power * d;
	m_radius = Clamp<float>(0.0f, 1000.0f, d);
	
	LOG.Write(ssprintf("light radius %f", m_radius));
}

void Light::BuildBoundingBox()
{
	SAFE_DELETE( m_boundingVolume );
	m_boundingVolume = new BoundingBox(MercuryVertex(0,0,0), MercuryVertex(m_radius,m_radius,m_radius) );
}

void Light::DifferedRender()
{
	GLCALL( glLoadMatrix( m_worldPosition ) );
	if ( !m_boundingVolume ) return;
	
	if ( m_shader.IsValid() )
		m_shader->Render( this );
	
	BoundingBox* bb = (BoundingBox*)m_boundingVolume;
//	bb->Render();
	
	MercuryVertex p(0,0,0,1);
	p = m_worldPosition2 * p;
	
	ShaderAttribute sa;
	sa.type = ShaderAttribute::TYPE_MATRIX;
	sa.value.matrix = m_worldPosition2.Ptr();
	Shader::SetAttribute("HG_ModelMatrix", sa);

	sa.type = ShaderAttribute::TYPE_FLOATV4;
	sa.value.fFloatV4[0] = p.GetX();
	sa.value.fFloatV4[1] = p.GetY();
	sa.value.fFloatV4[2] = p.GetZ();
	sa.value.fFloatV4[3] = 1.0f;
	Shader::SetAttribute("HG_LightPos", sa);
	
	sa.value.fFloatV4[0] = m_atten[0];
	sa.value.fFloatV4[1] = m_atten[1];
	sa.value.fFloatV4[2] = m_atten[2];
	sa.value.fFloatV4[3] = m_radius;
	Shader::SetAttribute("HG_LightAtten", sa);
	
	sa.value.fFloatV4[0] = m_color[0];
	sa.value.fFloatV4[1] = m_color[1];
	sa.value.fFloatV4[2] = m_color[2];
	sa.value.fFloatV4[3] = m_power;
	Shader::SetAttribute("HG_LightColor", sa);

	if (m_fullscreen)
	{
		GLCALL( glCullFace(GL_BACK) );
		m_fullScreenQuad.Render( this );
		GLCALL( glCullFace(GL_FRONT) );
	}
	else
		bb->RenderFaces();
	
	if ( m_shader.IsValid() )
		m_shader->PostRender( this);
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
