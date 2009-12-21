#ifndef _OGL_EXTENSIONS_H

#include <glext.h>

extern PFNGLBINDBUFFERARBPROC glBindBufferARB;
extern PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;
extern PFNGLACTIVETEXTUREPROC glActiveTexture;
extern PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTextureARB;
extern PFNGLGENBUFFERSARBPROC glGenBuffersARB;
extern PFNGLBUFFERDATAARBPROC glBufferDataARB;
extern PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements;

//glsl
extern PFNGLCREATEPROGRAMOBJECTARBPROC  glCreateProgramObjectARB;
extern PFNGLDELETEOBJECTARBPROC         glDeleteObjectARB;
extern PFNGLDETACHOBJECTARBPROC			glDetachObjectARB;
extern PFNGLGETATTACHEDOBJECTSARBPROC	glGetAttachedObjectsARB;
extern PFNGLUSEPROGRAMOBJECTARBPROC     glUseProgramObjectARB;
extern PFNGLCREATESHADEROBJECTARBPROC   glCreateShaderObjectARB ;
extern PFNGLSHADERSOURCEARBPROC         glShaderSourceARB;
extern PFNGLCOMPILESHADERARBPROC        glCompileShaderARB;
extern PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
extern PFNGLATTACHOBJECTARBPROC         glAttachObjectARB;
extern PFNGLGETINFOLOGARBPROC           glGetInfoLogARB;
extern PFNGLLINKPROGRAMARBPROC          glLinkProgramARB;
extern PFNGLGETUNIFORMLOCATIONARBPROC   glGetUniformLocationARB;
extern PFNGLPROGRAMPARAMETERIEXTPROC	glProgramParameteriEXT;
extern PFNGLGETACTIVEUNIFORMARBPROC		glGetActiveUniformARB;
extern PFNGLUNIFORM1IARBPROC			glUniform1iARB;
extern PFNGLUNIFORM1FARBPROC				glUniform1fARB;
extern PFNGLUNIFORM4FVARBPROC			glUniform4fvARB;
extern PFNGLUNIFORM4IVARBPROC			glUniform4ivARB;
extern PFNGLUNIFORMMATRIX4FVARBPROC		glUniformMatrix4fvARB;

extern PFNGLDELETEFRAMEBUFFERSEXTPROC	glDeleteFramebuffersEXT;
extern PFNGLGENRENDERBUFFERSEXTPROC		glGenRenderbuffersEXT;
extern PFNGLDELETERENDERBUFFERSEXTPROC	glDeleteRenderbuffersEXT;
extern PFNGLGENFRAMEBUFFERSEXTPROC		glGenFramebuffersEXT;
extern PFNGLBINDRENDERBUFFEREXTPROC		glBindRenderbufferEXT;
extern PFNGLRENDERBUFFERSTORAGEEXTPROC  glRenderbufferStorageEXT;
extern PFNGLBINDFRAMEBUFFEREXTPROC		glBindFramebufferEXT;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC	glFramebufferTexture2DEXT;
extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC	glFramebufferRenderbufferEXT;
extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC	glCheckFramebufferStatusEXT;
extern PFNGLDRAWBUFFERSARBPROC			glDrawBuffersARB;

extern PFNGLDELETEQUERIESARBPROC		glDeleteQueriesARB;
extern PFNGLGENQUERIESARBPROC			glGenQueriesARB;
extern PFNGLGETQUERYOBJECTUIVARBPROC	glGetQueryObjectuivARB;
extern PFNGLBEGINQUERYARBPROC			glBeginQueryARB;
extern PFNGLENDQUERYARBPROC				glEndQueryARB;

void SetupOGLExtensions();

#endif