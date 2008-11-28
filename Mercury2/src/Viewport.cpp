#include <Viewport.h>
#include <GL/gl.h>
//#include <GL/glut.h>

void Viewport::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( m_frustum.Ptr() );
	glMatrixMode(GL_MODELVIEW);
}

void Viewport::Perspective( float fov, float aspect, float znear, float zfar )
{
	float xmin, xmax, ymin, ymax;

	ymax = znear * TAN(fov * Q_PI / 360.0);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;

	ComputeFrustum(xmin, xmax, ymin, ymax, znear, zfar);
}

void Viewport::ComputeFrustum(float left, float right, float bottom, float top, float zNear, float zFar)
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
	
	m_frustum.Transpose();  //XXX fix it to remove this
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
