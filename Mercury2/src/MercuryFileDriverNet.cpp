#include <MercuryFileDriverNet.h>


/********************FILE DRIVER NET**********************/

MercuryFileObjectNet::~MercuryFileObjectNet()
{
	if( m_Buffer )
		delete m_Buffer;
	if( m_pSocket )
		delete ((ezSockets*)m_pSocket);
}

bool MercuryFileObjectNet::Init( const MString & fName, FilePermission p )
{
	MercuryFile::Init(fName, p);

	ezSockets * s = new ezSockets;

	//EXPECT "http://" then "[host]" then "[document]"
	int iDist = BytesUntil( fName.c_str(), "/", 8, fName.length(), 2 );
	MString sHost = fName.substr( 7, iDist + 1 );
	MString sDocument = fName.substr( iDist + 8 );

	s->bBlocking = true;
	s->mode = ezSockets::skGeneral;
	s->Create( );
	if( !s->Connect( sHost, 80 ) )
	{
		SAFE_DELETE( s );
		return false;
	}
	MString sRequest = "GET " + sDocument + " HTTP/1.1\r\n";
	sRequest += "User-Agent: Mercury Game Engine (http://sf.net/projects/hgengine)\r\n";
	sRequest += "Host:" + sHost + "\r\n\r\n";
	s->SendData( sRequest.c_str(), sRequest.length() );
	MString str;

	bool bNotBreak = true;
	while( bNotBreak )
	{
		if( !s->ReadLine( str ) )
			return false;
		if( str.substr( 0, 14 ) == "Content-Length" )
			m_size = atoi( str.substr( 15 ).c_str() );
		else if( str.length() == 0 )
			bNotBreak = false;
	}

	if( m_size == 0 )
		return false;

	m_Buffer = (char*)malloc( m_size );
	if( !s->ReadData( m_Buffer, m_size ) )
	{
		SAFE_FREE( m_Buffer );
		return false;
	}

	m_location = 0;
	s->Close();
	m_pSocket = s;

	return true;
}

bool MercuryFileObjectNet::Seek( unsigned long position ) 
{
	if ( position < m_size ) 
	{ 
		m_location = position;
		return true;
	} 
	else 
		return false;
}

void MercuryFileObjectNet::Close()
{
}

unsigned long MercuryFileObjectNet::Tell()
{
	return m_location;
}

unsigned long MercuryFileObjectNet::Length()
{
	return m_size;
}

unsigned long MercuryFileObjectNet::Read( void * data, unsigned long length )
{
	unsigned int iRead;
	if( length >= m_size - m_location )
		iRead = m_size - m_location - 1;
	else
		iRead = length;
	memcpy( data, m_Buffer + m_location, length );
	m_location += iRead;
	return iRead;
}

bool MercuryFileObjectNet::Check()
{
	return m_size > 0;
}

bool MercuryFileObjectNet::Eof()
{
	return m_location < m_size;
}



MercuryFile * MercuryFileDriverNet::GetFileHandle( const MString & sPath, FilePermission p  )
{
	if( sPath.substr( 0, 7 ) == "http://" )
	{
		MercuryFileObjectNet * fNew = new MercuryFileObjectNet;
		if( fNew->Init( sPath, p ) )
			return fNew;
		else
			return 0;
	}
	return 0;
}

void MercuryFileDriverNet::ListDirectory( const MString & sPath, MVector< MString > & output, bool bDirsOnly )
{
	//Do nothing, since we can't inquire as to folders on the internet.
}



/* 
 * Copyright (c) 2005-2006, Charles Lohr
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

