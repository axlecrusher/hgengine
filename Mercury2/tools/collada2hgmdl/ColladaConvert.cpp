#include <stdio.h>
#include <stdlib.h>
#include "XMLCog.h"

int main( int argc, char ** argv )
{
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
		}
		else if( geoitem.data == "vertices" )
		{
			string sSynonym = geoitem.leaves["name"];
			//Information about how to map things toegether.
			for( int i = 0; i < geoitem.children.size(); i++ )
			{
				XMLCog & idata = geoitem.children[i];
				if( idata.data == "input" )
					synonyms[sSynonym][idata.leaves["semantic"]] = idata.leaves["source"].substr(1);
			}
		}
	}
	

//	printf( "%d %d\n", lvanimations, ltest );
//	for( int i = 0; i < c.children.size(); i++ )
//	{
//		printf( "%d %s\n", i, c.children[i].data.c_str() );
//	}
}