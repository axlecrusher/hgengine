#include <GLHeaders.h>
#include <BoundingBox.h>
#include <string.h>
#include <Viewport.h>

#define SIGNED_DIST(x) m_normal.DotProduct(x)

// origional algorithim was -x<0
#define BEHIND_PLANE(x) x>=0

BoundingBox::BoundingBox(const MercuryVertex& center, const MercuryVertex& extend)
	:m_center(center), m_extend(extend)
{
	ComputeNormals();
};

BoundingBox::BoundingBox(const BoundingBox& bb)
	:m_center(bb.m_center), m_extend(bb.m_extend)
{
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
	//normals are probably just the cardinal axises
/*	MercuryVertex t(m_center, 0);
	t.SetX( t.GetX() + m_extend.GetX() );
	m_normals[0] = t;//.Normalize();
	
	t = m_center;
	t.SetY( t.GetY() + m_extend.GetY() );
	m_normals[1] = t;//.Normalize();

	t = m_center;
	t.SetZ( t.GetZ() + m_extend.GetZ() );
	m_normals[2] = t;//.Normalize();
	*/
}

void BoundingBox::Transform( const MercuryMatrix& m )
{
	BoundingBox bb;
	bb.m_extend = m_extend;
	MercuryMatrix mm = m * FRUSTUM->GetMatrix();
	bb.m_center = mm * MercuryVertex(m_center, 1);
	bb.m_normals[0] = (mm * MercuryVector(1.0f,0,0,0)).Normalize();
	bb.m_normals[1] = (mm * MercuryVector(0,1.0f,0,0)).Normalize();
	bb.m_normals[2] = (mm * MercuryVector(0,0,1.0f,0)).Normalize();
//	bb.Render();
	*this = bb;
}

bool BoundingBox::Clip( const MercuryPlane& p )
{
	MercuryVertex dp = p.GetNormal().DotProduct3(m_normals[0], m_normals[1], m_normals[2]);
	float x = ABS( m_extend.GetX() * dp[0] );
	x += ABS( m_extend.GetY() * dp[1] );
	x += ABS( m_extend.GetZ() * dp[2] );
	
	float d = p.GetNormal().DotProduct( m_center+p.GetCenter() ); //signed distance
	if ( ABS(d) <= x ) //intersection
		return false;

	return BEHIND_PLANE(d); //if we don't intersect, just see what side we are on
}

bool BoundingBox::Clip( const Frustum& f )
{
	bool inView = true;
	for (uint8_t i = 0; (i < 6) && inView; ++i)
	{
		inView = Clip( f.GetPlane(i) )?false:inView;
	}
	
	return !inView;
}

bool BoundingBox::FrustumCull() const
{
	static float b[3];
	uint32_t samples;
	const float* center = GetCenter();
	const float* extend = GetExtend();
	
	glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT );
	glDisable(GL_CULL_FACE);
	glFeedbackBuffer(3, GL_3D, b);
	glRenderMode( GL_FEEDBACK );
		
	glBegin(GL_QUADS);

	//front
	glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]+extend[2]);
	glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]+extend[2]);
	glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]+extend[2]);
	glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]+extend[2]);

	//back
	glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]-extend[2]);
	glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]-extend[2]);

	//top
	glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]+extend[2]);
	glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]+extend[2]);

	//bottom
	glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]+extend[2]);
	glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]+extend[2]);
	
	//left
	glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]-extend[2]);
	glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]-extend[2]);
	glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]+extend[2]);
	glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]+extend[2]);
	
	//right
	glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]+extend[2]);
	glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]+extend[2]);
	
	glEnd();
	samples = glRenderMode( GL_RENDER );
	glPopAttrib( );
	return samples==0;
}

bool BoundingBox::OcclusionCull() const
{
	static uint32_t q;
	uint32_t samples;
	const float* center = GetCenter();
	const float* extend = GetExtend();
		
	glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT );
	glDisable(GL_CULL_FACE);
	glGenQueriesARB(1, &q);
	glBeginQueryARB(GL_SAMPLES_PASSED_ARB, q);
	
	glBegin(GL_QUADS);

	//front
	glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]+extend[2]);
	glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]+extend[2]);
	glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]+extend[2]);
	glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]+extend[2]);

	//back
	glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]-extend[2]);
	glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]-extend[2]);

	//top
	glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]+extend[2]);
	glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]+extend[2]);

	//bottom
	glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]+extend[2]);
	glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]+extend[2]);
	
	//left
	glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]-extend[2]);
	glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]-extend[2]);
	glVertex3f(center[0]-extend[0], center[1]-extend[1], center[2]+extend[2]);
	glVertex3f(center[0]-extend[0], center[1]+extend[1], center[2]+extend[2]);
	
	//right
	glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]-extend[2]);
	glVertex3f(center[0]+extend[0], center[1]-extend[1], center[2]+extend[2]);
	glVertex3f(center[0]+extend[0], center[1]+extend[1], center[2]+extend[2]);
	
	glEnd();
	glEndQueryARB(GL_SAMPLES_PASSED_ARB);
	glGetQueryObjectuivARB(q, GL_QUERY_RESULT_ARB, &samples);

	glPopAttrib( );
	
	return samples==0;
}

void BoundingBox::Render()
{
//	BoundingBox gbb = *this;

//	gbb.Transform( ((RenderableNode*)node)->GetGlobalMatrix() );
//	if ( FRUSTUM->Clip( gbb ) ) return;
	
	const float* center = GetCenter();
	const float* extend = GetExtend();
	
//	glPushMatrix();
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
	
	//center
	glPointSize(4);
	glBegin(GL_POINTS);
	glVertex3f(center[0], center[1], center[2]);
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
//	glPopMatrix();
}

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
