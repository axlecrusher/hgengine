#ifndef _MODEL_CONTAINER_H
#define _MODEL_CONTAINER_H

#include "MercuryTypes.h"
#include <vector>
#include <string>

using namespace std;

//****MESH****//

struct Vert
{
	bool operator < ( const Vert & rhs ) const
	{
		if( pPosition.z != rhs.pPosition.z )
			return pPosition.z < rhs.pPosition.z;
		if( pPosition.y != rhs.pPosition.y )
			return pPosition.y < rhs.pPosition.y;
		if( pPosition.x != rhs.pPosition.x )
			return pPosition.x < rhs.pPosition.x;
		if( uv[0] != rhs.uv[0] )
			return uv[0] < uv[0];
		return uv[1] < uv[1];
	}

	float uv[2];
	MercuryPoint pNormal;
	MercuryPoint pPosition;
	vector< int > vUsedBones;	//IGNORED BY SAVE AND NOT LOADED.
	bool bInUse;				//IGNORED BY SAVE (Load sets this to true.)
	bool bInUse2;				//IGNORED BY SAVE (Load sets this to true.).
};

struct Triangle
{
	int iFace[3];
	bool bInUse;		//IGNORED BY SAVE (Load sets this to true.)
};

struct Mesh
{
	bool Load( FILE * f );
	void Save( FILE * f );

	string sName;
	vector< Vert > vVerticies;
	vector< vector < int > > vStrips;
	vector< vector < int > > vFans;
	vector< Triangle > vTriangles;
	int iMaterialNumber;
	bool bCache;

	int iBoneAttached;	//IGNORED BY SAVE
};

//****BONE****//

struct BoneAssignment
{
	int iMesh;
	int iVertex;
	float fWeight;
};

struct Bone
{
	bool Load( FILE * f );
	void Save( FILE * f );

	string sName;
	int iParentIndex;
	int iAssociatedMesh;

	MercuryPoint pJointPos;;
	MQuaternion  qJointRot;
	vector< BoneAssignment > vAssignments;

	MercuryMatrix pXFormMatrix;		//Performs that bone's xformation IGNORED BY SAVE
	MercuryMatrix pAntiXFormMatrix;	//Inverse of XFormMatrix   IGNORED BY SAVE
};

//****ANIMATION****//

struct AnimationKey
{
	float fTime;
	MercuryPoint pPos;
	MQuaternion	 qRot;
};

struct AnimationTrack
{
	bool Load( FILE * f );
	void Save( FILE * f );

	int iBone;
	vector< AnimationKey > vKeys;
};

struct Animation
{
	bool Load( FILE * f );
	void Save( FILE * f );

	string sName;
	float fDuration;
	vector< AnimationTrack > vTracks;
};


class Model
{
public:
	bool LoadModel( const char * sFileName );
	void SaveModel( const char * sFileName );

	vector< Mesh > vMeshes;
	vector< Bone > vBones;
	vector< Animation > vAnimations;
	vector< string > vMaterials;

	void Clear() { vMeshes.clear(); vBones.clear(); vAnimations.clear(); vMaterials.clear(); }
};

#endif

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
