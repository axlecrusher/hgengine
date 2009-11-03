#include <Quad.h>

#include <GLHeaders.h>


REGISTER_ASSET_TYPE(Quad);

Quad::Quad()
	:MercuryVBO()
{
}

Quad::~Quad()
{
	REMOVE_ASSET_INSTANCE(Quad,m_path);
}

Quad* Quad::Generate()
{
	Quad *asset = new Quad();
	ADD_ASSET_INSTANCE(Quad,"",asset);
	return asset;
}

void Quad::LoadFromXML(const XMLNode& node)
{
	LoadFromString( node.Attribute("file") );

	MercuryAsset::LoadFromXML( node );
}



bool Quad::LoadFromString( const MString & sDescription )
{
	float lX = -0.5;
	float lY = -0.5;
	float hX = 0.5;
	float hY = 0.5;
	float zp = 0;
	bool bResetRegistration = sDescription != m_path;

	AllocateIndexSpace(6);
	AllocateVertexSpace(4);

	if( bResetRegistration )
		REMOVE_ASSET_INSTANCE(Quad,m_path);
	m_path = sDescription;
	if( bResetRegistration )
		ADD_ASSET_INSTANCE(Quad,m_path,this);

	MVector< MString > vsDescription;
	SplitStrings( sDescription, vsDescription, ",", " ", 1, 1 );

	if( vsDescription.size() == 0 )
	{
		//Do nothing
	}
	else if( vsDescription.size() == 2 )
	{
		lX = 0;
		lY = 0;
		hX = StrToFloat( vsDescription[0] );
		hY = StrToFloat( vsDescription[1] );
	}
	else if( vsDescription.size() == 4 || vsDescription.size() == 5 )
	{
		lX = StrToFloat( vsDescription[0] );
		lY = StrToFloat( vsDescription[1] );
		hX = StrToFloat( vsDescription[2] );
		hY = StrToFloat( vsDescription[3] );
		if( vsDescription.size() == 5 )
			zp = StrToFloat( vsDescription[4] );
	}
	else
	{
		LOG.Write( ssprintf( "Invalid number of parameters passed into new Quad:  \"%s\"", sDescription.c_str() ) );
	}
	
//	float* buffer = m_vertexData.m_vertexData();
	int i = 0;
	
	//UV oriented so 0,0 is lower left and 1,0 upper right.
	//this makes it so FBO images render correctly right out of the buffer, no flip needed
	m_vertexData[i++] = 0; m_vertexData[i++] = 0;
	m_vertexData[i++] = 0; m_vertexData[i++] = 0; m_vertexData[i++] = -1.0;
	m_vertexData[i++] = lX; m_vertexData[i++] = lY; m_vertexData[i++] = zp;
	
	m_vertexData[i++] = 1; m_vertexData[i++] = 0;
	m_vertexData[i++] = 0; m_vertexData[i++] = 0; m_vertexData[i++] = -1.0;
	m_vertexData[i++] = hX; m_vertexData[i++] = lY; m_vertexData[i++] = zp;
	
	m_vertexData[i++] = 1; m_vertexData[i++] = 1;
	m_vertexData[i++] = 0; m_vertexData[i++] = 0; m_vertexData[i++] = -1.0;
	m_vertexData[i++] = hX; m_vertexData[i++] = hY; m_vertexData[i++] = zp;
	
	m_vertexData[i++] = 0; m_vertexData[i++] = 1;
	m_vertexData[i++] = 0; m_vertexData[i++] = 0; m_vertexData[i++] = -1.0;
	m_vertexData[i++] = lX; m_vertexData[i++] = hY; m_vertexData[i++] = zp;
	
	m_indexData[5] = m_indexData[0] = 0;
	m_indexData[1] = 1;
	m_indexData[3] = m_indexData[2] = 2;
	m_indexData[4] = 3;

	m_path = sDescription;

	return true;
}


/****************************************************************************
 *   Copyright (C) 2009 - 2009 by Joshua Allen                              *
 *                                Charles Lohr                              *
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


