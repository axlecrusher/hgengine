#include <stdio.h>
#include <stdlib.h>
#include "XMLCog.h"
#include "ModelContainer.h"

Model mout;


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

			const char * payload = geoitem.children[floatvalspos].payload.c_str();
			int payloadlen = geoitem.children[floatvalspos].payload.length();
			vector < float > & out = sourcevalues[name];
			string thisval = "";
			for( unsigned i = 0; i < payloadlen; i++ )
			{
				if( payload[i] == ' ' )
				{
					out.push_back( atof( thisval.c_str() ) );
					thisval = "";
				}
				else
					thisval += payload[i];
			}
			if( thisval.length() )
				out.push_back( atof( thisval.c_str() ) );
			printf( "%d values (%d verts) read for %s\n", out.size(), out.size()/3, name.c_str() );
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

	printf( "Extents: %f %f %f -> %f %f %f\n", minx, miny, minz, maxx, maxy, maxz );
	mout.SaveModel( argv[2] );

//	printf( "%d %d\n", lvanimations, ltest );
//	for( int i = 0; i < c.children.size(); i++ )
//	{
//		printf( "%d %s\n", i, c.children[i].data.c_str() );
//	}
}