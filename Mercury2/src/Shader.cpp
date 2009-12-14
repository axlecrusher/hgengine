#include <Shader.h>
#include <MercuryNode.h>
#include <MercuryFile.h>
#include <assert.h>
#include <GLHeaders.h>
#include <string.h>

//Because we need to dynamically check for glProgramParameteriEXT, even in Linux.
#ifndef WIN32
#include <GL/glx.h>
#endif

using namespace std;

REGISTER_ASSET_TYPE( Shader );

Shader * Shader::CurrentShader = NULL;

//On many GL Implementations, this is super wonky.
//It may not even exist at all, so we make it optional.
bool IsCustomGLProgramParISet = false;
PFNGLPROGRAMPARAMETERIEXTPROC CustomGLProgramParI;

ShaderAttribute * ShaderAttributesSet::GetHandle( const MString & sName )
{
	ShaderAttribute * ret = m_AllShaderAttributes[sName];
	if( !ret )
	{
		ret = new ShaderAttribute();
		m_AllShaderAttributes[sName] = ret;
	}
	return ret;
}

Shader::Shader( const MString & key, bool bInstanced ) :
	MercuryAsset( key, bInstanced ), fPriority( 0 )
{
	iProgramID = (GLhandleARB)NULL; 
	vertexShader = (GLhandleARB)NULL;
	fragmentShader = (GLhandleARB)NULL;
	geometryShader = (GLhandleARB)NULL;
	iTimeCode[0] = 0;
	iTimeCode[1] = 0;
	iTimeCode[2] = 0;

#ifdef WIN32
	CustomGLProgramParI = glProgramParameteriEXT;
#else
	if( !IsCustomGLProgramParISet )
	{
		CustomGLProgramParI = (PFNGLPROGRAMPARAMETERIEXTPROC)glXGetProcAddress( (GLubyte*)"glProgramParameteriEXT" );
		IsCustomGLProgramParISet = true;
	}
#endif
}

Shader::~Shader()
{
	DestroyShader( );
}

void Shader::Render(const MercuryNode* node)
{
	bool bApply = true;

	CheckForNewer();

	//If there's a currnet shader, we may want to abort switching shaders
	if( CurrentShader )
	{
		if( CurrentShader->fPriority > fPriority )
			bApply = false;
	}
	if( bApply )
	{
		OriginalShader = CurrentShader;
		CurrentShader = this;
		ActivateShader();
	}
}

void Shader::PostRender(const MercuryNode* node)
{
	CurrentShader = OriginalShader;
	if( CurrentShader )
		CurrentShader->ActivateShader();
	else
		DeactivateShader();
}

void Shader::LoadFromXML(const XMLNode& node)
{
	//LoadShader( node.Attribute("file"), ) ) );

	fPriority = StrToFloat( node.Attribute("priority" ) );
	ChangeKey( node.Attribute("file") );
}

bool Shader::ChangeKey( const MString& path )
{
	if( GetLoadState() != NONE && path == m_path )
		return true;
	
	sShaderName = path;
	LoadShader( );

	return MercuryAsset::ChangeKey( path );
}

bool Shader::LoadShader( )
{
	GetTimeCodes( iTimeCode );

	MString s1 = sShaderName, s2 = sShaderName, s3 = sShaderName;
	MercuryFile * f1;
	MercuryFile * f2;
	MercuryFile * f3; //geometry
	char * Buffer;
	int i;

	s1 += ".frag";
	s2 += ".vert";
	s3 += ".geom";
	f1 = FILEMAN.Open( s1 );
	f2 = FILEMAN.Open( s2 );
	f3 = FILEMAN.Open( s3 );

	if( f1 == 0 || f2 == 0 )
	{
		if( !f1 )
			LOG.Write(ssprintf( "Could not open %s.", (char*)s1.c_str() ));
		if( !f2 )
			LOG.Write(ssprintf( "Could not open %s.", (char*)s2.c_str() ));
		return false;
	}
	if( f1 )
	{
		i = f1->Length();
		Buffer = (char*)malloc( i+1 );
		f1->Read( Buffer, i );
		f1->Close();
		LOG.Write( "Compiling: " + s1 );
		Buffer[i] = '\0';
		if( !LoadShaderFrag( Buffer ) )
		{
			free( Buffer );
			if( f2 )
				f2->Close();
			if( f3 )
				f3->Close();
			LOG.Write("Reporting failed shaderload. Not linking." );
			return false;
		}
		free( Buffer );
	}
	if( f2 )
	{
		i = f2->Length();
		Buffer = (char*)malloc( i+1 );
		f2->Read( Buffer, i );
		f2->Close();
		Buffer[i] = '\0';
		LOG.Write("Compiling: "+s2);
		if( !LoadShaderVert( Buffer ) )
		{
			if( f3 )
				f3->Close();
			free( Buffer );
			LOG.Write("Reporting failed shaderload. Not linking." );
			return false;
		}
		free( Buffer );
	}
	if( f3 )
	{
		i = f3->Length();
		Buffer = (char*)malloc( i+1 );
		f3->Read( Buffer, i );
		f3->Close();
		Buffer[i] = '\0';
		LOG.Write("Compiling: "+s3);
		if( !LoadShaderGeom( Buffer ) )
		{
			free( Buffer );
			LOG.Write("Reporting failed shaderload. Not linking." );
			return false;
		}
		free( Buffer );
	}
	
	return LinkShaders();
}

bool Shader::LoadShaderFrag( const char * sShaderCode )
{
	if( strlen( sShaderCode ) < 5 )
		return false;

	GLint bFragCompiled;
	GLint stringLength;
	fragmentShader = GLCALL( glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB ) );
	GLCALL( glShaderSourceARB( fragmentShader, 1, &sShaderCode, NULL ) );
	GLCALL( glCompileShaderARB( fragmentShader ) );

	GLCALL( glGetObjectParameterivARB( fragmentShader, GL_OBJECT_COMPILE_STATUS_ARB, &bFragCompiled ) );
	GLCALL( glGetObjectParameterivARB( fragmentShader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &stringLength ) );
	if ( stringLength > 1 )
	{
		char * tmpstr = (char*)malloc( stringLength + 1 );
		GLCALL( glGetInfoLogARB( fragmentShader, stringLength, NULL, tmpstr ) );
		puts( "Compiling Fragment Shader response follows:" );
		puts( tmpstr );
		free( tmpstr );
		return bFragCompiled!=0;
	}
	return true;
}

bool Shader::LoadShaderVert( const char * sShaderCode )
{
	if( strlen( sShaderCode ) < 5 )
		return false;

	GLint bVertCompiled;
	GLint stringLength;
	//Create a new vertex shader
	vertexShader = GLCALL( glCreateShaderObjectARB( GL_VERTEX_SHADER_ARB ) );
	//Bind the shader to the text, setting that to be its source.
	GLCALL( glShaderSourceARB( vertexShader, 1, &sShaderCode, NULL ) );
	//Compile the shader
	GLCALL( glCompileShaderARB( vertexShader ) );
	//Did the shader compile?  Were there any errors?
	GLCALL( glGetObjectParameterivARB( vertexShader, GL_OBJECT_COMPILE_STATUS_ARB, &bVertCompiled ) );
	GLCALL( glGetObjectParameterivARB( vertexShader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &stringLength ) );
	if( stringLength > 1 )
	{
		char * tmpstr = (char*)malloc( stringLength + 1 );
		GLCALL( glGetInfoLogARB( vertexShader, stringLength, NULL, tmpstr ) );
		puts( "Compiling Vertex Shader response follows:" );
		puts( tmpstr );
		free( tmpstr );
		return bVertCompiled!=0;
	}

	return true;
}

bool Shader::LoadShaderGeom( const char * sShaderCode )
{
	if( strlen( sShaderCode ) < 5 )
		return false;

	GLint bGeomCompiled;
	GLint stringLength;
	//Create a new geometry shader
	geometryShader = GLCALL( glCreateShaderObjectARB( GL_GEOMETRY_SHADER_EXT ) );
	//Bind the shader to the text, setting that to be its source.
	GLCALL( glShaderSourceARB( geometryShader, 1, &sShaderCode, NULL ) );
	//Compile the shader
	GLCALL( glCompileShaderARB( geometryShader ) );
	//Did the shader compile?  Were there any errors?
	GLCALL( glGetObjectParameterivARB( geometryShader, GL_OBJECT_COMPILE_STATUS_ARB, &bGeomCompiled ) );
	GLCALL( glGetObjectParameterivARB( geometryShader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &stringLength ) );
	if( bGeomCompiled == 0 )
	{
		char * tmpstr = (char*)malloc( stringLength + 1 );
		GLCALL( glGetInfoLogARB( geometryShader, stringLength, NULL, tmpstr ) );
		puts( "Compiling Geometry Shader response follows:" );
		puts( tmpstr );
		free( tmpstr );
		return bGeomCompiled!=0;
	}
	return true;
}

bool Shader::LinkShaders()
{
	GLint bLinked;
	GLint stringLength;
	//Create the actual shader prgoram
	iProgramID = GLCALL( glCreateProgramObjectARB() );
	//Attach the fragment/vertex shader to it.
	if( vertexShader )
	{
		GLCALL( glAttachObjectARB( iProgramID, vertexShader ) );
	}
	if( fragmentShader )
	{
		GLCALL( glAttachObjectARB( iProgramID, fragmentShader ) );
	}
	if( geometryShader )
	{
		GLCALL( glAttachObjectARB( iProgramID, geometryShader ) );
	}
	//Attempt to link the shader
	GLCALL( glLinkProgramARB( iProgramID ) );

	//If we're using a geometry shader, we have to do a little extra.
	if( CustomGLProgramParI && geometryShader )
	{
		CustomGLProgramParI( iProgramID, GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES );
		CustomGLProgramParI( iProgramID, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP );

		int ierror, i;
		GLint imaxvert;
		GLCALL( glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT,&imaxvert) );
		if( (ierror = glGetError()) != 0 )
		{
			puts( "ERROR: You cannot load a geometry shader when there are still errors left in OpenGL." );
			puts( "Please track down the error remaining by using glGetError() to cordon off your code." );
			LOG.Write( ssprintf( "The last error received was: %d", ierror ) );
		}
		for( i = 1; i < imaxvert; i++ )
		{
			CustomGLProgramParI(iProgramID,GL_GEOMETRY_VERTICES_OUT_EXT,imaxvert/i);
			if( glGetError() == 0 )
				break;
		}
		LOG.Write(ssprintf( "Geometry Shader loaded with a total of %d max vertices.  Because there are %d max vertices, and %d preceived components per vert.", imaxvert/i, imaxvert, i ));
	}


	//See if there were any errors.
	GLCALL( glGetObjectParameterivARB( iProgramID, GL_OBJECT_LINK_STATUS_ARB, &bLinked ) );
	GLCALL( glGetObjectParameterivARB( iProgramID, GL_OBJECT_INFO_LOG_LENGTH_ARB, &stringLength ) );

	if ( stringLength > 1 || bLinked == 0 )
	{
		char * tmpstr = (char*)malloc( stringLength + 1 );
		GLCALL( glGetInfoLogARB( iProgramID, stringLength, NULL, tmpstr ) );
		puts( "Linking shaders. response follows:" );
		puts( tmpstr );
		free( tmpstr );
		DestroyShader();
		return bLinked!=0;
	}

	//Build the list of uniform tabs.
	int iNumUniforms;
	GLCALL( glGetObjectParameterivARB( iProgramID, GL_OBJECT_ACTIVE_UNIFORMS_ARB, &iNumUniforms ) );
	m_uniforms.clear();
	for( int i = 0; i < iNumUniforms; ++i )
	{
		char buffer[1024];
		int bufflen;
		GLint size;
		GLenum type;
		GLCALL( glGetActiveUniformARB( iProgramID, i, 1024, &bufflen, &size, &type, buffer ) );
		buffer[bufflen] = 0;
//		m_uniforms[buffer] = GLCALL( glGetUniformLocationARB( iProgramID, buffer ) );
		int location = GLCALL( glGetUniformLocationARB( iProgramID, buffer ) );
		m_uniforms.push_back( UniformMap(buffer, location) );
	}
	return true;
}

void Shader::DestroyShader()
{
	GLhandleARB *objects = NULL;
	GLint i, count = -1;

	if( !iProgramID )
		return;

	//If we can't destroy the object, then don't try.
	GLCALL( glGetObjectParameterivARB(iProgramID, GL_OBJECT_ATTACHED_OBJECTS_ARB, &count) );

	//Iterate through all children.
	if (count > 0)
	{
		objects = (GLhandleARB *)malloc(count*sizeof(GLhandleARB));
		GLCALL( glGetAttachedObjectsARB(iProgramID, count, NULL, objects) );
	}
	else
		return;

	for ( i = 0; i < count; ++i)
	{
		GLCALL( glDetachObjectARB(iProgramID, objects[i]) );
	}

	GLCALL( glDeleteObjectARB(iProgramID) );
	
	iProgramID = 0;
	free( objects );
	return;
}

void Shader::CheckForNewer( )
{
	unsigned long iCurTimes[3];
	GetTimeCodes( iCurTimes );

	if( iCurTimes[0] != iTimeCode[0] || iCurTimes[1] != iTimeCode[1] || iCurTimes[2] != iTimeCode[2] )
	{
		DestroyShader( );
		LoadShader( );
	}
}

void Shader::GetTimeCodes( unsigned long * iOut )
{
	MercuryFile * f = FILEMAN.Open( sShaderName + ".frag" );
	if( f )
	{
		iOut[0] = f->GetModTime();
		f->Close();
	} else
		iOut[0] = 0;

	f = FILEMAN.Open( sShaderName + ".vert" );
	if( f )
	{
		iOut[1] = f->GetModTime();
		f->Close();
	} else
		iOut[1] = 0;

	f = FILEMAN.Open( sShaderName + ".geom" );
	if( f )
	{
		iOut[2] = f->GetModTime();
		f->Close();
	} else
		iOut[2] = 0;
}

void Shader::ActivateShader()
{
	if ( !iProgramID ) return;
	
	GLCALL( glUseProgramObjectARB( iProgramID ) );
	GLERRORCHECK;
	
	//set attributes here
	std::list< UniformMap >::iterator ui = m_uniforms.begin();
	for (;ui != m_uniforms.end(); ++ui)
	{
		ShaderAttribute* a = m_globalAttributes.get( ui->name );
		if( a )
			SetAttributeInternal(ui->name,*a);
	}
}

void Shader::DeactivateShader()
{
	GLCALL( glUseProgramObjectARB( 0 ) );
	GLERRORCHECK;
}

int32_t Shader::GetUniformLocation(const MString& n)
{
	if ( !iProgramID ) return -1;
	
	std::list< UniformMap >::iterator i = m_uniforms.begin();
	for (;i != m_uniforms.end(); ++i)
		if (i->name == n) return i->id;
	return -1;
}

void Shader::SetAttribute(const MString& name, const ShaderAttribute& x)
{
	m_globalAttributes[name] = x;

	Shader *current = GetCurrentShader();
	if (current) current->SetAttributeInternal( name, x );
}

void Shader::RemoveAttribute(const MString& name)
{
	m_globalAttributes.remove( name );
	//no sense in unsetting it in the current shader, what would we set it to?
}

void Shader::SetAttributeInternal(const MString& name, const ShaderAttribute& x)
{
	int location = GetUniformLocation( name );

	if ( location != -1 )
	{
		switch( x.type )
		{
			case ShaderAttribute::TYPE_INT:
			case ShaderAttribute::TYPE_SAMPLER:
				GLCALL( glUniform1iARB( location, x.value.iInt ) );
				break;
			case ShaderAttribute::TYPE_FLOAT:
				GLCALL( glUniform1fARB( location, x.value.fFloat ) );
				break;
			case ShaderAttribute::TYPE_FLOATV4:
				GLCALL( glUniform4fvARB( location, 1, &x.value.fFloatV4[0] ) );
				break;
			case ShaderAttribute::TYPE_MATRIX:
				GLCALL( glUniformMatrix4fvARB(location, 1, 1, x.value.matrix) ); //transpose too
				break;
			case ShaderAttribute::TYPE_INT4:
				GLCALL( glUniform4ivARB( location, 1, x.value.iInts ) );
				break;
		};
		GLERRORCHECK;
	}
}

MHash< ShaderAttribute> Shader::m_globalAttributes;

/* 
 * Copyright (c) 2009 Charles Lohr
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *	-	Redistributions of source code must retain the above
 *		copyright notice, this list of conditions and the following disclaimer.
 *	-	Redistributions in binary form must reproduce the above copyright
 *		notice, this list of conditions and the following disclaimer in
 *		the documentation and/or other materials provided with the distribution.
 *	-	Neither the name of the Mercury Engine nor the names of its
 *		contributors may be used to endorse or promote products derived from
 *		this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
