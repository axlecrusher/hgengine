#include <Texture.h>
//#include <RenderableNode.h>
#include <ImageLoader.h>
#include <MercuryNode.h>
#include <GLHeaders.h>
#include <Shader.h>
#include <assert.h>

using namespace std;

REGISTER_ASSET_TYPE(Texture);

Texture::Texture()
	:MercuryAsset(), m_raw(NULL),m_textureID(0)
{
	if (!m_initTextureSuccess)
	{
		m_initTextureSuccess = true;
		m_numActiveTextures = 0;
	}
	
	SetIgnoreCull( true );
}

Texture::~Texture()
{
	REMOVE_ASSET_INSTANCE(TEXTURE, m_path);
	
	Clean();
	
	SAFE_DELETE(m_raw);
}

void Texture::Clean()
{
	if (m_textureID) { GLCALL( glDeleteTextures(1, &m_textureID) ); }
	m_textureID = 0;
}

void Texture::LoadFromRaw()
{
	if ( !m_raw ) return;
	if ( !m_textureID ) { GLCALL( glGenTextures(1, &m_textureID) ); }
	
//	m_raw = raw;
	GLenum byteType = ToGLColorType( m_raw->m_ColorByteType );
	
	GLCALL( glBindTexture(GL_TEXTURE_2D, m_textureID) );
/*	
	glTexImage2D(GL_TEXTURE_2D,
				0,
				ByteType,
				m_raw->m_width,
				m_raw->m_height,
				0,
				ByteType,
				GL_UNSIGNED_BYTE,
				m_raw->m_data);
*/
	GLCALL( gluBuild2DMipmaps( GL_TEXTURE_2D, byteType, m_raw->m_width, m_raw->m_height, byteType, GL_UNSIGNED_BYTE, m_raw->m_data ) );
	
	GLCALL( glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST) );
	GLCALL( glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR) );

//	GLCALL( glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ) );
	
	GLCALL( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP) );
	GLCALL( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP) );
	
//	GLCALL( gluBuild2DMipmaps( GL_TEXTURE_2D, 3, m_raw->m_width, m_raw->m_height, ByteType, GL_UNSIGNED_BYTE, m_raw->m_data ) );
	SAFE_DELETE(m_raw);
};

void Texture::Render(const MercuryNode* node)
{
	if (GetLoadState() == LOADED)
	{
		LoadFromRaw();
		SetLoadState(NONE);
	}
	BindTexture();
}

void Texture::PostRender(const MercuryNode* node)
{
	UnbindTexture();
}

void Texture::LoadFromXML(const XMLNode& node)
{
	bool dynamic = false;
	if ( !node.Attribute("dynamic").empty() )
		dynamic = node.Attribute("dynamic")=="true"?true:false;
	
	MString file = node.Attribute("file");
	
	if ( dynamic )
		MakeDynamic( 0, 0, RGBA, file );
	else
		LoadImagePath( file );
}

void Texture::BindTexture()
{
	if ( !m_lastBound ) InitiateBindCache();
	
	if (m_numActiveTextures >= m_maxActiveTextures) return;
	
	m_textureResource = GL_TEXTURE0+m_numActiveTextures;
	GLCALL( glActiveTexture( m_textureResource ) );
	GLCALL( glClientActiveTextureARB(m_textureResource) );
	GLCALL( glEnableClientState(GL_TEXTURE_COORD_ARRAY) );
	GLCALL( glEnable( GL_TEXTURE_2D ) );
	
	if (m_lastBound[m_numActiveTextures] != this)
	{
		GLCALL( glBindTexture(GL_TEXTURE_2D, m_textureID) );
		m_lastBound[m_numActiveTextures] = this;
		++m_textureBinds;
	}
	GLCALL( glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ) );
	
	GLERRORCHECK;
	
	ShaderAttribute sa;
	sa.type = ShaderAttribute::TYPE_SAMPLER;
	sa.value.iSampler = m_numActiveTextures;
	Shader::SetAttribute( ssprintf("HG_Texture%d", m_numActiveTextures), sa);

	m_activeTextures.push_back(this);
	
	++m_numActiveTextures;
}

void Texture::UnbindTexture()
{
	GLCALL( glActiveTexture( m_textureResource ) );
	GLCALL( glClientActiveTextureARB(m_textureResource) );
	GLCALL( glDisableClientState(GL_TEXTURE_COORD_ARRAY) );
	GLCALL( glDisable( GL_TEXTURE_2D ) );
	GLERRORCHECK;
	
	Shader::RemoveAttribute( ssprintf("HG_Texture%d", m_numActiveTextures) );
	m_activeTextures.pop_back();
	
	--m_numActiveTextures;
}

void Texture::InitiateBindCache()
{
	GLint x;
	GLCALL( glGetIntegerv( GL_MAX_TEXTURE_UNITS, &x ) );
	m_lastBound = new Texture*[x];
	m_maxActiveTextures = x;
	
	for ( x = 0; x < m_maxActiveTextures; ++x)
		m_lastBound[x] = NULL;
}

void Texture::LoadImagePath(const MString& path)
{
	if (m_isInstanced) return;
	if ( !path.empty() )
	{
		SetLoadState(LOADING);
		ADD_ASSET_INSTANCE(Texture, path, this);
		m_path = path;
		
//		MercuryThread loaderThread;
//		ImageLoader::LoadImageThreaded(this, m_filename );
		ImageLoader::GetInstance().LoadImageThreaded(this);
//		LoadFromRaw();
//		RawImageData* d = ImageLoader::GetInstance().LoadImage( m_filename );
//		if (d) LoadFromRaw( d );
//		m_raw = d;
	}
}

void Texture::SetRawData(RawImageData* raw)
{
	SAFE_DELETE(m_raw);
	m_raw = raw;
}

void Texture::MakeDynamic(uint16_t width, uint16_t height, ColorByteType cbt, const MString& name)
{
//	Clean();

	SetLoadState(LOADED);
	
	REMOVE_ASSET_INSTANCE(TEXTURE, m_path);
	m_path = "DYNATEXT"+name;
	ADD_ASSET_INSTANCE(Texture, m_path, this);
	
	if (m_textureID == 0) { GLCALL( glGenTextures( 1, &m_textureID ) ); }
	GLCALL( glBindTexture( GL_TEXTURE_2D, m_textureID ) );
	GLCALL( glCopyTexImage2D( GL_TEXTURE_2D, 0, ToGLColorType(cbt), 0, 0, width, height, 0 ) );
	GLCALL( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ) );
	GLCALL( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) );
	GLCALL( glBindTexture( GL_TEXTURE_2D, 0 ) );

	GLERRORCHECK;
}

Texture* Texture::Generate()
{
	return new Texture();
}

MAutoPtr< Texture > Texture::LoadFromFile(const MString& path)
{
	MAutoPtr< MercuryAsset > t( AssetFactory::GetInstance().Generate("Texture", path) );
	Texture *a = (Texture*)t.Ptr();
	a->LoadImagePath( path );
	return a;
}

MAutoPtr< Texture > Texture::LoadDynamicTexture(const MString& name)
{
	MAutoPtr< MercuryAsset > t( AssetFactory::GetInstance().Generate("Texture", "DYNATEXT"+name) );
	Texture *a = (Texture*)t.Ptr();
//	a->LoadImagePath( path );
	return a;
}

bool Texture::m_initTextureSuccess = false;
uint8_t Texture::m_numActiveTextures = 0;
uint32_t Texture::m_textureBinds = 0;
std::list< Texture* > Texture::m_activeTextures;
Texture** Texture::m_lastBound = NULL;
uint8_t Texture::m_maxActiveTextures = 0;

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
