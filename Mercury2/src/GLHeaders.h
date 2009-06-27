#ifndef GLHEADERS_H
#define GLHEADERS_H

#ifdef WIN32
#include <windows.h>
#else
#define GL_GLEXT_PROTOTYPES
#endif

#include <GL/gl.h>

#ifdef WIN32
#include <glext.h>
#include <freeglut/glut.h>
#include <OGLExtensions.h>
#else
#include <GL/glext.h>
#include <GL/freeglut.h>
#endif

#include <GLHelpers.h>

#define GLERRORCHECK { \
uint32_t e = glGetError(); \
if ( e != GL_NO_ERROR ) { \
printf("GL Error:%s\n", GlError2String(e).c_str()); \
assert(0); } }

#define CHECKFBO { \
uint32_t e = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER); \
if ( e != GL_FRAMEBUFFER_COMPLETE ) { \
printf("GL FBO Error:%s\n", GlError2String(e).c_str()); \
assert(0); } }


#endif