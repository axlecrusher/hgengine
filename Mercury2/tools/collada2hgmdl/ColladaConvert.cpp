#include <stdio.h>
#include <stdlib.h>
#include "XMLCog.h"
#include "ModelContainer.h"

Model mout;

int FindBone( vector< Bone > & bl, const string & sName )
{
	for( unsigned i = 0; i < bl.size(); i++ )
		if( bl[i].sName == sName )
			return i;

	fprintf( stderr, "Warning: Could not find bone with name \"%s\".", sName.c_str() );
	return -1;
}

//XXX: TODO: Actually use this function, since it's good and can reduce code.
void IntParse( const string & sData, vector< int > & is )
{
	const char * c = sData.c_str();
	string stmp = "";
	for( unsigned i = 0; i <= sData.length(); i++ )
	{
		if( c[i] == ' ' || i == sData.length() )
		{
			is.push_back( atoi( stmp.c_str() ) );
			stmp = "";
		}
			stmp += c[i];
	}
}

void FloatParse( const string & sData, vector< float > & is )
{
	const char * c = sData.c_str();
	string stmp = "";
	for( unsigned i = 0; i <= sData.length(); i++ )
	{
		if( c[i] == ' '  || i == sData.length() )
		{
			is.push_back( atof( stmp.c_str() ) );
			stmp = "";
		}
			stmp += c[i];
	}
}

void SetupBone( XMLCog * tp, XMLCog * tc, vector< Bone > & bl )
{
	int oParent = -1;
	int oThis = -1;

	if( tp )
	{
		oParent = FindBone( bl, tp->leaves["name"] );
	}
	
	oThis = FindBone( bl, tc->leaves["name"] );

	if( oThis == -1 )
	{
		fprintf( stderr, "Could not find bone \"%s\".", tc->leaves["name"].c_str() );
		return;
	}

	//Setup "this" bone
	bl[oThis].iParentIndex = oParent;
	bl[oThis].iAssociatedMesh = -1;


	//Setup all children bones.
	for( unsigned i = 0; i < tc->children.size(); i++ )
	{
		if( tc->children[i].data == "node" &&  tc->children[i].leaves["type"] == "JOINT" )
			SetupBone( tc, &tc->children[i], bl );
		else if( tc->children[i].data == "matrix" )
		{
			string sMatrix = tc->children[i].payload;
			vector< float > fd;
			FloatParse( sMatrix, fd );
			bl[oThis].pJointPos.x = fd[3];
			bl[oThis].pJointPos.y = fd[7];
			bl[oThis].pJointPos.z = fd[11];

			bl[oThis].qJointRot.w = sqrtf( 1.0 + fd[0] + fd[5] + fd[10] );
			float w4 = ( 4.0 * bl[oThis].qJointRot.w );
			bl[oThis].qJointRot.x = (fd[6] - fd[9]) / w4;
			bl[oThis].qJointRot.y = (fd[8] - fd[2]) / w4;
			bl[oThis].qJointRot.z = (fd[1] - fd[4]) / w4;
		}
	}
}

int main( int argc, char ** argv )
{
	float maxx = -10000, maxy = -10000, maxz = -10000;
	float minx =  10000, miny =  10000, minz =  10000;

	if( argc != 3 )
	{
		fprintf( stderr, "Mercury Game Engine Collada Converter\n" );
		fprintf( stderr, "  (C) 2009 Charles Lohr under the NewBSD License\n" );
		fprintf( stderr, "  Invalid usage. Usage: %s (Collada .dae File) (Mercury .hgmdl File)\n" );
		return -1;
	}
	XMLCog c;
	if( !LoadXML( argv[1], c ) )
	{
		fprintf( stderr, "Could not load/parse Collada file.\n" );
		return -2;
	}

	if( !c.leaves["version"].length() )
	{
		fprintf( stderr, "Could not find version of Collada.\n" );
		return -3;
	}

	printf( "Collada file, version %s opened as expected.\n", c.leaves["version"].c_str() );

	int lcontrol = c.FindChild( "library_controllers" );
	int lgeometries = c.FindChild( "library_geometries" );
	int lvscenes = c.FindChild( "library_visual_scenes" );
	int lvanimations = c.FindChild( "library_animations" );

	if( lgeometries == -1 )
	{
		fprintf( stderr, "Could not find library_geometries section.\n" );
		return -3;
	}

	int geometriesgeometry = c.children[lgeometries].FindChild( "geometry" );
	if( geometriesgeometry == -1 )
	{
		fprintf( stderr, "Could not find any geometry in the library_geometries section.\n" );
		return -3;
	}

	int geomesh = c.children[lgeometries].children[geometriesgeometry].FindChild( "mesh" );
	if( geomesh == -1 )
	{
		fprintf( stderr, "Could not find any mesh in the geometry section in the library_geometries section.\n" );
		return -3;
	}

	XMLCog & geometry = c.children[lgeometries].children[geometriesgeometry].children[geomesh];

	map< string, vector< float > > sourcevalues;
	map< string, map< string, string > > synonyms;

	for( unsigned i = 0; i < geometry.children.size(); i++ )
	{
		XMLCog & geoitem = geometry.children[i];
		if( geoitem.data == "source" )
		{
			string name = geoitem.leaves["name"];
			int floatvalspos = geoitem.FindChild( "float_array" );
			if( floatvalspos == -1 )
			{
				fprintf( stderr, "Source: %s does not have a float_array for values.  Cannot operate.\n", name.c_str() );
				return -3;
			}

			FloatParse( geoitem.children[floatvalspos].payload, sourcevalues[name] );
			printf( "%d values (%d verts) read for %s\n", sourcevalues[name].size(), sourcevalues[name].size()/3, name.c_str() );
		}
		else if( geoitem.data == "vertices" )
		{
			string sSynonym = geoitem.leaves["name"];
			//Information about how to map things toegether.
			for( unsigned i = 0; i < geoitem.children.size(); i++ )
			{
				XMLCog & idata = geoitem.children[i];
				if( idata.data == "input" )
					synonyms[sSynonym][idata.leaves["semantic"]] = idata.leaves["source"].substr(1);
			}
		}
		else if( geoitem.data == "triangles" )
		{
			string matname = geoitem.leaves["material"];
			//Find all the sources.
			vector< string > vSources;
			vector< string > vSemantics;

			for( unsigned i = 0; i < geoitem.children.size(); i++ )
			{
				XMLCog & idata = geoitem.children[i];
				if( idata.data == "input" )
				{
					int offset = atoi( idata.leaves["offset"].c_str() );
					if( vSources.size() <= offset )
					{
						vSources.resize( offset+1 );
						vSemantics.resize( offset+1 );
					}
					vSources[offset] = idata.leaves["source"].substr( 1 );
					vSemantics[offset] = idata.leaves["semantic"];
				}
			}

			//Extract actual indices
			int p = geoitem.FindChild( "p" );
			if( p == -1 )
			{
				fprintf( stderr, "Could not find list of indices for mesh with material \"%s\".", matname.c_str() );
				exit( -3 );
			}

			vector< vector< int > > vvLists;
			vvLists.resize( vSources.size() );
			int noelem = 3 * atoi( geoitem.leaves["count"].c_str() );

			for( unsigned i = 0; i < vSources.size(); i++ )
				vvLists[i].resize( noelem );

			string sList = geoitem.children[p].payload;
			string stmp = "";
			int elementno = 0;

			int IDForVertex = -1;
			int IDForTexcoord = -1;
			string sNameForVertex = "";
			string sNameOfTexCoord = "";
			for( unsigned i = 0; i < vSemantics.size(); i++ )
			{
				if( vSemantics[i] == "VERTEX" )
				{
					IDForVertex = i;
					sNameForVertex = vSources[i];
				}
				if( vSemantics[i] == "TEXCOORD" )
				{
					IDForTexcoord = i;
					sNameOfTexCoord = vSources[i];
				}
			}

			if( IDForVertex == -1 )
			{
				fprintf( stderr, "Could not find VERTEX semantic in object with material \"%s\".\n", matname.c_str() );
				exit( -3 );
			}

			for( unsigned i = 0; i <= sList.length(); i++ )
			{
				if( sList.c_str()[i] == ' ' || i == sList.length() )
				{
					if( elementno/vvLists.size() > noelem )
					{
						fprintf( stderr, "Error.  You have too many elements in the array for object with material \"%s\".  Expected %d, Got More.\n", matname.c_str(), noelem );
						exit( -3 );
					}
					vvLists[elementno%vvLists.size()][elementno/vvLists.size()] = atoi( stmp.c_str() );
					stmp = "";
					elementno++;
				}
				else
				{
					stmp += sList.c_str()[i];
				}
			}

			string sNameForPositions = "";
			string sNameForNormals = "";

			vector< float > * vpForPos = 0;
			vector< float > * vpForNrm = 0;
			vector< float > * vpForTex = 0;

			//Find the vertices.
			if( sourcevalues.find( sNameForVertex ) == sourcevalues.end() )
			{
				if( synonyms.find( sNameForVertex ) == synonyms.end() )
				{
					fprintf( stderr, "Error.  Could not find element \"%s\".\n", sNameForVertex.c_str() );
					exit( -3 );
				}

				sNameForPositions = synonyms[sNameForVertex]["POSITION"];
				sNameForNormals = synonyms[sNameForVertex]["NORMAL"];
			}
			else
				sNameForPositions = sNameForVertex;

			if( sNameForPositions == "" )
			{
				fprintf( stderr, "Error.  Could not find positions for object with material \"%s\".\n", matname.c_str() );
				exit( -3 );
			} else
			{
				vpForPos = &sourcevalues[sNameForPositions];
				for( unsigned i = 0; i < vpForPos->size()/3; i++ )
				{
					float x = (*vpForPos)[i*3+0];
					float y = (*vpForPos)[i*3+1];
					float z = (*vpForPos)[i*3+2];

					if( x < minx ) minx = x;
					if( y < miny ) miny = y;
					if( z < minz ) minz = z;

					if( x > maxx ) maxx = x;
					if( y > maxy ) maxy = y;
					if( z > maxz ) maxz = z;
				}
			}

			if( !vpForPos )
			{
				fprintf( stderr, "Error.  Could not find positions for object with material \"%s\". (Internal error version)\n", matname.c_str() );
				exit( -3 );
			}

			if( sNameForNormals == "" )
				vpForNrm = new vector< float >( vpForPos->size(), 0.0 );
			else
				vpForNrm = &sourcevalues[sNameForNormals];

			if( !vpForNrm )
			{
				fprintf( stderr, "Error.  Could not find normals for object with material \"%s\". (Internal error version)\n", matname.c_str() );
				exit( -3 );
			}

			if( sNameOfTexCoord == "" )
				vpForTex = new vector< float >( vpForPos->size() / 3 * 2, 0.0 );
			else
			{
				vector< float > *origTex = &sourcevalues[sNameOfTexCoord];
				if( !origTex )
				{
					fprintf( stderr, "Errror.  Could not find normals for object with material \"%s\". (Internal error version)\n", matname.c_str() );
					exit( -3 );
				}

				vpForTex = new vector< float >( vpForPos->size() / 3 * 2 );
				//I want to, for every index, copy the data...
				for( unsigned i = 0; i < vvLists[IDForTexcoord].size(); i++ )
				{
					(*vpForTex)[vvLists[IDForVertex][i]*2+0] = (*origTex)[vvLists[IDForTexcoord][i]*2+0];
					(*vpForTex)[vvLists[IDForVertex][i]*2+0] = (*origTex)[vvLists[IDForTexcoord][i]*2+1];
				}
			}

			printf( "Adding new mesh: %d\n", mout.vMeshes.size() + 1 );
			mout.vMeshes.resize( mout.vMeshes.size() + 1 );
			Mesh & tm = mout.vMeshes[mout.vMeshes.size() - 1];
			tm.sName = matname;
			tm.iMaterialNumber = mout.vMeshes.size() - 1;
			tm.vVerticies.resize( vpForPos->size() / 3 );
			tm.vTriangles.resize( vvLists[0].size() / 3 );

			for( unsigned i = 0; i < tm.vVerticies.size(); i++ )
			{
				tm.vVerticies[i].pPosition.x = (*vpForPos)[i*3+0];
				tm.vVerticies[i].pPosition.y = (*vpForPos)[i*3+1];
				tm.vVerticies[i].pPosition.z = (*vpForPos)[i*3+2];

				tm.vVerticies[i].pNormal.x = (*vpForNrm)[i*3+0];
				tm.vVerticies[i].pNormal.y = (*vpForNrm)[i*3+1];
				tm.vVerticies[i].pNormal.z = (*vpForNrm)[i*3+2];

				tm.vVerticies[i].uv[0] = (*vpForTex)[i*2+0];
				tm.vVerticies[i].uv[1] = (*vpForTex)[i*2+1];
			}

			for( unsigned i = 0; i < tm.vTriangles.size(); i++ )
			{
				tm.vTriangles[i].iFace[0] = vvLists[0][i*3+0];
				tm.vTriangles[i].iFace[1] = vvLists[0][i*3+1];
				tm.vTriangles[i].iFace[2] = vvLists[0][i*3+2];
			}

			//Add material for this mesh...
			mout.vMaterials.resize( mout.vMaterials.size() + 1 );
			mout.vMaterials[mout.vMaterials.size() - 1] = matname + ".ini";
		}
	}


	vector< Bone > & bl = mout.vBones;

	if( lcontrol != -1 )
	{
		XMLCog & skin = c.children[lcontrol].children[0].children[0];
		XMLCog & lv = c.children[lvscenes].children[0].children[0];
		//Setup all the bones now...

		//Enumerate All Bones
		for( unsigned i = 0; i < skin.children.size(); i++ )
		{
			if( skin.children[i].data == "source" )
			{
				for( unsigned j = 0; j < skin.children[i].children.size(); j++ )
				{
					XMLCog & c = skin.children[i].children[j];
					if( c.data == "Name_array" )
					{
						string bones = c.payload;

						string sThisBone = "";
						for( unsigned i = 0; i <= bones.length(); i++ )
						{
							if( bones.c_str()[i] == ' ' || bones.length() == i )
							{
								bl.resize( bl.size() + 1 );
								bl[bl.size()-1].sName = sThisBone;
								sThisBone = "";
							}
							else
								sThisBone+= bones.c_str()[i];
						}
					}
				}
			}
		}

		printf( "Bones found: %d\n", bl.size() );

		for( unsigned i = 0; i < lv.children.size(); i++ )
		{
			if( lv.children[i].data == "node" && lv.children[i].leaves["type"] == "JOINT" )
			{
				SetupBone( 0, &lv.children[i], bl );
			}
		}

		map< string, vector< int > > sources;

		for( unsigned i = 0; i < skin.children.size(); i++ )
		{
			if( skin.children[i].data == "source" )
			{
				string sName = skin.children[i].leaves["name"];
				for( unsigned j = 0; j < skin.children[i].children.size(); j++ )
				{
					XMLCog & sid = skin.children[i].children[j];

					if( sid.data == "float_array" )
					{
						const char * pl = sid.payload.c_str();
						string stmp = "";
						for( unsigned i = 0; i <= sid.payload.length(); i++ )
						{
							if( pl[i] == ' ' || i == sid.payload.length() )
							{
								sources[sName].push_back( atof( stmp.c_str() ) );
								stmp = "";
							}
							else
							{
								stmp += pl[i];
							}
						}
					}
				}
			}
		}

		//Sources is now filled out, go find the vertex weight
		for( unsigned i = 0; i < skin.children.size(); i++ )
		{
			if( skin.children[i].data == "vertex_weights" )
			{
				string jid = "";
				string wid = "";

				int jido = 0;
				int wido = 1;

				vector< int > vcount;
				vector< int > vs;

				for( unsigned j = 0; j < skin.children[i].children.size(); j++ )
				{
					XMLCog & c = skin.children[i].children[j];
					if( c.data == "input" )
					{
						if( c.leaves["semantic"] == "JOINT" )
						{
							jid = c.leaves["source"].substr(1);
							jido = atoi( c.leaves["offset"].c_str() );
						}
						if( c.leaves["semantic"] == "WEIGHT" )
						{
							wid = c.leaves["source"].substr(1);
							wido = atoi( c.leaves["offset"].c_str() );
						}
					}

					if( c.data == "vcount" )
						IntParse( c.payload, vcount );
					if( c.data == "v" )
						IntParse( c.payload, vs );
				}

				int iCurVertex = 0;
				int iTC = 0;

				//Wooh, we have everything oriented by vertex...
				for( unsigned i = 0; i < vs.size()/2; i++ )
				{
					iTC++;
					if( vcount[iCurVertex] < iTC )
					{
						iCurVertex++;
						iTC = 0;
					}

					int iJoint = vs[i*2+jido];
					int iWeight = vs[i*2+wido];

					if( iJoint >= bl.size() )
					{
						fprintf( stderr, "Warning.  Attempting to access joint outside workable area (joint %d)\n", iJoint );
					}
					else
					{
						bl[iJoint].vAssignments.resize( bl[iJoint].vAssignments.size() + 1 );
						bl[iJoint].vAssignments[bl[iJoint].vAssignments.size()-1].iMesh = 0;
						bl[iJoint].vAssignments[bl[iJoint].vAssignments.size()-1].iVertex = iCurVertex;
						bl[iJoint].vAssignments[bl[iJoint].vAssignments.size()-1].fWeight = sources[wid][iWeight];
					}
				}
			}
		}

		//Gotta load the animations...
		if( lvanimations == -1 )
			fprintf( stderr, "Could not find library_animations.  No animations to load.\n" );
		else
		{
			float fMaxTime = 0.0;
			map< string, vector< AnimationKey > > mKeys;
			map< string, vector< float > > mTimes;
			map< string, vector< float > > mvVals;

			for( unsigned i = 0; i < c.children[lvanimations].children.size(); i++ )
			{
				XMLCog & bt = c.children[lvanimations].children[i];
				if( bt.data == "animation" )
				{
					map< string, vector< float > > mvVals;
					string sAnimation = "";
					for( unsigned i = 0; i < bt.leaves["id"].length(); i++ )
					{
						if( bt.leaves["id"].c_str()[i] == '.' )
							break;
						else
							sAnimation += bt.leaves["id"].c_str()[i];
					}

					printf( "Animation: %s : ", sAnimation.c_str() );
					if( FindBone( bl, sAnimation ) == -1 )
					{
						printf( "Ignore.\n" );
						continue;
					}
					else
					{
						printf( "Use.\n" );
					}

					for( unsigned i = 0; i < bt.children.size(); i++ )
					{
						XMLCog & dt = bt.children[i];
						if( dt.data == "source" )
						{
							int Technique = dt.FindChild( "technique_common" );
							int FloatArray = dt.FindChild( "float_array" );
							if( Technique == -1 || FloatArray == -1 )
								continue;
							XMLCog & Tech = dt.children[Technique].children[0];
							XMLCog & Data = dt.children[FloatArray];
							string sName = Tech.children[0].leaves["name"];
							string sData = Data.payload;
							FloatParse( sData, mvVals[sName] );
						}
					}

					if( mvVals["TIME"].size() == 0 || mvVals["TRANSFORM"].size() == 0 )
					{
						fprintf( stderr, "Error: Missing Time or Transform for Animation: %s\n", sAnimation.c_str() );
						exit( -3 );
					}

					mTimes[sAnimation] = mvVals["TIME"];
					vector< AnimationKey > & k = mKeys[sAnimation];
					int keys = mvVals["TIME"].size();
					k.resize( keys );
					for( unsigned i = 0; i < keys; i++ )
					{
						if( mvVals["TIME"][i] > fMaxTime )
							fMaxTime = mvVals["TIME"][i];
						float * fd = &mvVals["TRANSFORM"][i*12];
						AnimationKey & tk = k[i];
						
						tk.pPos.x = fd[3];
						tk.pPos.y = fd[7];
						tk.pPos.z = fd[11];
				
						tk.qRot.w = sqrtf( 1.0 + fd[0] + fd[5] + fd[10] );
						float w4 = ( 4.0 * tk.qRot.w );
						tk.qRot.x = (fd[6] - fd[9]) / w4;
						tk.qRot.y = (fd[8] - fd[2]) / w4;
						tk.qRot.z = (fd[1] - fd[4]) / w4;
						tk.fTime = mvVals["TIME"][i];
					}
				}
			}

//			map< string, vector< AnimationKey > > mKeys;
//			map< string, vector< float > > mTimes;

			//mTimes and mKeys is filled out, use them!

			mout.vAnimations.resize(mout.vAnimations.size()+1);
			Animation & RootAnimation = mout.vAnimations[0];

			RootAnimation.sName = "default";
			RootAnimation.fDuration = fMaxTime;

			for( map< string, vector< AnimationKey > >::iterator i = mKeys.begin(); i != mKeys.end(); i++ )
			{
				string sBone = i->first;

				RootAnimation.vTracks.resize( RootAnimation.vTracks.size() + 1 );
				AnimationTrack & at = RootAnimation.vTracks[ RootAnimation.vTracks.size() - 1 ];

				at.iBone = FindBone( bl, sBone );

				at.vKeys = i->second;
			}
			printf( "Animations found, animations are %f long.\n", fMaxTime );
		}
	}


	printf( "Extents: %f %f %f -> %f %f %f\n", minx, miny, minz, maxx, maxy, maxz );
	mout.SaveModel( argv[2] );

//	printf( "%d %d\n", lvanimations, ltest );
//	for( int i = 0; i < c.children.size(); i++ )
//	{
//		printf( "%d %s\n", i, c.children[i].data.c_str() );
//	}
}