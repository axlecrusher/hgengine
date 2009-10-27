#include <MercuryFileDriverPacked.h>
#include <MercuryVector.h>
#include <string.h>

const MString PackagePrefix = "Packages/";

MercuryFileObjectPacked::~MercuryFileObjectPacked()
{
	Close();
}

bool MercuryFileObjectPacked::Init( const MString & fName, 
	FilePermission p, const MString & base, unsigned offset, 
	unsigned size )
{
	MercuryFile::Init(fName, p);

	if ( p == MFP_APPEND )
		return false;			//Not supported.

//	m_sPath = fName;
//	m_p = p;
	m_base = FILEMAN.Open( base, p );
	if ( m_base->Check() )
		return false;
	m_offset = offset;
	m_size = size;
	m_location = 0;

	return m_base->Seek( m_offset );
}

bool MercuryFileObjectPacked::Seek( unsigned long position )
{
	if ( position >= m_size )
		return false;
	m_location = position;
	return m_base->Seek( m_location + m_offset );
}

unsigned long MercuryFileObjectPacked::Tell()
{
	return m_location;
}

unsigned long MercuryFileObjectPacked::Length()
{
	return m_size;
}

bool MercuryFileObjectPacked::Write( const void * data, unsigned long length )
{
	//First make sure we won't over-write good data.
	if ( length >= ( m_size - m_location ) )
		return false;

	bool ret = m_base->Write( data, length );
	if ( ret )
		m_location += length;
	return ret;
}

unsigned long MercuryFileObjectPacked::Read( void * data, unsigned long length )
{
	int readable = m_size - m_location;
	if ( (unsigned)readable < length )
	{
		int ret = m_base->Read( data, readable );
		m_location += ret;
		return ret;
	}
	else 
	{
		int ret = m_base->Read( data, length );
		m_location += ret;
		return ret;
	}
}

bool MercuryFileObjectPacked::Check()
{
	return ( m_base != 0 );
}

bool MercuryFileObjectPacked::Eof()
{
	return m_location >= m_size;
}

void MercuryFileObjectPacked::Close()
{
	SAFE_DELETE( m_base );
}

inline unsigned long ToLong(char * inx)
{
	unsigned char *in = (unsigned char*)inx;
	return in[0] + long(in[1])*256 + long(in[2])*65536 + long(in[3])*16777216;
}

void MercuryFileDriverPacked::Init()
{
	MVector< MString > out;
	MercuryFileDriver::Init();

	FILEMAN.ListDirectory( PackagePrefix+"*.*", out, false );

	for ( unsigned i = 0; i < out.size(); i++ )
	{
		MercuryFile * z = FILEMAN.Open( PackagePrefix+out[i] );
		
		if ( !z )
			continue;

		unsigned long NumberOfFiles = 0;
		char tmp[256];

		if ( !z->Eof() && !z->Check() )
		{
			z->Read( tmp, 4 );
			if ( strncmp( tmp, "HGPK", 4 ) != 0 )
				continue;
			z->Read( tmp, 128 );
			z->Read( tmp, 4 );
			NumberOfFiles = ToLong( tmp );
		}

		for ( unsigned j = 0; j < NumberOfFiles; j++ )
		{
			MString basename;
			MString path;
			MString name;
			unsigned long filesize;
			unsigned long namelen;
			unsigned long offset;
			if ( z->Eof() || z->Check() )
				break;
			z->Read( tmp, 4 );
			filesize = ToLong( tmp );
			z->Read( tmp, 4 );
			offset = ToLong( tmp );
			z->Read( tmp, 4 );
			namelen = ToLong( tmp );
			z->Read( tmp, namelen );
			tmp[namelen] = '\0';
			name = tmp;
			//First, strip out folders.
			MString LastFolder;
			MString CurrentElement;
			for ( int k = 0; (unsigned)k < namelen; k++ )
			{
				if( tmp[k] == '/' )
				{
					PckFileEntry y;
					y.m_bFolder = true;
					//Other values don't apply.
					m_mFileFolders[LastFolder][CurrentElement] = y;
					LastFolder += CurrentElement + "/";
					CurrentElement = "";
				}
				else
					CurrentElement+=tmp[k];
			}
			PckFileEntry y;
			y.m_bFolder = false;
			y.m_sPackageName = PackagePrefix+out[i];
			y.m_iOffset = offset;
			y.m_iSize = filesize;
			m_mFileFolders[LastFolder][CurrentElement] = y;
		}

		z->Close();
		SAFE_DELETE( z );
	}
}

MercuryFile * MercuryFileDriverPacked::GetFileHandle( const MString & sPath, FilePermission p )
{
	if ( sPath.length() == 0 )
		return NULL;

	//First break up 
	const char * sp = sPath.c_str();
	unsigned int i;
	for ( i = sPath.length()-1; i > 0; i-- )
		if ( sp[i] == '/' )
			break;

	MString ActualName;
	if (i >0)
	    ActualName = sPath.substr(i+1);
	else
	    ActualName = sPath;

	MString Path;
	if (i>0)
	  Path = sPath.substr(0,i+1);
	else
	  Path = "";

	if ( m_mFileFolders.find( Path ) == m_mFileFolders.end() )
		return NULL;

	if ( m_mFileFolders[Path].find( ActualName ) == m_mFileFolders[Path].end() )
		return NULL;

	PckFileEntry z = m_mFileFolders[Path][ActualName];

	if ( z.m_bFolder )
		return NULL;

	MercuryFileObjectPacked * ret = new MercuryFileObjectPacked();

	bool success = ret->Init( ActualName, p, z.m_sPackageName, z.m_iOffset, z.m_iSize );
	if ( !success )
	{
		SAFE_DELETE( ret );
		return NULL;
	}
	return ret;
}

void MercuryFileDriverPacked::ListDirectory( const MString & sPath, MVector< MString > & output, bool bDirsOnly )
{
	const char * sp = sPath.c_str();
	unsigned i;
	for (i = sPath.length()-1; i > 0; i-- )
		if ( sp[i] == '/' )
			break;

	MString ActualName;
	if (i>0)
	    ActualName = sPath.substr( i+1 );
	else
	    ActualName = sPath;

	MString Path;
	if(i>0)
	    Path = sPath.substr( 0, i+1 );

	if ( m_mFileFolders.find( Path ) == m_mFileFolders.end() )
		return;

	std::map< MString, PckFileEntry > * cDir = &m_mFileFolders[Path];

	std::map< MString, PckFileEntry >::iterator iter, end;
	end = cDir->end();
	for ( iter = cDir->begin(); iter != end; iter++ )
	{
		MString fName = iter->first;
		PckFileEntry z = iter->second;
		if ( bDirsOnly && !z.m_bFolder )
			continue;
		if ( !bDirsOnly && z.m_bFolder )
			continue;

		if ( MercuryFileManager::FileMatch( fName, ActualName ) )
			output.push_back( fName );
	}
}


/* 
 * Copyright (c) 2005-2008, Charles Lohr
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

