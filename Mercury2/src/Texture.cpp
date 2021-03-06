#include <Texture.h>
//#include <RenderableNode.h>
#include <ImageLoader.h>
#include <MercuryNode.h>
#include <GLHeaders.h>
#include <Shader.h>
#include <assert.h>

using namespace std;

REGISTER_ASSET_TYPE(Texture);

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

Texture::Texture( const MString & key, bool bInstanced )
	:MercuryAsset( key, bInstanced ), m_raw(NULL),m_textureID(0),m_bDeleteRaw(true),m_dynamic(false), m_bClamp(true), m_tFilterMode(TF_LINEAR_MIPS), m_timeStamp(0)
{
	if (!m_initTextureSuccess)
	{
		m_initTextureSuccess = true;
		m_numActiveTextures = 0;
		m_shaderBindPoints = new MString[Texture::TextureStackDepth];
		for (unsigned short i = 0; i < Texture::TextureStackDepth; ++i)
			m_shaderBindPoints[i] = ssprintf("HG_Texture%d", m_numActiveTextures);
	}
	
	SetIgnoreCull( true );
}

Texture::~Texture()
{
	Clean();	
	SAFE_DELETE(m_raw);
}

void Texture::Clean()
{
	if (m_textureID) { GLCALL( glDeleteTextures(1, &m_textureID) ); }
	for (uint8_t i = 0; i < m_maxActiveTextures; ++i)
	{
		if (m_lastBound[i] == this)
		{
			Deactivate(GL_TEXTURE0 + i);
			m_lastBound[i] = 0;
		}
	}
	m_textureID = 0;
}

void Texture::LoadFromRaw()
{
	if ( !m_raw ) return;
	if ( !m_textureID ) { GLCALL( glGenTextures(1, &m_textureID) ); }
	
//	m_raw = raw;
	GLenum byteType = ToGLColorType( m_raw->m_ColorByteType );
	
	GLCALL( glPushAttrib( GL_TEXTURE_BIT ) );
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
	if( m_tFilterMode == TF_NONE )
	{
		GLCALL( glTexImage2D(GL_TEXTURE_2D, 0, byteType, m_raw->m_width, m_raw->m_height, 0, byteType, GL_UNSIGNED_BYTE, m_raw->m_data) );
		GLCALL( glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST) );
		GLCALL( glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST) );
	}
	else if( m_tFilterMode == TF_LINEAR )
	{
		GLCALL( glTexImage2D(GL_TEXTURE_2D, 0, byteType, m_raw->m_width, m_raw->m_height, 0, byteType, GL_UNSIGNED_BYTE, m_raw->m_data) );
		GLCALL( glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR) );
		GLCALL( glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR) );
	}
	else
	{
		GLCALL( gluBuild2DMipmaps( GL_TEXTURE_2D, byteType, m_raw->m_width, m_raw->m_height, byteType, GL_UNSIGNED_BYTE, m_raw->m_data ) );	
		GLCALL( glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR) );
		GLCALL( glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR) );
	}

//	GLCALL( glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ) );
	
	if( m_bClamp )
	{
		GLCALL( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP) );
		GLCALL( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP) );
	}
	
//	GLCALL( gluBuild2DMipmaps( GL_TEXTURE_2D, 3, m_raw->m_width, m_raw->m_height, ByteType, GL_UNSIGNED_BYTE, m_raw->m_data ) );
	if( m_bDeleteRaw )
		SAFE_DELETE(m_raw);
	
	GLCALL( glPopAttrib() );

};

void Texture::Render(const MercuryNode* node)
{
	if ( !m_lastBound ) InitiateBindCache();

	if (GetLoadState() == LOADED)
	{
		printf( "Rendering Texture (%s), but state is: %d\n", m_path.c_str(), GetLoadState() );
		LoadFromRaw();
		SetLoadState(NONE);
		//force rebind since we were used before
		if (m_lastBound[m_numActiveTextures] == this) m_lastBound[m_numActiveTextures] = NULL;

//		for (uint8_t i = 0; i <= m_numActiveTextures; i++)
//			if (m_lastBound[i] == this) m_lastBound[i] = NULL;
	}
	BindTexture();
}

void Texture::PostRender(const MercuryNode* node)
{
	UnbindTexture();
}

void Texture::LoadFromXML(const XMLNode& node)
{
	LOAD_FROM_XML( "dynamic", m_dynamic, StrToBool );
	LOAD_FROM_XML( "clamp", m_bClamp, StrToBool );
	
	MString filter = node.Attribute( "filter" );
	if( !filter.empty() )
	{
		if( filter == "none" )
		{
			m_tFilterMode = TF_NONE;
		}
		else if( filter == "linear" )
		{
			m_tFilterMode = TF_LINEAR;
		}
	}


	MString file = node.Attribute("file");

	ChangeKey( file );
}

bool Texture::ChangeKey( const MString & sNewKey )
{
	if( sNewKey == m_path && GetLoadState() != NONE )
		return true;

	if ( !m_dynamic )
		LoadImagePath( sNewKey );
	else
//		MakeDynamic( 0, 0, RGBA, sNewKey );

	if( sNewKey != m_path )
		return MercuryAsset::ChangeKey( sNewKey );
	return true;
}

void Texture::BindTexture()
{
//	if ( !m_lastBound ) InitiateBindCache();
	
	if (m_numActiveTextures >= m_maxActiveTextures) return;
	
	if (m_lastBound[m_numActiveTextures] != this)
	{
//		We don't really even have to disable old spots
//		if ( m_lastBound[m_numActiveTextures] != NULL)
//			m_lastBound[m_numActiveTextures]->Deactivate();
		
		Activate(GL_TEXTURE0 + m_numActiveTextures);
		
		GLCALL( glBindTexture(GL_TEXTURE_2D, m_textureID) );
		
		GLCALL( glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ) );
		
		m_lastBound[m_numActiveTextures] = this;
		++m_textureBinds;
	}
	
	GLERRORCHECK;
	
	ShaderAttribute sa;
	sa.type = ShaderAttribute::TYPE_SAMPLER;
	sa.value.iSampler = m_numActiveTextures;
	Shader::SetAttribute( m_shaderBindPoints[m_numActiveTextures], sa);

	m_activeTextures.Push(this);

	++m_numActiveTextures;
}

void Texture::UnbindTexture()
{
//Everything needs to be done in reverse of BindTexture()
	--m_numActiveTextures;
	
	Shader::RemoveAttribute( m_shaderBindPoints[m_numActiveTextures] );
	m_activeTextures.Pop();

//	Deactivate(GL_TEXTURE0 + m_numActiveTextures);
}

void Texture::ActiveTexture(uint32_t i)
{
	static uint32_t active = 0;
	if (active!=i)
	{
		GLCALL( glActiveTexture( i ) );
	}
	active = i;
}

void Texture::Activate(uint32_t textureResource)
{
	ActiveTexture(textureResource);
//	GLCALL( glClientActiveTextureARB(textureResource) );	//XXX: Note to self, this seems to be causing a crash, look into it.
	GLCALL( glEnableClientState(GL_TEXTURE_COORD_ARRAY) );
	GLCALL( glEnable( GL_TEXTURE_2D ) );
}

void Texture::Deactivate(uint32_t textureResource)
{
	ActiveTexture(textureResource);
//	GLCALL( glClientActiveTextureARB(textureResource) );
	GLCALL( glDisableClientState(GL_TEXTURE_COORD_ARRAY) );
	GLCALL( glDisable( GL_TEXTURE_2D ) );
	GLERRORCHECK;
}

void Texture::ApplyActiveTextures(uint16_t stride, uint8_t uvByteOffset)
{
	for (uint8_t i = 0; i < m_numActiveTextures; ++i)
	{
		ActiveTexture(GL_TEXTURE0+i);
//		GLCALL( glClientActiveTextureARB(GL_TEXTURE0+i) );	//XXX: Note to self, this seems to be causing a crash, look into it.
		GLCALL( glTexCoordPointer(2, GL_FLOAT, stride, BUFFER_OFFSET(uvByteOffset)) );
	}
	
	Texture::DisableUnusedTextures();
}

void Texture::DisableUnusedTextures()
{
	for (uint8_t i = m_numActiveTextures; i < m_maxActiveTextures; ++i)
	{
		if (m_lastBound[m_numActiveTextures] != NULL)
		{
			m_lastBound[m_numActiveTextures]->Deactivate(GL_TEXTURE0 + i);
			m_lastBound[m_numActiveTextures] = NULL;
		}
	}
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
	if ( !path.empty() )
	{
		SetLoadState(LOADING);
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

	m_path = "DYNATEXT"+name;
	
	if (m_textureID == 0) { GLCALL( glGenTextures( 1, &m_textureID ) ); }
	GLCALL( glBindTexture( GL_TEXTURE_2D, m_textureID ) );
	GLCALL( glCopyTexImage2D( GL_TEXTURE_2D, 0, ToGLColorType(cbt), 0, 0, width, height, 0 ) );
	GLCALL( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ) );
	GLCALL( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) );
	GLCALL( glBindTexture( GL_TEXTURE_2D, 0 ) );

	GLERRORCHECK;
}

bool Texture::CheckForNewer()
{
	uint32_t timeStamp = m_timeStamp;
	
	if (timeStamp == 0) return false;

	MercuryFile *f = FILEMAN.Open(m_path);
	if (f)
	{
		m_timeStamp = f->GetModTime();
		delete f;
	}
	
	return timeStamp != m_timeStamp;
}

void Texture::Reload()
{
	LoadImagePath(m_path);
}

MString Texture::GenKey(const MString& k, const XMLNode* n)
{
	bool dynamic = false;
	if (n)
	{
		const XMLNode& node = *n;
		LOAD_FROM_XML( "dynamic", dynamic, StrToBool );
	}
	if (dynamic) return "DYNATEXT"+k;
	return k;
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
ArrayStack< Texture* > Texture::m_activeTextures(Texture::TextureStackDepth);
Texture** Texture::m_lastBound = NULL;
uint8_t Texture::m_maxActiveTextures = 0;

MString* Texture::m_shaderBindPoints = NULL;

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
