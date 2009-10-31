#include "configuration.h"

FOLDERS="src"


SOURCES="src/Mercury2.cpp src/MercuryAsset.cpp src/MercuryNode.cpp \
	src/MercuryThreads.cpp  src/MercuryMath.cpp src/MercuryWindow.cpp\
	src/TransformNode.cpp src/MercuryMatrix.cpp src/Viewport.cpp src/Quad.cpp src/MercuryUtil.cpp \
	src/Texture.cpp src/RawImageData.cpp src/BMPLoader.cpp src/PNGLoader.cpp src/ImageLoader.cpp \
	src/MercuryVBO.cpp src/MSemaphore.cpp src/UpdateThreader.cpp src/HGMDLMesh.cpp \
	src/HGMDLModel.cpp src/MercuryString.cpp src/MercuryCrash.c src/MercuryBacktrace.c \
	src/MercuryFile.cpp src/MercuryTimer.cpp src/MercuryMessageManager.cpp src/MercuryVertex.cpp \
	src/MercuryPlane.cpp src/BoundingBox.cpp src/Shader.cpp src/RenderGraph.cpp src/Frustum.cpp \
	src/Camera.cpp src/MercuryInput.cpp src/MQuaternion.cpp src/ModuleManager.cpp src/MercuryFBO.cpp \
	src/GLHelpers.cpp src/FullscreenQuad.cpp src/MercuryNamedResource.cpp src/MercuryPrefs.cpp \
	src/MercuryTheme.cpp src/Orthographic.cpp src/Light.cpp src/RenderDifferedLights.cpp \
	src/MercuryLog.cpp src/MercuryCTA.cpp src/DataTypes/MTriangle.cpp"

SOURCES="$SOURCES src/MercuryFileDriverDirect.cpp src/MercuryFileDriverMem.cpp \
	src/MercuryFileDriverPacked.cpp src/MercuryFileDriverZipped.cpp"

//Not implemented yet, since we don't have ezsocekts
//src/MercuryFileDriverNet.cpp 

#ifdef USE_LIBXML
SOURCES="$SOURCES src/XMLParser.cpp"
#endif

#ifdef USE_X11
SOURCES="$SOURCES src/X11Window.cpp"
#endif

PROJ="mercury"
CFLAGS="$CFLAGS -DHAVE_CONFIG -DHGENGINE -fno-exceptions -fPIC -Isrc -Isrc/DataStructures -Isrc/DataTypes -g -rdynamic"
LDFLAGS="$LDFLAGS -rdynamic -g -fPIC "

/*
 * (c) 2007-2008 Charles Lohr
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

