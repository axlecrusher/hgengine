#ifndef GLHEADERS_H
#define GLHEADERS_H

#ifdef WIN32
#include <windows.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <configuration.h>
#endif

#include <GL/gl.h>

#ifdef WIN32
#include <glext.h>
#include <freeglut/glut.h>
#include <OGLExtensions.h>
#else
#include <GL/glext.h>
#include <GL/glu.h>
#endif

#include <GLHelpers.h>
#include <assert.h>

#include <MercuryLog.h>

#define GLERRORCHECK { \
uint32_t e = GLCALL( glGetError() ); \
if ( e != GL_NO_ERROR ) { \
LOG.Write(ssprintf("GL Error:%s", GlError2String(e).c_str())); \
printf("GL Error:%s", GlError2String(e).c_str()); \
assert(0); } }

#define CHECKFBO { \
uint32_t e = GLCALL( glCheckFramebufferStatusEXT(GL_FRAMEBUFFER) ); \
if ( e != GL_FRAMEBUFFER_COMPLETE ) { \
LOG.Write(ssprintf("GL FBO Error:%s", GlError2String(e).c_str())); \
assert(0); } }

#include <MercuryHash.h>
extern uint32_t GLCALLCOUNT;
extern MHash< uint32_t> GLFUNCTCOUNT;

#endif
