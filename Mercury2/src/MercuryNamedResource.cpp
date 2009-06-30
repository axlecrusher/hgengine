#include <MercuryNamedResource.h>
#include <MercuryUtil.h>


MString MercuryNamedResource::GetValueS( const MString & sDataPointer )
{
	MString ret;
	GetValue( sDataPointer, ret );
	return ret;
}

MString MercuryNamedResource::GetValueS( const MString & sDataPointer, MString sDefaultValue, bool bSetValue )
{
	MString ret;
	if( GetValue( sDataPointer, ret ) )
		return ret;

	if( bSetValue )
		SetValueS( sDataPointer, sDefaultValue );
	return sDefaultValue;
}


float MercuryNamedResource::GetValueF( const MString & sDataPointer, float fDefaultValue, bool bSetValue )
{
	MString tmpret;
	if( GetValue( sDataPointer, tmpret ) )
		return StrToFloat( tmpret );
	if( bSetValue )
		SetValueF( sDataPointer, fDefaultValue );

	return fDefaultValue;
}

void  MercuryNamedResource::SetValueF( const MString & sDataPointer, float fValue )
{
	char sset[64];
	snprintf( sset, 63, "%f", fValue );
	SetValueS( sDataPointer, sset );
}

bool MercuryNamedResource::GetValueB( const MString & sDataPointer, bool bDefaultValue, bool bSetValue )
{
	MString tmpret;
	if( GetValue( sDataPointer, tmpret ) )
	{
		tmpret = ToUpper( tmpret );
		if( tmpret.compare( "TRUE" ) == 0 )	return 1;
		if( tmpret.compare( "FALSE" ) == 0 )	return 0;
		if( tmpret.compare( "ON" ) == 0 )	return 1;
		if( tmpret.compare( "OFF" ) == 0 )	return 0;
		if( tmpret.compare( "YES" ) == 0 )	return 1;
		if( tmpret.compare( "NO" ) == 0 )	return 0;

		return StrToInt( tmpret );
	}

	if( bSetValue )
		SetValueB( sDataPointer, bDefaultValue );

	return bDefaultValue;
}

void MercuryNamedResource::SetValueB( const MString & sDataPointer, bool bValue )
{
	if( bValue )
		SetValueS( sDataPointer, "1" );
	else
		SetValueS( sDataPointer, "0" );
}

int  MercuryNamedResource::GetValueI( const MString & sDataPointer, int iDefaultValue, bool bSetValue )
{
	MString tmpret;
	if( GetValue( sDataPointer, tmpret ) )
		return StrToInt( tmpret );
	if( bSetValue )
		SetValueI( sDataPointer, iDefaultValue );

	return iDefaultValue;
}

void MercuryNamedResource::SetValueI( const MString & sDataPointer, int iValue )
{
	char sset[64];
	snprintf( sset, 63, "%d", iValue );
	SetValueS( sDataPointer, sset );
}


/****************************************************************************
 *   Copyright (C) 2009 by Charles Lohr                                     *
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
