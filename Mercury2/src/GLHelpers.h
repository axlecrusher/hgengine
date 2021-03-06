#ifndef GLHELPERS_H
#define GLHELPERS_H

#include <global.h>
#include <MercuryUtil.h>
#include <MercuryMatrix.h>
#include <MercuryVertex.h>

#include <RawImageData.h>

#ifdef GL_PROFILE
#define GLCALL(x) x; ProfileGLCall(#x);
#else
#define GLCALL(x) x;
#endif

MString GlError2String(uint32_t e);
void glLoadMatrix(const MercuryMatrix& m);
MercuryMatrix glGetMatrix(unsigned int m);
MercuryVertex pointFromScreenLoc(int screen_x, int screen_y);
MercuryVertex pointFromScreenLoc(int screen_x, int screen_y, float fForceDepth);
void CameraPointAndRay(int screen_x, int screen_y, MercuryVertex & p, MercuryVertex & r);
unsigned int ToGLColorType(ColorByteType cbt);

#ifdef GL_PROFILE
void ProfileGLCall(const MString& funcName);
#endif
void PrintGLFunctionCalls();

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
