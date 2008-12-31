#include <Texture.h>
#include <RenderableNode.h>
#include <ImageLoader.h>

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glext.h>

using namespace std;

REGISTER_ASSET_TYPE(Texture);

Texture::Texture()
	:MercuryAsset(), m_raw(NULL),m_textureID(0)
{
	if (!m_initTextureSuccess)
	{
		m_initTextureSuccess = true;
		m_activeTextures = 0;
	}
}

Texture::~Texture()
{
	REMOVE_ASSET_INSTANCE(TEXTURE, m_filename);
	
	if (m_textureID) glDeleteTextures(1, &m_textureID);
	m_textureID = 0;
	
	SAFE_DELETE(m_raw);
}

void Texture::Init(MercuryNode* node)
{
	MercuryAsset::Init( node );
	
	RenderableNode* rn;
	if ( (rn=RenderableNode::Cast( node )) )
		rn->AddPostRender( this );
}

void Texture::LoadFromRaw(const RawImageData* raw)
{
	if ( !m_textureID ) glGenTextures(1, &m_textureID);
	
	m_raw = raw;
	int ByteType;
	
	switch (m_raw->m_ColorByteType)
	{
		case RGB:
			ByteType = GL_RGB;
			break;
		case RGBA:
			ByteType = GL_RGBA;
			break;
		default:
			printf( "Unsupported byte type (%d) in Texture::LoadFromRaw\n", m_raw->m_ColorByteType );
			ByteType = GL_RGB;
			break;
	}

	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexImage2D(GL_TEXTURE_2D,
				0,
				ByteType,
				m_raw->m_width,
				m_raw->m_height,
				0,
				ByteType,
				GL_UNSIGNED_BYTE,
				m_raw->m_data);
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

//	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	
};

void Texture::Render(MercuryNode* node)
{
	BindTexture();
}

void Texture::PostRender(MercuryNode* node)
{
	UnbindTexture();
}

void Texture::LoadFromXML(const XMLNode& node)
{
	LoadImage( node.Attribute("file") );	
}

void Texture::BindTexture()
{
		m_textureResource = GL_TEXTURE0+m_activeTextures;
		glActiveTexture( m_textureResource );
		glClientActiveTextureARB(m_textureResource);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnable( GL_TEXTURE_2D );
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		++m_activeTextures;
}

void Texture::UnbindTexture()
{
	glActiveTexture( m_textureResource );
	glClientActiveTextureARB(m_textureResource);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable( GL_TEXTURE_2D );
	--m_activeTextures;
}

void Texture::LoadImage(const MString& path)
{
	if (m_isInstanced) return;
	if ( !path.empty() )
	{
		ADD_ASSET_INSTANCE(Texture, path, this);
		m_filename = path;
		RawImageData* d = ImageLoader::GetInstance().LoadImage( m_filename );
		if (d) LoadFromRaw( d );
	}
}

Texture* Texture::Generate()
{
	return new Texture();
}

MAutoPtr< Texture > Texture::LoadFromFile(const MString& path)
{
	MAutoPtr< MercuryAsset > t( AssetFactory::GetInstance().Generate("Texture", path) );
	Texture *a = (Texture*)&(*t);
	a->LoadImage( path );
	return a;
}

bool Texture::m_initTextureSuccess = false;
unsigned short Texture::m_activeTextures = 0;


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
