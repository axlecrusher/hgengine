#include <MercuryFileDriverZipped.h>
#include <MercuryFileDriverPacked.h>
#include <MercuryVector.h>

//For the store compression on zips
#include <zlib.h>
#include <string.h>

#if defined(WIN32)
#  if defined(_MSC_VER)
#    pragma comment(lib, "zdll.lib")
#  endif
#endif

const char PackagePrefix[] = "Packages/";


/********************FILE DRIVER ZIPPED*******************/

MercuryFileObjectZipped::~MercuryFileObjectZipped()
{
	Close();
}

bool MercuryFileObjectZipped::Init( const MString & fName, 
	FilePermission p, const MString & base, unsigned offset, 
	unsigned size, unsigned packedsize, unsigned decompID )
{
	if ( p == MFP_APPEND )
		return false;			//Not supported.

	//NOTE: While we don't actually support a file opened for writing,
	//		we shouldn't fail if write permissions are requested.

	m_sPath = fName;
	m_p = p;
	MercuryFile * m_base = FILEMAN.Open( base, p );

	//Seek to the beginning of the actual data
	m_base->Seek( offset );

	char * inbuff = (char*)malloc( packedsize + 258 );
	m_Buffer = (char*)malloc( size );

	m_base->Read( inbuff + 2, packedsize + 2 );
	m_base->Close();

	z_stream_s stream;
	memset( &stream, 0, sizeof(stream) );
	stream.avail_out = size;
	stream.total_out = 0;
	stream.next_out = (unsigned char*)m_Buffer;
	stream.total_in = packedsize+2;
	stream.avail_in = packedsize+2;
	stream.next_in = (unsigned char*)inbuff;

	//Compression type 8 (Always) (These really are magic numbers here)
	inbuff[0] = 0x78;
	inbuff[1] = 0x01;

	int err = inflateInit(&stream);

	if ( err )
	{
		inflateEnd(&stream);
		free( inbuff );
		free( m_Buffer );
		m_Buffer = 0;
		return false;
	}

	err = inflate( &stream, Z_NO_FLUSH );

	if ( err < 0 )
	{
		inflateEnd(&stream);
		free( inbuff );
		free( m_Buffer );
		m_Buffer = 0;
		return false;
	}
	
	inflateEnd(&stream);

	free( inbuff );

	m_size = size;
	m_location = 0;
	m_bIsDummy = false;

	return true;
}

bool MercuryFileObjectZipped::MemFileInit( const MString & sName, const char * pData, unsigned long iSize )
{
	m_bIsDummy = true;
	//Yuck, but we're read only anyways! :)
	m_Buffer = (char*)pData;
	m_size = iSize;
	m_sPath = sName;
	m_location = 0;
	m_p = MFP_READ_ONLY;
	return true;
}

bool MercuryFileObjectZipped::Seek( unsigned long position )
{
	if ( position >= m_size )
		return false;
	m_location = position;
	return true;
}

unsigned long MercuryFileObjectZipped::Tell()
{
	return m_location;
}

unsigned long MercuryFileObjectZipped::Length()
{
	return m_size;
}

bool MercuryFileObjectZipped::Write( const void * data, unsigned long length )
{
	//You cannot write to a zipped file in the store form.
	return false;
}

unsigned long MercuryFileObjectZipped::Read( void * data, unsigned long length )
{
	int readable = m_size - m_location;
	int ret;
	if ( (unsigned)readable < length )
		ret = readable;
	else 
		ret = length;
	memcpy( data, m_Buffer + m_location, ret );
	m_location += ret;
	return ret;
}

bool MercuryFileObjectZipped::Check()
{
	if ( m_Buffer )
		return 0;
	else
		return 1;
}

bool MercuryFileObjectZipped::Eof()
{
	return m_location >= m_size;
}

void MercuryFileObjectZipped::Close()
{
	if ( !m_bIsDummy )
		SAFE_DELETE_ARRAY( m_Buffer );
}

inline unsigned long ZIPToLong(char * inx)
{
	unsigned char *in = (unsigned char*)inx;
	return in[0] + long(in[1])*256 + long(in[2])*65536 + long(in[3])*16777216;
}

void MercuryFileDriverZipped::Init()
{
	unsigned i;
	MVector< MString > out;

	MercuryFileDriver::Init();

	FILEMAN.ListDirectory( MString(PackagePrefix)+"*.zip", out, false );
	for ( i = 0; i < out.size(); i++ )
		out[i] = PackagePrefix + out[i];

	FILEMAN.ListDirectory( "*.zip", out, false );
	for ( i = 0; i < out.size(); i++ )
	{
		MercuryFile * z = FILEMAN.Open( out[i] );
		
//		unsigned long NumberOfFiles = 0;
		char tmp[512];

		while ( !z->Check() && !z->Eof() )
		{
			MString basename;
			MString path;
			MString name;

			unsigned long filesize;
			unsigned long namelen;
			unsigned long packed;
			unsigned long PK;
			unsigned long PK2;

			z->Read( tmp, 2 );
			if ( tmp[0] != 'P' || tmp[1] != 'K' )
				break;

			z->Read( tmp, 4 );
			PK = ZIPToLong( tmp );

			//If we are not reading a normal entry, we don't know how to continue
			if ( ( PK % 0x10000 ) != 0x0403 )
				break;

			z->Read( tmp, 4 );
			PK2 = ZIPToLong( tmp );

			z->Read( tmp, 4 ); //Time (Throw out)
			z->Read( tmp, 4 ); //CRC32 (Throw out)

			z->Read( tmp, 4 );
			packed = ZIPToLong( tmp );

			z->Read( tmp, 4 );
			filesize = ZIPToLong( tmp );

			z->Read( tmp, 4 );
			namelen = ZIPToLong( tmp );

			z->Read( tmp, namelen );
			tmp[namelen] = '\0';
			name = tmp;

			ZipFileEntry e;

			if ( name.length() > 0 )
				if ( name.c_str()[name.length()-1] == '/' )
					continue;	//Ignore folder entries

			//First, strip out folders.
			MString LastFolder;
			MString CurrentElement;
			for ( int k = 0; (unsigned)k < namelen; k++ )
			{
				if( tmp[k] == '/' )
				{
					ZipFileEntry y;
					y.m_bFolder = true;
					//Other values don't apply.
					m_mFileFolders[LastFolder][CurrentElement] = y;
					LastFolder += CurrentElement + "/";
					CurrentElement = "";
				}
				else
					CurrentElement+=tmp[k];
			}
			ZipFileEntry y;
			y.m_bFolder = false;
			y.m_sPackageName = out[i];
			y.m_iOffset = z->Tell();
			y.m_iSize = filesize;
			y.m_iPackedSize = packed;
			y.m_pkID = PK;
			y.m_pkID2 = PK2;
			m_mFileFolders[LastFolder][CurrentElement] = y;

			z->Seek( z->Tell() + packed );
		}

		SAFE_DELETE( z );
	}
}

MercuryFile * MercuryFileDriverZipped::GetFileHandle( const MString & sPath, FilePermission p )
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
	if (i > 0)
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

	ZipFileEntry z = m_mFileFolders[Path][ActualName];

	if ( z.m_bFolder )
		return NULL;

	bool success;

	MercuryFile * ret = NULL;

	//Zero indicates we're not actually doing compression
	if ( z.m_pkID2 / 65536 == 0 )
	{
		MercuryFileObjectPacked * tmp = new MercuryFileObjectPacked();
		success = tmp->Init( ActualName, p, z.m_sPackageName, z.m_iOffset, z.m_iSize );
		ret = tmp;
	}
	else
	{
		MercuryFileObjectZipped * tmp = new MercuryFileObjectZipped();
		success = tmp->Init( ActualName, p, z.m_sPackageName, z.m_iOffset, z.m_iSize, z.m_iPackedSize, z.m_pkID2 );
		ret = tmp;
	}

	if ( !success )
	{
		SAFE_DELETE( ret );
		return NULL;
	}
	return ret;
}

void MercuryFileDriverZipped::ListDirectory( const MString & sPath, MVector< MString > & output, bool bDirsOnly )
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

	std::map< MString, ZipFileEntry > * cDir = &m_mFileFolders[Path];

	std::map< MString, ZipFileEntry >::iterator iter, end;
	end = cDir->end();
	for ( iter = cDir->begin(); iter != end; iter++ )
	{
		MString fName = iter->first;
		ZipFileEntry z = iter->second;
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

