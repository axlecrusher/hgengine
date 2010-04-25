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
	{
		GLCALL( glDeleteQueriesARB( 1, &m_occlusionQuery ) );
	}
	m_occlusionQuery = 0;
}

uint32_t OcclusionResult::GetSamples() const
{
	if (m_occlusionQuery == 0) return ~(uint32_t(0));
	
	uint32_t samples;
	GLCALL( glGetQueryObjectuivARB(m_occlusionQuery, GL_QUERY_RESULT_ARB, &samples) );
	return samples;
}

BoundingBox::BoundingBox(const MercuryVertex& center, const MercuryVertex& extend)
	:BoundingVolume(), m_center(center), m_extend(extend)
{
	PopulateVertices();
	ComputeNormals();
}

BoundingBox::BoundingBox(const BoundingBox& bb)
	:BoundingVolume(), m_center(bb.m_center), m_extend(bb.m_extend)
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

float BoundingBox::ComputeRadius() const
{
/*	MercuryVertex p(m_center+m_extend);
	float r = (m_center-p).Length();
	if (abs(m_extend.Length() - r) > .000001) printf("wrong %f %f\n",m_extend.Length(), r);
	return (m_center-p).Length();
	*/
	return m_extend.Length(); //this is actually correct, verified above
}

bool BoundingBox::Clip( const MercuryPlane& p )
{
	//do a quick spherical test using the signed distance
	//from the center of the box
	float d = p.GetNormal().DotProduct( m_center - p.GetCenter() );
	if (d < -ComputeRadius()) return true;  //sphere is further than radius distance behind plane
	
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

bool BoundingBox::Intersect( const BoundingBox& b )
{
	MercuryVector d(m_center - b.m_center);
	if ( d.Length() > (ComputeRadius()+b.ComputeRadius()) ) return false; //quick circle check
	
	MercuryVector l(m_extend+b.GetExtend());
	return l.GetX()<=ABS(d.GetX()) && l.GetY()<=ABS(d.GetY()) && l.GetZ()<=ABS(d.GetZ());
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
	
	GLCALL( glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT) );
	GLCALL( glDisable(GL_CULL_FACE) );
	
	GLCALL( glPushMatrix() );
	GLCALL( glTranslatef(center[0], center[1], center[2]) );
	GLCALL( glScalef(extend[0],extend[1],extend[2]) );
	
	if (m_vboID == 0) InitVBO();
	
	if ( MercuryVBO::GetLastRendered() != &m_vboID )
	{
		MercuryVBO::SetLastRendered( &m_vboID );
		GLCALL( glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vboID) ); // once
		GLCALL( glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );  // once
		GLCALL( glVertexPointer(3, GL_FLOAT, 0, 0) );  // once
	}
	
	if (result.GetQueryID() == 0) { GLCALL( glGenQueriesARB(1, &result.GetQueryID()) ); }
	GLCALL( glBeginQueryARB(GL_SAMPLES_PASSED_ARB, result.GetQueryID()) );

	GLCALL( glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE) );
	GLCALL( glDepthMask(GL_FALSE) );

	GLCALL( glDrawArrays(GL_QUADS, 0, 24) );
	MercuryVBO::IncrementBatches();
	GLCALL( glEndQueryARB(GL_SAMPLES_PASSED_ARB) );
//	GLCALL( glGetQueryObjectuivARB(q, GL_QUERY_RESULT_ARB, &samples) );

	GLCALL( glPopMatrix() );
	GLCALL( glPopAttrib( ) );
}

void BoundingBox::RenderFaces() const
{
	const float* center = GetCenter();
	const float* extend = GetExtend();
	
	GLCALL( glPushMatrix() );
	GLCALL( glTranslatef(center[0], center[1], center[2]) );
	GLCALL( glScalef(extend[0],extend[1],extend[2]) );
	
	if (m_vboID == 0) InitVBO();
	
//	if ( MercuryVBO::GetLastRendered() != &m_vboID )
	{
		MercuryVBO::SetLastRendered( &m_vboID );
		GLCALL( glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vboID) ); // once
		GLCALL( glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );  // once
		GLCALL( glVertexPointer(3, GL_FLOAT, 0, 0) );  // once
	}
	
	GLCALL( glDrawArrays(GL_QUADS, 0, 24) );
	MercuryVBO::IncrementBatches();
	GLCALL( glPopMatrix() );
}

void BoundingBox::Render()
{
//	BoundingBox gbb = *this;

//	gbb.Transform( ((RenderableNode*)node)->GetGlobalMatrix() );
//	if ( FRUSTUM->Clip( gbb ) ) return;
	
	const float* center = GetCenter();
	const float* extend = GetExtend();
	
	GLCALL( glPushMatrix() );
//	GLCALL( glLoadIdentity() );
	GLCALL( glPushAttrib( GL_CURRENT_BIT  ) );
	GLCALL( glBegin(GL_LINES) );
	GLCALL( glColor3f(0,1.0f,0) );

	//front
	GLCALL( glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]+extend[2]) );
	GLCALL( glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]+extend[2]) );
	GLCALL( glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]+extend[2]) );
	GLCALL( glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]+extend[2]) );
	GLCALL( glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]+extend[2]) );
	GLCALL( glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]+extend[2]) );
	GLCALL( glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]+extend[2]) );
	GLCALL( glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]+extend[2]) );

	//back
	GLCALL( glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]-extend[2]) );
	GLCALL( glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]-extend[2]) );
	GLCALL( glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]-extend[2]) );
	GLCALL( glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]-extend[2]) );
	GLCALL( glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]-extend[2]) );
	GLCALL( glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]-extend[2]) );
	GLCALL( glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]-extend[2]) );
	GLCALL( glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]-extend[2]) );

	//top
	GLCALL( glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]-extend[2]) );
	GLCALL( glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]+extend[2]) );
	GLCALL( glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]-extend[2]) );
	GLCALL( glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]+extend[2]) );

	//bottom
	GLCALL( glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]-extend[2]) );
	GLCALL( glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]+extend[2]) );
	GLCALL( glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]-extend[2]) );
	GLCALL( glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]+extend[2]) );

	GLCALL( glEnd() );
	
	GLCALL( glPointSize(4) );
	GLCALL( glBegin(GL_POINTS) );
	//center
	GLCALL( glVertex3f(center[0], center[1], center[2]) );
	//max point
	GLCALL( glColor3f(1,1,0) );
	GLCALL( glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]+extend[2]) );
	//min point
//	GLCALL( glColor3f(1,0,0) );
	GLCALL( glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]-extend[2]) );
	GLCALL( glEnd() );

	
	//normals
	MercuryVertex c;
	GLCALL( glBegin(GL_LINES) );
	
	GLCALL( glColor3f(1.0f,0,0) );
	GLCALL( glVertex3f(center[0], center[1], center[2]) );
	c = center;
	c += m_normals[0];
	GLCALL( glVertex3f(c.GetX(), c.GetY(), c.GetZ()) );
	
	GLCALL( glColor3f(0,1.0f,0) );
	GLCALL( glVertex3f(center[0], center[1], center[2]) );
	c = center;
	c += m_normals[1];
	GLCALL( glVertex3f(c.GetX(), c.GetY(), c.GetZ()) );
	
	GLCALL( glColor3f(0,0,1.0f) );
	GLCALL( glVertex3f(center[0], center[1], center[2]) );
	c = center;
	c += m_normals[2];
	GLCALL( glVertex3f(c.GetX(), c.GetY(), c.GetZ()) );
	
	GLCALL( glEnd() );

	GLCALL( glPopAttrib( ) );
	GLCALL( glPopMatrix() );
}

void BoundingBox::PopulateVertices()
{
	if (m_vertexData.Length() == 72) return;
	m_vertexData.Allocate( 72 );
	
	uint32_t i = 0;
	
	//back
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f;
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f; m_vertexData[i++] = -1.0f;
	m_vertexData[i++] = -1.0f; m_vertexData[i++] = -1.0f; m_vertexData[i++] = -1.0f;
	m_vertexData[i++] = -1.0f; m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f;

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
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = 1.0f; m_vertexData[i++] = 1.0f;
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f; m_vertexData[i++] = 1.0f;
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f; m_vertexData[i++] = -1.0f;
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f;

	//top
	m_vertexData[i++] = -1.0f; m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f;
	m_vertexData[i++] = -1.0f; m_vertexData[i++] = 1.0f; m_vertexData[i++] = 1.0f;
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = 1.0f; m_vertexData[i++] = 1.0f;
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f;

	//bottom
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f; m_vertexData[i++] = -1.0f;
	m_vertexData[i++] = 1.0f; m_vertexData[i++] = -1.0f; m_vertexData[i++] = 1.0f;
	m_vertexData[i++] = -1.0f; m_vertexData[i++] = -1.0f; m_vertexData[i++] = 1.0f;
	m_vertexData[i++] = -1.0f; m_vertexData[i++] = -1.0f; m_vertexData[i++] = -1.0f;
}

void BoundingBox::InitVBO()
{
	GLCALL( glGenBuffersARB(1, &m_vboID) );
	
	//vertex VBO
	GLCALL( glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vboID) );
	GLCALL( glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_vertexData.LengthInBytes(), m_vertexData.Buffer(), GL_STATIC_DRAW_ARB) );
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
