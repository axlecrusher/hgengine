#include <GLHeaders.h>
#include <GLHelpers.h>
#include <MercuryHash.h>


MString GlError2String(uint32_t e)
{
	switch (e)
	{
		case GL_INVALID_ENUM:
			return "Invalid Enum";
		case GL_INVALID_VALUE:
			return "Invalid Value";
		case GL_INVALID_OPERATION:
			return "Invalid Operation";
		case GL_STACK_OVERFLOW:
			return "Stack Overflow";
		case GL_STACK_UNDERFLOW:
			return "Stack Underflow";
		case GL_OUT_OF_MEMORY:
			return "Out of Memory";
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "Invalid FBO Operation";
		case GL_FRAMEBUFFER_UNSUPPORTED:
			return "FBO Unsupported";
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			return "FBO Incomplete Attachment";
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			return "FBO Incomplete Missing Attachment";
//		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
//			return "FBO Incomplete Dimensions";
//		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
//			return "FBO Incomplete Formats";
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			return "FBO Incomplete Draw Buffer";
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			return "FBO Incomplete Read Buffer";
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			return "FBO Incomplete Multisample";
	}
	return "Unknown Error";
}

void glLoadMatrix(const MercuryMatrix& m)
{
	static MercuryMatrix l;
	l = m;
	l.Transpose();
	glLoadMatrixf( l.Ptr() );
}

MercuryMatrix glGetMatrix(unsigned int m)
{
	MercuryMatrix mm;
	glGetFloatv(m, mm.Ptr());
	mm.Transpose();
	return mm;
}

MercuryVertex pointFromScreenLoc(int screen_x, int screen_y, float fForceDepth)
{
	GLfloat winX, winY, winZ;
	GLdouble mouseX = 0, mouseY = 0, mouseZ = 0;
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	
	GLCALL( glGetIntegerv(GL_VIEWPORT, viewport) );
	GLCALL( glGetDoublev(GL_MODELVIEW_MATRIX, modelview) );
	GLCALL( glGetDoublev(GL_PROJECTION_MATRIX, projection) );
	
	winX = (float)screen_x;
	winY = (float)viewport[3] - (float)screen_y;
	
	gluUnProject(
		winX, winY, fForceDepth,
		modelview, projection, viewport,
		&mouseX, &mouseY, &mouseZ);
	return MercuryVertex( (float)mouseX, (float)mouseY, (float)mouseZ );
}

MercuryVertex pointFromScreenLoc(int screen_x, int screen_y )
{
	GLint viewport[4];
	GLCALL( glGetIntegerv(GL_VIEWPORT, viewport) );

	float winY = (float)viewport[3] - (float)screen_y;

	float winZ;
	GLCALL( glReadPixels( screen_x, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ ) );
	return pointFromScreenLoc( screen_x, screen_y, winZ );
}

unsigned int ToGLColorType(ColorByteType cbt)
{
	switch (cbt)
	{
		case WHITE:
			return GL_LUMINANCE;
		case WHITE_ALPHA:
			return GL_LUMINANCE_ALPHA;
		case RGB:
			return GL_RGB;
		case RGBA:
			return GL_RGBA;
		case RGB16:
			return GL_RGB16;
		case RGBA16:
			return GL_RGBA16;
		case RGBA16F:
			return GL_RGBA16F_ARB;
		case RGBA32F:
			LOG.Write("GL_RGBA32F_ARB unsupported using GL_RGBA16F_ARB" );
			return GL_RGBA16F_ARB;
		default:
			LOG.Write(ssprintf( "Unsupported color byte type (%d)", cbt ));
			return GL_RGB;
	}

}

#ifdef GL_PROFILE
uint32_t GLCALLCOUNT = 0;
MHash< uint32_t> GLFUNCTCOUNT;

void ProfileGLCall(const MString& funcName)
{
	++GLCALLCOUNT;
	GLFUNCTCOUNT[funcName]++;
}

void PrintGLFunctionCalls()
{
	MVector < MString > v;
	GLFUNCTCOUNT.VectorIndices( v );
	//We now know all the names of the functions that were called.
	for( unsigned i = 0; i < v.size(); i++ )
		LOG.Write(ssprintf( "%d ", GLFUNCTCOUNT[v[i]] ) + v[i] );

	GLFUNCTCOUNT.clear();
}
#else
void PrintGLFunctionCalls()
{
	LOG.Write( "No profiling performed on data set because it was configrued off." );
}
#endif

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
