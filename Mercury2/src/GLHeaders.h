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

#endif