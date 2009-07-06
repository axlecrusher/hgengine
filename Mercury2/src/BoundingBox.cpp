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
	
	*this = bb;
}

bool BoundingBox::Clip( const MercuryPlane& p )
{
	//do a quick spherical test using the signed distance
	float d = p.GetNormal().DotProduct( m_center - p.GetCenter() );
	if (d < -m_extend.Length()) return true;
	return false;
	
	///XXX everything below is broken
	
	MercuryVector dp; //plain normal in box space
//	dp = p.GetNormal().DotProduct3(m_normals[0],m_normals[2],m_normals[3]);
	dp[0] = m_normals[0].DotProduct( p.GetNormal() );
	dp[1] = m_normals[1].DotProduct( p.GetNormal() );
	dp[2] = m_normals[2].DotProduct( p.GetNormal() );
	dp.NormalizeSelf();
//	dp = p.GetNormal();
	
	MercuryVertex P; //max
	if (dp[0] >= 0) P[0] = m_center.GetX() + m_extend.GetX();
	if (dp[1] >= 0) P[1] = m_center.GetY() + m_extend.GetY();
	if (dp[2] >= 0) P[2] = m_center.GetZ() + m_extend.GetZ();

	MercuryVertex N; //min
	if (dp[0] >= 0) N[0] = m_center.GetX() - m_extend.GetX();
	if (dp[1] >= 0) N[1] = m_center.GetY() - m_extend.GetY();
	if (dp[2] >= 0) N[2] = m_center.GetZ() - m_extend.GetZ();
	
	float x = dp.DotProduct( P );
	float y = dp.DotProduct( N );
//	printf("p %f     n %f\n", x, y);
	if ( x < 0)
		return true; //max value outside
	if ( y < 0) //is negative value outside
		return false; //intersect

	return false;
/*
	db * m_extend;
	float x = ABS( m_extend.GetX() * dp[0] );
	x += ABS( m_extend.GetY() * dp[1] );
	x += ABS( m_extend.GetZ() * dp[2] );
	
	float d = p.GetNormal().DotProduct( m_center+p.GetCenter() ); //signed distance
	if ( ABS(d) <= x ) //intersection
		return false;

	return BEHIND_PLANE(d); //if we don't intersect, just see what side we are on
	*/
}

bool BoundingBox::Clip( const Frustum& f )
{
	bool clipped = false;
	for (uint8_t i = 0; (i < 6) && !clipped; ++i)
	{
		bool t = Clip( f.GetPlane(i) );
//		printf("p%d %d\n", i, t);
		clipped = t;
	}
//	printf("******\n");
//	return false;
	
	return clipped;
}

//bool BoundingBox::FrustumCull() const
//{
	/*feedback in openGL is probably depreciated
	and is known to fallback to software
	the OcclusionTest provides the same performence
	it is probably best to avoid using this function */
/*	
	static float b[3];
	uint32_t samples;
	const float* center = GetCenter();
	const float* extend = GetExtend();
		
	glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT );
	glDisable(GL_CULL_FACE);
	
	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);
	glScalef(extend[0],extend[1],extend[2]);

	uint8_t tCount = Texture::NumberActiveTextures();
	for (uint8_t i = 0; i < tCount; ++i)
	{
		glActiveTexture( GL_TEXTURE0+i );
		glClientActiveTextureARB( GL_TEXTURE0+i );
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable( GL_TEXTURE_2D );
	}
	glFeedbackBuffer(3, GL_3D, b);
	glRenderMode( GL_FEEDBACK );
	
	InitVBO();
	
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vboID);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glDrawArrays(GL_QUADS, 0, 24);

	for (uint8_t i = 0; i < tCount; ++i)
	{
		glActiveTexture( GL_TEXTURE0+i );
		glClientActiveTextureARB(GL_TEXTURE0+i);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnable( GL_TEXTURE_2D );
	}
	
	samples = glRenderMode( GL_RENDER );
	glPopMatrix();
	glPopAttrib( );
	
//	return false;
	return samples==0;
}
*/

bool BoundingBox::DoFrustumTest( const MercuryMatrix& m )
{
	BoundingBox bb(*this);
//	bb.Render();
	bb.Transform( m );
//	bb.Render();
	return bb.Clip( *FRUSTUM );
}

void BoundingBox::DoOcclusionTest( OcclusionResult& result )
{
	return;
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
