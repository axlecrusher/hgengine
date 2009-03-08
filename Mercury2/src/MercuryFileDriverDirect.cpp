#include "MercuryFileDriverDirect.h"

#if !defined(WIN32)
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#if defined(_EE)
#define FSHEADER "host:"
#else
#define FSHEADER ""
#endif

#include <string.h>

//Core base only.
MercuryFile::MercuryFile()
{
	//No code
}

MercuryFile::~MercuryFile()
{
	//No code
}

bool MercuryFile::Init( const MString &sPath, FilePermission p )
{
	m_sPath = sPath;
	m_p = p;
	return true;
}

bool MercuryFile::ReadLine( MString & data )
{
	data = "";
	char b1[1];
	bool Success;
	while ( Success = ( Read( b1, 1 ) > 0 ) )
	{
		if ( ( b1[0] == '\r' ) || ( b1[0] == '\n' ) )
			break;
		data += b1[0];
	}

	//We're using windows, check to make sure we don't have an extra \n.
	if ( b1[0] == '\r' )
	{
		unsigned long loc = Tell();
		Read( b1, 1 );
		if ( b1[0] != '\n' )
			Seek(loc);
	}

	return Success;
}

MercuryFileObjectDirect::MercuryFileObjectDirect( ):
	MercuryFile( )
{
	m_fF = NULL;
}

MercuryFileObjectDirect::~MercuryFileObjectDirect()
{
	if ( m_fF != NULL )
	{
		fclose ( m_fF );
		m_fF = NULL;
	}
}

void MercuryFileObjectDirect::Close()
{
	if ( m_fF != NULL )
	{
		fclose ( m_fF );
		m_fF = NULL;
	}
	delete this;
}

bool MercuryFileObjectDirect::Init( const MString & fName, FilePermission p )
{
	MercuryFile::Init( fName, p );
	if ( m_fF != NULL )
		SAFE_DELETE( m_fF );

	switch ( p )
	{
	case MFP_READ_ONLY:
		m_fF = fopen( (FSHEADER+fName).c_str(), "rb" );
		break;
	case MFP_WRITE_ONLY:
		m_fF = fopen( (FSHEADER+fName).c_str(), "wb" );
		break;
	case MFP_READ_AND_WRITE:
		m_fF = fopen( (FSHEADER+fName).c_str(), "wb+" );
		break;
	case MFP_APPEND:
		m_fF = fopen( (FSHEADER+fName).c_str(), "a" );
		break;
	default:
		m_fF = NULL;
		break;
	}

	return ( m_fF != NULL );
}

bool MercuryFileObjectDirect::Seek( unsigned long position )
{
	if ( m_fF == NULL )
		return false;
	return ( fseek( m_fF, position, SEEK_SET ) == 0 );
}

unsigned long MercuryFileObjectDirect::Tell()
{
	if ( m_fF == NULL )
		return false;
	return ftell( m_fF );
}

unsigned long MercuryFileObjectDirect::Length()
{
	if ( m_fF == NULL )
	  return false;
	unsigned long prev = ftell( m_fF );
	fseek( m_fF, 0, SEEK_END );
	unsigned long ret = ftell( m_fF );
	fseek( m_fF, prev, SEEK_SET );
	return ret;
}

bool MercuryFileObjectDirect::Write( void * data, unsigned long length )
{
	if ( m_fF == NULL )
		return false;
	return ( fwrite( data, length, 1, m_fF ) > 0 );
}

unsigned long MercuryFileObjectDirect::Read( void * data, unsigned long length )
{
	if ( m_fF == NULL )
		return false;
	return ( fread( data, 1, length, m_fF) );
}

bool MercuryFileObjectDirect::Check()
{
	if ( m_fF == NULL )
		return true;
	return (ferror( m_fF )!=0);
}

bool MercuryFileObjectDirect::Eof()
{
	if ( m_fF == NULL )
		return false;
	return (feof( m_fF )!=0);
}

unsigned long MercuryFileObjectDirect::GetModTime()
{
	if( !m_fF )
		return 0;
#if defined(WIN32)
	HANDLE hFile = CreateFile( m_sPath.c_str(), GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( !hFile )
		return 0;
	FILETIME Tcreate,Taccess,Twrite;
	if( !GetFileTime( hFile, &Tcreate, &Taccess, &Twrite ) )
	{
		CloseHandle( hFile );
		return 0;
	}
	else
	{
		CloseHandle( hFile );
		//Windows does time based on 100ns  intervals from Jan 1 1601 AD.  So, we have t
		return (Twrite.dwHighDateTime - 27636147) * 400 + (Twrite.dwLowDateTime/10000000);
	}
#else
	//Pot shot... does this even compile in linux?
	struct stat sb;
	if ( stat( m_sPath.c_str(), &sb ) )
		return 0;
	else
		return sb.st_mtime;
#endif
}

MercuryFileDirverDirect::MercuryFileDirverDirect()
{
#ifndef RUN_FROM_START_FOLDER

#if defined( WIN32 )
	char buffer[1024];
	char * path_end;
	int retcode = 0;
	if( ( retcode = GetModuleFileName( NULL, buffer, 1024 ) ) < 0 )
	{
		fprintf( stderr, "WARNING:  Could not change path of program to path of executable! Faild to get it with response: %d\n", retcode );
	}
	path_end = strrchr( buffer, '\\' );
	if( !path_end )
	{
		fprintf( stderr, "WARNING:  Could not change path of program to path of executable!  Path retreived: \"%s\".\n", buffer );
	}
	*path_end = 0;
	if( !SetCurrentDirectory( buffer ) )
	{
		fprintf( stderr, "WARNING:  Could not set operational folder.", buffer );
	}
#else
	char buffer[PATH_MAX];
	char * path_end;
	if( readlink( "/proc/self/exe", buffer, PATH_MAX ) <= 0 )
	{
		fprintf( stderr, "WARNING:  Could not change path of program to path of executable!\n" );
		return;
	}
	path_end = strrchr( buffer, '/' );
	if( !path_end )
	{
		fprintf( stderr, "WARNING:  Could not change path of program to path of executable!  Path retreived: %s.\n", buffer );
		return;
	}
	*path_end = 0;
	chdir( buffer );
#endif
#endif
}

MercuryFile * MercuryFileDirverDirect::GetFileHandle( const MString & sPath, FilePermission p )
{
	MercuryFile * ret = new MercuryFileObjectDirect;
	if ( ret->Init( sPath, p ) )
		return ret;

	SAFE_DELETE(ret);

	return NULL;
}

#if defined(WIN32)
#include <windows.h>
#endif

void MercuryFileDirverDirect::ListDirectory( const MString & sPath, MVector< MString > & output, bool bDirsOnly )
{
#if defined(WIN32)
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile( sPath, &fd );
	if( INVALID_HANDLE_VALUE == hFind )		// no files found
		return;
	do
	{
		if( bDirsOnly  &&  !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			continue;	// skip

		if( (!bDirsOnly)  &&  (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			continue;	// skip

		MString sDirName( fd.cFileName );

		if( sDirName == "."  ||  sDirName == ".." )
			continue;

		output.push_back( sDirName );
	} while( ::FindNextFile( hFind, &fd ) );
	::FindClose( hFind );
#elif !defined(_EE)
	DIR * pDe = opendir( "./" + sPath );
	if ( pDe == NULL )
		return;

	dirent * pEnt;

	while ( (pEnt = readdir( pDe )) )
	{
		if ( ( pEnt->d_type & DT_DIR ) && !bDirsOnly )
			continue;
		if ( !( pEnt->d_type & DT_DIR ) && bDirsOnly )
			continue;

		if ( strcmp( pEnt->d_name, "." ) == 0 )
			continue;

		if ( strcmp( pEnt->d_name, ".." ) == 0 )
			continue;

		output.push_back( pEnt->d_name );
	}

	closedir( pDe );
#endif
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
