#include "configuration.h"

FOLDERS="src"


SOURCES="src/Mercury2.cpp src/MercuryAsset.cpp \
	src/MercuryNode.cpp src/MercuryPoint.cpp \
	src/MercuryThreads.cpp  src/MercuryMath.cpp \
	src/MercuryWindow.cpp src/RenderableNode.cpp \
	src/TransformNode.cpp src/MercuryMatrix.cpp \
	src/Viewport.cpp src/Quad.cpp src/MercuryUtil.cpp \
	src/Texture.cpp src/RawImageData.cpp src/BMPLoader.cpp \
	src/PNGLoader.cpp src/ImageLoader.cpp"

#ifdef USE_LIBXML
SOURCES="$SOURCES src/XMLParser.cpp"
#endif

#ifdef USE_X11
SOURCES="$SOURCES src/X11Window.cpp"
#endif

PROJ="mercury"
CFLAGS="$CFLAGS -DHAVE_CONFIG -fno-exceptions -fPIC -Isrc"
LDFLAGS="$LDFLAGS -rdynamic -g -fPIC"

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

