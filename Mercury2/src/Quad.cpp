#include <Quad.h>

#include <GLHeaders.h>


REGISTER_ASSET_TYPE(Quad);

Quad::Quad( const MString & key, bool bInstanced )
	:MercuryVBO( key, bInstanced ), m_bFlipV( true )
{
	ChangeKey( key );
}

Quad::~Quad()
{
}

void Quad::LoadFromXML(const XMLNode& node)
{
	//If we've already started with the same key - this function will just return.
	ChangeKey( node.Attribute("file") );

	MercuryAsset::LoadFromXML( node );
}



bool Quad::ChangeKey( const MString & sDescription )
{
	if( sDescription == m_path && GetLoadState() != NONE )
		return true;

	float lX = -0.5;
	float lY = -0.5;
	float hX = 0.5;
	float hY = 0.5;
	float zp = 0;
	m_bFlipV = true;

	AllocateIndexSpace(6);
	AllocateVertexSpace(4);

	MVector< MString > vsDescription;
	SplitStrings( sDescription, vsDescription, ",", " ", 1, 1 );

	//First, check for known attributes...
	do
	{
		if( !vsDescription.size() )
			break;

		if( vsDescription[0] == "straightv" )
		{
			m_bFlipV = false;
			vsDescription.remove( 0 );
		}
		else
			break;
	} while(1);

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
		return false;
	}
	
//	float* buffer = m_vertexData.m_vertexData();
	int i = 0;
	
	//UV oriented so 0,0 is lower left and 1,0 upper right.
	//this makes it so FBO images render correctly right out of the buffer, no flip needed
	m_vertexData[i++] = 0; m_vertexData[i++] = (m_bFlipV)?0:1;
	m_vertexData[i++] = 0; m_vertexData[i++] = 0; m_vertexData[i++] = -1.0;
	m_vertexData[i++] = lX; m_vertexData[i++] = lY; m_vertexData[i++] = zp;
	
	m_vertexData[i++] = 1; m_vertexData[i++] = (m_bFlipV)?0:1;
	m_vertexData[i++] = 0; m_vertexData[i++] = 0; m_vertexData[i++] = -1.0;
	m_vertexData[i++] = hX; m_vertexData[i++] = lY; m_vertexData[i++] = zp;
	
	m_vertexData[i++] = 1; m_vertexData[i++] = (m_bFlipV)?1:0;
	m_vertexData[i++] = 0; m_vertexData[i++] = 0; m_vertexData[i++] = -1.0;
	m_vertexData[i++] = hX; m_vertexData[i++] = hY; m_vertexData[i++] = zp;
	
	m_vertexData[i++] = 0; m_vertexData[i++] = (m_bFlipV)?1:0;
	m_vertexData[i++] = 0; m_vertexData[i++] = 0; m_vertexData[i++] = -1.0;
	m_vertexData[i++] = lX; m_vertexData[i++] = hY; m_vertexData[i++] = zp;
	
	m_indexData[5] = m_indexData[0] = 0;
	m_indexData[1] = 1;
	m_indexData[3] = m_indexData[2] = 2;
	m_indexData[4] = 3;

	SetLoadState( LOADED );

	return MercuryVBO::ChangeKey( sDescription );
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


