#include "MercuryFile.h"
#include "MercuryVector.h"
#include "MercuryFileDriverDirect.h"
#include "MercuryFileDriverNet.h"
#include "MercuryFileDriverMem.h"
#include "MercuryFileDriverZipped.h"
#include "MercuryFileDriverPacked.h"
#include "MercuryTheme.h"
#include <MercuryStringUtil.h>

/********************FILE MANAGER**************************/

MercuryFileManager::MercuryFileManager()
	:m_Drivers(NULL)
{
	
}

MercuryFileManager::~MercuryFileManager()
{
	SAFE_DELETE(m_Drivers);
}

void MercuryFileManager::Init()
{
	m_Drivers = new MVector< MAutoPtr< MercuryFileDriver > >;

	//Careful!  The order here ACTUALLY MATTERS
	m_Drivers->push_back( new MercuryFileDirverDirect );

	MercuryFileDriver * Mem = new MercuryFileDriverMem;
	Mem->Init();
	m_Drivers->push_back( Mem );
	
	MercuryFileDriver * Packed = new MercuryFileDriverPacked;
	Packed->Init();
	m_Drivers->push_back( Packed );

	MercuryFileDriver * Zipped = new MercuryFileDriverZipped;
	Zipped->Init();
	m_Drivers->push_back( Zipped );

//Net doesn't work yet
//	MercuryFileDriver * Net = new MercuryFileDriverNet;
//	Net->Init();
//	m_Drivers->push_back( Net );
}

MercuryFile * MercuryFileManager::Open( const MString & sPath, FilePermission p )
{
	if( sPath.empty() )
		return NULL;

	//Tricky: We can support all caps colon (GRAPHIC:, FILE:, etc.)
	//So, instead of doing some checking in multiple passes, we are doing
	//a generalized system to handle getting it.
	{
		const char * c;
		for( c = sPath.c_str(); (*c <= 'Z' && *c >= 'A'); c++ );
		if( *c == ':' )
		{
			c++;
			MString ExtString = ToProper( MString( sPath.c_str(), (int)(c-sPath.c_str()-1) ) ) + "/" + c;
			const MVector< MercuryThemeManager::Theme > & th = THEME.GetThemes();
			for( int i = (int)th.size()-1; i >= 0; i-- )
			{
				MercuryFile * ret = FILEMAN.Open( ssprintf( "Themes/%s/%s", th[i].sTheme.c_str(),ExtString.c_str() ), p );
				if( ret )
					return ret;
			}
			return 0;
		}
	}

	MercuryFile * ret;
	for ( unsigned int i = 0; i < m_Drivers->size(); i++ )
	{
		ret = m_Drivers->at(i)->GetFileHandle( sPath, p );
		if ( ret != NULL )
			return ret;
	}
	return NULL;
}

void MercuryFileManager::ListDirectory( const MString & sPath, MVector< MString > & output, bool bDirsOnly )
{
	for ( unsigned int i = 0; i < m_Drivers->size(); i++ )
	{
		if ( sPath.substr(0,1).compare("/") )
			m_Drivers->at(i)->ListDirectory( sPath, output, bDirsOnly );
		else
			m_Drivers->at(i)->ListDirectory( sPath.substr(1), output, bDirsOnly );
	}
}

bool MercuryFileManager::FileMatch( const MString & file, const MString & regex )
{
	const char * rx = regex.c_str();
	unsigned j = 0;
	const char * fe = file.c_str();
	unsigned i;
	for ( i = 0; i < regex.length() && j < file.length(); i++ )
	{
		switch ( rx[i] )
		{
		case '*':
			{
				i++;
				if ( i == regex.length() )
					return true;

				while ( ( j < file.length() ) && 
					( fe[j] != rx[i] ) )
					j++;

				j++;
				if ( j == file.length() )
					return false;
			}
			break;
		case '?':
			j++;
			break;
		default:

			if ( fe[j] != rx[i] )
				return false;
			j++;
			break;
		}
	}
	if ( ( j >= file.length() ) && !( i >= regex.length() ) )
		return false;
	return true;
}

long FileToString( const MString & sFileName, char * & data )
{
	data = 0;

	MercuryFile * f = FILEMAN.Open( sFileName );
	if( !f ) return -1;

	int length = f->Length();

	data = (char*)malloc( length + 1 );

	if( !data )
	{
		data = 0;
		delete f;
		return -1;
	}

	int r = f->Read( data, length );

	delete f;

	if( r != length )
	{
		free( data );
		data = 0;
		return -1;
	}

	return length;
}

bool StringToFile( const MString & sFileName, const MString & data )
{
	MercuryFile * f = FILEMAN.Open( sFileName, MFP_WRITE_ONLY );
	if( !f ) return false;
	f->Write( data.c_str(), data.length() );
	delete f;
	return true;
}

MercuryFileManager& MercuryFileManager::GetInstance()
{
	static MercuryFileManager* instance = NULL;
	if (!instance)
	{
		instance = new MercuryFileManager;
		instance->Init();
	}
	return *instance;
}


/* 
 * Copyright (c) 2005-2009, Charles Lohr
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

