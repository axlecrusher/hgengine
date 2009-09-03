#include "ModelContainer.h"

//Utility functions, these should be written depending on the endian of the system.

char Read1( FILE * f )
{
	char iTmp;
	fread( &iTmp, 1, 1, f );
	return iTmp;
}

void Write1( FILE * f, char c )
{
	fwrite( &c, 1, 1, f );
}

int Read4( FILE * f )
{
	int iTmp;
	fread( &iTmp, 4, 1, f );
	return iTmp;
}

void Write4( FILE * f, int i )
{
	fwrite( &i, 4, 1, f );
}

string ReadString( FILE * f )
{
	int iTmp = Read4( f );
	char * sTmp = new char[iTmp+1];
	fread( sTmp, iTmp, 1, f );
	sTmp[iTmp] = '\0';
	string sRet = sTmp;
	delete sTmp;
	return sRet;
}

//Actual code

bool Mesh::Load( FILE * f )
{
	unsigned i;
	unsigned j;

	sName = ReadString( f );
	iBoneAttached = -1;

	vVerticies.resize( Read4( f ) );
	vStrips.resize( Read4( f ) );
	vFans.resize( Read4( f ) );
	vTriangles.resize( Read4( f ) );
	iMaterialNumber = Read4( f );
	bCache = Read1( f ) != '\0';

	for( i = 0; i < vVerticies.size(); i++ )
	{
		fread( &vVerticies[i], 8*4, 1, f );
		vVerticies[i].bInUse = true;
		vVerticies[i].bInUse2 = true;
	}

	for( i = 0; i < vStrips.size(); i++ )
	{
		vStrips[i].resize( Read4( f ) );
		for( j = 0; j < vStrips[i].size(); j++ )
			vStrips[i][j] = Read4( f );
	}

	for( i = 0; i < vFans.size(); i++ )
	{
		vFans[i].resize( Read4( f ) );
		for( j = 0; j < vFans[i].size(); j++ )
			vFans[i][j] = Read4( f );
	}

	for( i = 0; i < vTriangles.size(); i++ )
	{
		vTriangles[i].bInUse = true;
		for ( j = 0; j < 3; j++ )
			vTriangles[i].iFace[j] = Read4( f );
	}

	return !feof( f );
}

void Mesh::Save( FILE * f )
{
	unsigned i;
	unsigned j;

	Write4( f, sName.length() );
	fwrite( sName.c_str(), sName.length(), 1, f );
	Write4( f, vVerticies.size() );
	Write4( f, vStrips.size() );
	Write4( f, vFans.size() );
	Write4( f, vTriangles.size() );
	Write4( f, iMaterialNumber );
	Write1( f, bCache );

	for( i = 0; i < vVerticies.size(); i++ )
		fwrite( &vVerticies[i], 8*4, 1, f );

	for( i = 0; i < vStrips.size(); i++ )
		for ( j = 0; j < vStrips[i].size(); j++ )
			Write4( f, vStrips[i][j] );

	for( i = 0; i < vFans.size(); i++ )
		for ( j = 0; j < vFans[i].size(); j++ )
			Write4( f, vFans[i][j] );

	for( i = 0; i < vTriangles.size(); i++ )
		for ( j = 0; j < 3; j++ )
			Write4( f, vTriangles[i].iFace[j] );
}

bool Bone::Load( FILE * f )
{
	sName = ReadString( f );
	iParentIndex = Read4( f );
	iAssociatedMesh = Read4( f );
	fread( &pJointPos, 12, 1, f );
	fread( &qJointRot, 16, 1, f );
	vAssignments.resize( Read4( f ) );

	for( unsigned i = 0; i < vAssignments.size(); i++ )
	{
		vAssignments[i].iMesh = Read4( f );
		vAssignments[i].iVertex = Read4( f );
		fread( &vAssignments[i].fWeight, 4, 1, f );
	}

	return !feof( f );
}

void Bone::Save( FILE * f )
{
	Write4( f, sName.length() );
	fwrite( sName.c_str(), sName.length(), 1, f );
	Write4( f, iParentIndex );
	Write4( f, iAssociatedMesh );
	fwrite( &pJointPos, 12, 1, f );
	fwrite( &qJointRot, 16, 1, f );

	Write4( f, vAssignments.size() );
	for( unsigned i = 0; i < vAssignments.size(); i++ )
	{
		Write4( f, vAssignments[i].iMesh );
		Write4( f, vAssignments[i].iVertex );
		fwrite( &vAssignments[i].fWeight, 4, 1, f );
	}
}

bool AnimationTrack::Load( FILE * f )
{
	iBone = Read4( f );
	vKeys.resize( Read4( f ) );
	fread( &vKeys[0], 8*4*vKeys.size(), 1, f );
	return !feof( f );
}

void AnimationTrack::Save( FILE * f )
{
	Write4( f, iBone );
	Write4( f, vKeys.size() );
	fwrite( &vKeys[0], 8*4*vKeys.size(), 1, f );
}

bool Animation::Load( FILE * f )
{
	sName = ReadString( f );
	fread( &fDuration, 4, 1, f );
	vTracks.resize( Read4( f ) );
	for( unsigned i = 0; i < vTracks.size(); i++ )
		vTracks[i].Load( f );
	return !feof( f );
}

void Animation::Save( FILE * f )
{
	Write4( f, sName.length() );
	fwrite( sName.c_str(), sName.length(), 1, f );
	fwrite( &fDuration, 4, 1, f );
	Write4( f, vTracks.size() );
	for( unsigned i = 0; i < vTracks.size(); i++ )
		vTracks[i].Save( f );	
}

bool Model::LoadModel( const char * sFileName )
{
	FILE * f = fopen( sFileName, "rb" );
	if ( !f )
		return false;

	char sMercuryHeadder[4];
	int iVersion;
	unsigned i;

	fread( sMercuryHeadder, 4, 1, f );
	iVersion = Read4( f );

	vMeshes.resize( Read4( f ) );
	for( i = 0; i < vMeshes.size(); i++ )
		vMeshes[i].Load( f );

	vBones.resize( Read4( f ) );
	for( i = 0; i < vBones.size(); i++ )
		vBones[i].Load( f );

	//Compute bone xformations, and antixformations
	for( i = 0; i < vBones.size(); i++ )
	{
		MercuryMatrix m,n,o;

		vBones[i].qJointRot.toMatrix4( o );
		o[0][3] = vBones[i].pJointPos.x;
		o[1][3] = vBones[i].pJointPos.y;
		o[2][3] = vBones[i].pJointPos.z;

		if ( vBones[i].iParentIndex >= 0 )
		{
			R_ConcatTransforms4( (const float*)&vBones[vBones[i].iParentIndex].pXFormMatrix, (const float*)&o, (float*)&m );
			Copy16f( &vBones[i].pXFormMatrix, &m);
		}
		else
			Copy16f(&vBones[i].pXFormMatrix, &o);

		InvertMatrix( vBones[i].pXFormMatrix, vBones[i].pAntiXFormMatrix );
	}

	vAnimations.resize( Read4( f ) );
	for( i = 0; i < vAnimations.size(); i++ )
		vAnimations[i].Load( f );

	vMaterials.resize( Read4( f ) );
	for( i = 0; i < vMaterials.size(); i++ )
		vMaterials[i] = ReadString( f );

	fclose(f);
	return true;
}

void Model::SaveModel( const char * sFileName )
{
	unsigned i;
	FILE * f = fopen( sFileName, "wb" );
	if ( !f )
		return;
	const char * sMercuryKey = "MBMF";
	fwrite( sMercuryKey, 4, 1, f );
	Write4( f, 1 );

	Write4( f, vMeshes.size() );
	for( i = 0; i < vMeshes.size(); i++ )
		vMeshes[i].Save( f );

	Write4( f, vBones.size() );
	for( i = 0; i < vBones.size(); i++ )
		vBones[i].Save( f );

	Write4( f, vAnimations.size() );
	for( i = 0; i < vAnimations.size(); i++ )
		vAnimations[i].Save( f );

	Write4( f, vMaterials.size() );
	for( i = 0; i < vMaterials.size(); i++ )
	{
		Write4( f, vMaterials[i].length() );
		fwrite( vMaterials[i].c_str(), vMaterials[i].length(), 1, f );
	}

	fclose(f);
}

/*
 * (c) 2006 Charles Lohr
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
