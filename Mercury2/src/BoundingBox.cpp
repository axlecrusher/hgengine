#include <GLHeaders.h>
#include <BoundingBox.h>
#include <string.h>
#include <Viewport.h>
#include <Texture.h>

#include <MercuryVBO.h>

#define SIGNED_DIST(x) m_normal.DotProduct(x)

// origional algorithim was -x<0
#define BEHIND_PLANE(x) x>=0

OcclusionResult::~OcclusionResult()
{
	if ( m_occlusionQuery != 0 )
		glDeleteQueriesARB( 1, &m_occlusionQuery );
	m_occlusionQuery = 0;
}

uint32_t OcclusionResult::GetSamples() const
{
	if (m_occlusionQuery == 0) return ~0;
	
	uint32_t samples;
	glGetQueryObjectuivARB(m_occlusionQuery, GL_QUERY_RESULT_ARB, &samples);
	return samples;
}

BoundingBox::BoundingBox(const MercuryVertex& center, const MercuryVertex& extend)
	:m_center(center), m_extend(extend)
{
	PopulateVertices();
	ComputeNormals();
}

BoundingBox::BoundingBox(const BoundingBox& bb)
	:m_center(bb.m_center), m_extend(bb.m_extend)
{
	PopulateVertices();
	for (uint8_t i = 0; i < 3; ++i)
		m_normals[i] = bb.m_normals[i];
}

BoundingVolume* BoundingBox::SpawnClone() const
{
	return new BoundingBox(*this);
}

void BoundingBox::LoadFromBinary(char* data)
{
	memcpy(m_center, data, sizeof(float)*3);
	memcpy(m_extend, data+(sizeof(float)*3), sizeof(float)*3);
	ComputeNormals();
}

void BoundingBox::ComputeNormals()
{
	//these need to be the normals of each face
	/*
	MercuryVertex t(m_center, 0);
	t.SetX( t.GetX() + m_extend.GetX() );
	m_normals[0] = (m_center - t).Normalize();
	
	t = m_center;
	t.SetY( t.GetY() + m_extend.GetY() );
	m_normals[1] = (m_center - t).Normalize();

	t = m_center;
	t.SetZ( t.GetZ() + m_extend.GetZ() );
	m_normals[2] = (m_center - t).Normalize();
	m_normals[0].Print();
	*/
}

void BoundingBox::Transform( const MercuryMatrix& m )
{
	//the frustum planes are defined in world space so
	//these values need to be transformed into world space
	BoundingBox bb;
	bb.m_extend = m * m_extend; //Rotate and scale
	bb.m_center = m * MercuryVertex(m_center, 1);
	
	//transform the box axises into world axises
	bb.m_normals[0] = (m * MercuryVector(1,0,0)).Normalize();
	bb.m_normals[1] = (m * MercuryVector(0,1,0)).Normalize();
	bb.m_normals[2] = (m * MercuryVector(0,0,1)).Normalize();
	
	//compute box vertices into world coordinates
	//it would be nice to do this without needing to save
	//the result in the bounding box.
	//XXX look into ways to optimize this
	MercuryMatrix mm = m;
	mm.Translate( m_center );
	mm.Scale( m_extend );
	
	bb.v[0] = mm * MercuryVertex(-1, 1, 1, 1);
	bb.v[1] = mm * MercuryVertex(-1, -1, 1, 1);
	bb.v[2] = mm * MercuryVertex(1, -1, 1, 1);
	bb.v[3] = mm * MercuryVertex(1, 1, 1, 1);
		
	bb.v[4] = mm * MercuryVertex(-1, 1, -1, 1);
	bb.v[5] = mm * MercuryVertex(-1, -1, -1, 1);
	bb.v[6] = mm * MercuryVertex(1, -1, -1, 1);
	bb.v[7] = mm * MercuryVertex(1, 1, -1, 1);
	
	*this = bb;
}

bool BoundingBox::Clip( const MercuryPlane& p )
{
	//do a quick spherical test using the signed distance
	//from the center of the box
	float d = p.GetNormal().DotProduct( m_center - p.GetCenter() );
	if (d < -m_extend.Length()) return true;  //sphere is further than radius distance behind plane
	
	//all points must be behind a plane to be considered clipped
	bool clip = true;
	for (uint8_t i = 0; (i < 8) && clip; ++i)
		clip = p.IsBehindPlane( v[i] );
	
	return clip;
}

bool BoundingBox::Clip( const Frustum& f )
{
	bool clipped = false;
	for (uint8_t i = 0; (i < 6) && !clipped; ++i)
		clipped = Clip( f.GetPlane(i) );
	
	return clipped;
}

bool BoundingBox::DoFrustumTest( const MercuryMatrix& m )
{
	BoundingBox bb(*this);
//	bb.Render();
	bb.Transform( m );
	return bb.Clip( *FRUSTUM );
}

void BoundingBox::DoOcclusionTest( OcclusionResult& result )
{
	const float* center = GetCenter();
	const float* extend = GetExtend();
	
	glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	
	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);
	glScalef(extend[0],extend[1],extend[2]);
	
	if (m_vboID == 0) InitVBO();
	
	if ( MercuryVBO::m_lastVBOrendered != &m_vboID )
	{
		MercuryVBO::m_lastVBOrendered = &m_vboID;
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vboID); // once
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);  // once
		glVertexPointer(3, GL_FLOAT, 0, 0);  // once
	}
	
	if (result.GetQueryID() == 0) glGenQueriesARB(1, &result.GetQueryID());
	glBeginQueryARB(GL_SAMPLES_PASSED_ARB, result.GetQueryID());

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	glDrawArrays(GL_QUADS, 0, 24);
	
	glEndQueryARB(GL_SAMPLES_PASSED_ARB);
//	glGetQueryObjectuivARB(q, GL_QUERY_RESULT_ARB, &samples);

	glPopAttrib( );
	glPopMatrix();
}

void BoundingBox::Render()
{
//	BoundingBox gbb = *this;

//	gbb.Transform( ((RenderableNode*)node)->GetGlobalMatrix() );
//	if ( FRUSTUM->Clip( gbb ) ) return;
	
	const float* center = GetCenter();
	const float* extend = GetExtend();
	
	glPushMatrix();
//	glLoadIdentity();
	glPushAttrib( GL_CURRENT_BIT  );
	glBegin(GL_LINES);
	glColor3f(0,1.0f,0);

	//front
	glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]+extend[2]);
	glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]+extend[2]);
	glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]+extend[2]);
	glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]+extend[2]);
	glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]+extend[2]);
	glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]+extend[2]);
	glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]+extend[2]);
	glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]+extend[2]);

	//back
	glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]-extend[2]);
	glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]-extend[2]);
	glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]-extend[2]);
	glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]-extend[2]);

	//top
	glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]-extend[2]);
	glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]+extend[2]);
	glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]+extend[2]);

	//bottom
	glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]-extend[2]);
	glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]+extend[2]);
	glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]+extend[2]);

	glEnd();
	
	glPointSize(4);
	glBegin(GL_POINTS);
	//center
	glVertex3f(center[0], center[1], center[2]);
	//max point
	glColor3f(1,1,0);
	glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]+extend[2]);
	//min point
//	glColor3f(1,0,0);
	glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]-extend[2]);
	glEnd();

	
	//normals
	MercuryVertex c;
	glBegin(GL_LINES);
	
	glColor3f(1.0f,0,0);
	glVertex3f(center[0], center[1], center[2]);
	c = center;
	c += m_normals[0];
	glVertex3f(c.GetX(), c.GetY(), c.GetZ());
	
	glColor3f(0,1.0f,0);
	glVertex3f(center[0], center[1], center[2]);
	c = center;
	c += m_normals[1];
	glVertex3f(c.GetX(), c.GetY(), c.GetZ());
	
	glColor3f(0,0,1.0f);
	glVertex3f(center[0], center[1], center[2]);
	c = center;
	c += m_normals[2];
	glVertex3f(c.GetX(), c.GetY(), c.GetZ());
	
	glEnd();

	glPopAttrib( );
	glPopMatrix();
}

void BoundingBox::PopulateVertices()
{
	if (m_vertexData.Length() == 72) return;
	m_vertexData.Allocate( 72 );
	
	uint32_t i = 0;
	
	//back
	m_vertexData[i++] = -1.0f; m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f;
	m_vertexData[i++] = -1.0f; m_vertexData[i++] = -1.0f; m_vertexData[i++] = -1.0f;
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f; m_vertexData[i++] = -1.0f;
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f;

	//front
	m_vertexData[i++] = -1.0f; m_vertexData[i++] = 1.0f; m_vertexData[i++] = 1.0f;
	m_vertexData[i++] = -1.0f; m_vertexData[i++] = -1.0f; m_vertexData[i++] = 1.0f;
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f; m_vertexData[i++] = 1.0f;
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = 1.0f; m_vertexData[i++] = 1.0f;

	//left
	m_vertexData[i++] = -1.0f; m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f;
	m_vertexData[i++] = -1.0f; m_vertexData[i++] = -1.0f; m_vertexData[i++] = -1.0f;
	m_vertexData[i++] = -1.0f; m_vertexData[i++] = -1.0f; m_vertexData[i++] = 1.0f;
	m_vertexData[i++] = -1.0f; m_vertexData[i++] = 1.0f; m_vertexData[i++] = 1.0f;

	//right
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f;
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f; m_vertexData[i++] = -1.0f;
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f; m_vertexData[i++] = 1.0f;
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = 1.0f; m_vertexData[i++] = 1.0f;

	//top
	m_vertexData[i++] = -1.0f; m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f;
	m_vertexData[i++] = -1.0f; m_vertexData[i++] = 1.0f; m_vertexData[i++] = 1.0f;
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = 1.0f; m_vertexData[i++] = 1.0f;
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f;

	//bottom
	m_vertexData[i++] = -1.0f; m_vertexData[i++] = -1.0f; m_vertexData[i++] = -1.0f;
	m_vertexData[i++] = -1.0f; m_vertexData[i++] = -1.0f; m_vertexData[i++] = 1.0f;
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f; m_vertexData[i++] = 1.0f;
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f; m_vertexData[i++] = -1.0f;
}

void BoundingBox::InitVBO()
{
	glGenBuffersARB(1, &m_vboID);
	
	//vertex VBO
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vboID);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_vertexData.LengthInBytes(), m_vertexData.Buffer(), GL_STATIC_DRAW_ARB);
}

AlignedBuffer<float> BoundingBox::m_vertexData;
uint32_t BoundingBox::m_vboID = 0;

/****************************************************************************
 *   Copyright (C) 2008 by Joshua Allen                                     *
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
