#include <Viewport.h>
#include <GL/gl.h>

REGISTER_NODE_TYPE(Viewport);

const Frustum* FRUSTUM = NULL;

void Viewport::Render()
{
	FRUSTUM = &m_frustum;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
		
	MercuryMatrix m = FindGlobalMatrix();
	m.Transpose();
	
	MercuryMatrix f = m_frustum.GetMatrix();
	f.Transpose();

	glLoadMatrixf( (m * f).Ptr() );
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

void Frustum::SetPerspective( float fov, float aspect, float znear, float zfar )
{
	float xmin, xmax, ymin, ymax;
	
	m_fov = fov;
	m_aspect = aspect;
	m_zNear = znear;
	m_zFar = zfar;

	float tang = TAN(m_fov * Q_PI / 360.0);

	m_nh = ymax = m_zNear * tang; //nh
	ymin = -ymax;
	xmin = ymin * m_aspect;
	m_nw = xmax = ymax * m_aspect; //nw
	
	m_fh = m_zFar*tang;
	m_fw = m_fh*aspect;

	ComputeFrustum(xmin, xmax, ymin, ymax, m_zNear, m_zFar);
}

void Frustum::ComputeFrustum(float left, float right, float bottom, float top, float zNear, float zFar)
{
	float near2 = 2*zNear;
	float rml = right-left;
	float tmb = top - bottom;
	float fmn = zFar - zNear;

	float A = (right+left)/rml;
	float B = (top+bottom)/tmb;
	float C = -(zFar+zNear)/fmn;
	float D = -(near2*zFar)/fmn;

	m_frustum.Zero();

	//row major
	m_frustum[0][0] = near2/rml;
	m_frustum[0][2] = A;
	m_frustum[1][1] = near2/tmb;
	m_frustum[1][2] = B;
	m_frustum[2][2] = C;
	m_frustum[2][3] = D;
	m_frustum[3][2] = -1;
	
//	m_frustum.Transpose();  //XXX fix it to remove this
}

void Frustum::LookAt(const MercuryVertex& eye, const MercuryVector& look, const MercuryVector& up)
{
	//Right now this only builds the frustum planes
	MercuryVector X,Y,Z;
	
	Z = (eye - look).Normalize(); //direction behind camera
	X = (up.CrossProduct(Z)).Normalize(); //X axis
	Y = Z.CrossProduct( X ); //real up
	
	m_nc = (eye - Z) * m_zNear;
	m_fc = (eye - Z) * m_zFar;
	
	m_planes[PNEAR].Setup(m_nc, Z*(-1));
	m_planes[PFAR].Setup(m_fc, Z);
//	m_fc.Print();
//	Z.Print();
	MercuryVector aux,normal;

	aux = (m_nc + Y*m_nh) - eye;
	aux.NormalizeSelf();
	normal = aux * X;
	m_planes[PTOP].Setup(m_nc+Y*m_nh,normal);

	aux = (m_nc - Y*m_nh) - eye;
	aux.NormalizeSelf();
	normal = X * aux;
	m_planes[PBOTTOM].Setup(m_nc-Y*m_nh,normal);
	
	aux = (m_nc - X*m_nw) - eye;
	aux.NormalizeSelf();
	normal = aux * Y;
	m_planes[PLEFT].Setup(m_nc-X*m_nw,normal);

	aux = (m_nc + X*m_nw) - eye;
	aux.NormalizeSelf();
	normal = Y * aux;
	m_planes[PRIGHT].Setup(m_nc+X*m_nw,normal);
}

bool Frustum::Clip(const BoundingBox& bb) const
{
	bool inView = true;
	for (uint8_t i = 0; (i < 6) && inView; ++i)
	{
		inView = m_planes[i].IsBehindPlane( bb )?false:inView;
	}
	
	return !inView;
};

/*
void Frustum::LookAt()
{
	
}
*/
/*
bool Frustum::IsPointInFrustum( float x, float y, float z )
{
	for( uint16_t i = 0; i < 6; ++i )
		if( frustum[i][0] * x + frustum[i][1] * y + frustum[i][2] * z + frustum[i][3] <= 0 )
			return false;
	return true;
}*/

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
