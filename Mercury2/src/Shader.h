#ifndef SHADER_H
#define SHADER_H

#include <MercuryAsset.h>
#include <map>
#include <vector>

///Basic Attribute for all shaders
class ShaderAttribute
{
public:
	ShaderAttribute() : type( TYPE_INT ) { value.iInt = 0; ShaderControlled=0;}

	///Type of ShaderAttribute for shader
	enum ShaderAttributeTyp
	{
		TYPE_INT,		///Synonomous to 'int' when passing into a shader
		TYPE_SAMPLER,		///Synonomous to 'sampler2D' when passing into a shader
		TYPE_FLOAT,		///Synonomous to 'float' when passing into a shader
		TYPE_FLOATV4		///Synonomous to 'vec4' when passing into a shader
	} type;

	///Actual data for value.
	union ShaderAttributeVal
	{
		int		iInt;		///Synonomous to 'int'
		unsigned int	iSampler;	///Synonomous to 'sampler2D'
		float 		fFloat;		///Synonomous to 'float'
		float		fFloatV4[4]; 	///Synonomous to 'vec4'
	} value;
	
	MString name;
	bool ShaderControlled;
};

///Shader Attribute Retainer
class ShaderAttributesSet
{
public:
	ShaderAttribute * GetHandle( const MString & sName );
private:
	///All shader attributes
	/** This contains a list of all attributes that are passed in.
	    If a shader does not have an attribute, it does not insert
	    it here.  Note that if shaders change and an element is
	    no longer referenced, it will remain in the map, as to
	    prevent bad pointers.  This list is all-enduring.
	*/
	std::map< MString, ShaderAttribute * > m_AllShaderAttributes;
};

extern ShaderAttributesSet SHADERATTRIBUTES;

///Basic element for turning shaders on and off
/** This class helps aide in the loading and use of shaders.  It allows loading of files
    through the LoadShader() function that actively looks for .frag and .vert files.  By use
    of the CheckForNewer() function, the class looks for changes, if any changes are found
    the shaders will be re-loaded, compiled and linked.  Once this node has a shader loaded,
    the shader can be activated or deactivated using the ActivateShader() and
    DeactivateShader() functions respectively.  Note that shaders do not stack.  When you
    call DeactiveShader(), it does not bring back previously activated shaders.  It simply
    turns all shaders off. */
class Shader : public MercuryAsset
{
public:
	Shader();
	virtual ~Shader();
		
	virtual void Init(MercuryNode* node);
	virtual void Render(const MercuryNode* node);
	virtual void PostRender(const MercuryNode* node);
	static Shader* Generate() { return new Shader; }
	virtual void LoadFromXML(const XMLNode& node);
	int32_t GetUniformLocation(const MString& n);

	///Explicitly get the OpenGL ProgramID in the event you need it for advanced techniques
	unsigned int	GetProgramID() { return iProgramID; }
	inline static Shader* GetCurrentShader() { return CurrentShader; }
private:
	void LoadShader( const MString& path, float priority );
	
	///Suggested function for loading shaders.
	/** This function looks for {sShaderName}.vert and {sShaderName}.frag.  It will
	    attempt to load, compile and link the files.  If any errors are found, they will
	    be announced at STDOUT.  When Geometry shader support is added, it will search
	    for .geom files */
	bool LoadShader(  );

	///Explicitly load a fragment shader
	/** This function takes on raw code in the sShaderCode string and attemps to compile
	    it.  Any errors it runs into will be displayed at STDOUT.  Note you are
            discouraged to use this function, in favor of using LoadShader(). */
	bool LoadShaderFrag( const char * sShaderCode );

	///Explicitly load a vertex shader
	/** This function takes on raw code in the sShaderCode string and attemps to compile
	    it.  Any errors it runs into will be displayed at STDOUT. You should use
	    LoadShader() instead of this function. */
	bool LoadShaderVert( const char * sShaderCode );

	///Explicitly load a geometry shader
	/** This function takes on raw code in the sShaderCode string and attemps to compile
	    it.  Any errors it runs into will be displayed at STDOUT. You should use
	    LoadShader() instead of this function. */
	bool LoadShaderGeom( const char * sShaderCode );

	///Explicitly link all shaders currently loaded
	/** This takes vertexShader and fragmentShader and converts them into iProgramID.
	    this function is discouraged when using LoadShader(). */
	bool LinkShaders();

	///Check for newer version of 'this' shader
	void CheckForNewer( );

	///Get the last modified time for sShaderName
	/* This function takes on iOut as being where to put the last time the shader was modified.
	   this value is system dependent and is necessiarly not linked to anything like seconds. */
	void GetTimeCodes( unsigned long * iOut );

	///Activate Shader (apply to current OpenGL state)
	void ActivateShader();

	///Turn all shaders off in OpenGL state.
	void DeactivateShader();

	///Destroy this shader
	void DestroyShader();

	///The last time codes (for vertex and fragment shaders respectively)
	unsigned long	iTimeCode[3];

	///The OpenGL Program ID (in OpenGL Land, contains all shaders, linked together)
	unsigned int	iProgramID;

	///The OpenGL Geometry Program ID
	unsigned int	geometryShader;

	///The OpenGL Vertex Program ID
	unsigned int	vertexShader;

	///The OpenGL Fragment Program ID
	unsigned int	fragmentShader;

	///Shader attributes
	/** This is the system that helps make it possible to blast
	    through all attributes currently set up by dereferencing
	    the pointers in the attributes repository.
	*/
//	std::vector< ShaderAttribute * >  m_vShaderTabs;

	///Name of the shader
	MString sShaderName;

	///Priority of THIS shader (if lower than currently active one, it does not become active)
	float fPriority;

	///Global static shader (so shaders can recursively activate and deactivate shaders)
	static Shader * CurrentShader;

	///Original Shader (to re-enable when leaving)
	Shader * OriginalShader;
};

#endif

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
