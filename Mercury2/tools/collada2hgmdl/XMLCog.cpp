#include "XMLCog.h" 
#include <string.h>
#include <stdlib.h>
void XMLCog::Clear()
{
	children.clear();
	leaves.clear();
}

int XMLCog::FindChild( const string & sName )
{
	for ( int i = 0; i < children.size(); i++ )
		if ( sName == children[i].data )
			return i;
	return -1;
}

//Start at first character
void ReadLeafVal( const char * start, string & pin, int & chars )
{
	pin = "";
	const char * place = start;
	while ( place[0] != '=' && place[0] != '\0' && place[0] != ' ' )
		place++;
	pin.append( start, place-start );
	chars = place-start;
}

//Start after an opening "
void ReadString( const char * start, string & pin, int & chars )
{
	pin = "";
	const char * place = start;
	while ( place[0] != '\"' && place[0] != '\0' )
	{
		if ( place[0] == '\\' )
			place++;
		pin.append( place, 1 );
		place++;
	}
	chars = place-start;
}

int ReadElem( const char * & loc, map< string, string > & pin, string & name )
{
	string	sTmp;
	string	sTmp2;
	int		iTmp;


	//Remove leading whitespace
	while ( loc[0] == ' ' || loc[0] == '\n' || loc[0] == '\r' || loc[0] == '\t' )
		loc++;

	//If we see a <, strip it.
	if ( loc[0] == '<' )
		loc++;

	//Get rid of any whitespace, i.e. <   asdf>
	while( loc[0] == ' ' )
		loc++;

	//If we see a /, we know we're closing.
	if ( loc[0] == '/' )
	{
		if ( loc[0] != '\0' )
			loc++;

		while ( loc[0] == ' ' || loc[0] == '\n' || loc[0] == '\r' || loc[0] == '\t' )
			loc++;

		while ( loc[0] != '>' && loc[0] != '\0' )
		{
			name += *loc;
			loc++;
		}

		if ( loc[0] != '\0' )
			loc++;
		return 3;
	}

	//If we've reached the end escape with error.
	if( loc[0] == 0 )
		return 0;

	//Append to the name until we hit a space.
	while( loc[0] != ' ' && loc[0] != '>' && loc[0] != '/' )
	{
		name.append( loc,1 );
		loc++;
	}

//	if ( loc[0] == '>' )
//	{
//		printf( "*" );
//		loc++;
//		return 1;
//	}

	while ( true )
	{
		//If in error, escape with error.
		if ( loc[0] == '\0' )
			return 0;

		//Remove any whitespace in this clip.
		while ( loc[0] == ' ' )
			loc++;

		//we're in the body of a block, and it terminates with an ending />
		if ( loc[0] == '/' )
		{
			loc++;
			if ( loc[0] == '\0' )
				return 0;

			//To get rid of the trailing > after the /
			loc++;
			return 2;
		}

		//Ending normally.
		if ( loc[0] == '>' )
		{
			loc++;
			return 1;
		}

		ReadLeafVal( loc, sTmp, iTmp );
		loc += iTmp;
		while ( loc[0] == ' ' )
			loc++;

		if ( loc[0] != '=' )
		{
			pin[sTmp] = "";
			continue;
		}

		loc++;

		while ( loc[0] == ' ' )
			loc++;

		//We've read a leaf, i.e.   tseq=, we're expecting a " as per required by XML
		if ( loc[0] != '\"' )
		{
			printf("Warning.  After leaf value %s, there was no starting \" beginner.\n", sTmp.c_str() );
			return 0;
		}

		loc++;

		ReadString( loc, sTmp2, iTmp );
		loc += iTmp + 1;

		pin[sTmp] = sTmp2;
	}
}

int DoXML( const char * &current, XMLCog & cog )
{
	static int fd = 0;
	int depth = 0;

	fd++;

	int ret = ReadElem( current, cog.leaves, cog.data );
	//0: FAIL
	//1: OPEN
	//2: OPEN-AND-CLOSED
	//3: CLOSE

	while ( current[0] == ' ' || current[0] == '\n' || current[0] == '\r' || current[0] == '\t' )
	{
		current++;
	}


	//There is a payload, rip it out.
	if( ret == 1 && current[0] != '<' )
	{
		while( *current != '<' && *current != 0 )
			cog.payload += *current++;
		if( *current == 0 )
		{
			fd--;
			return 0;	//Error!
		}
		else
		{
			while( *current != '>' && *current != 0 )
				current++;
			
			if( *current == 0 )
			{
				fd--;
				return 0;
			}

			current++;

			while ( current[0] == ' ' || current[0] == '\n' || current[0] == '\r' || current[0] == '\t' )
			{
				current++;
			}

			if( *current == 0 )
			{
				fd--;
				return 0;
			}

			fd--;
			return 2;
		}
	}

	switch( ret )
	{
	case 0: //error
		fd--;
		return 0;
	case 1: //open
	{
START_LOOP:
		cog.children.resize( cog.children.size() + 1 );
		int rlocal = DoXML(current, cog.children[cog.children.size()-1] );

		if( rlocal == 0 || rlocal == 3 )
		{
			cog.children.resize( cog.children.size() - 1 );
		}

		switch( rlocal )
		{
		case 0:
			fd--;
			return 0;
		case 1:
			fd--;
			return 0;
		case 2:
			goto START_LOOP;
		case 3:
			fd--;
			return 2;
		default:
			fd--;
			return 0;
		};
	}
	case 2: //open-and-close
		fd--;
		return 2;

	case 3: //close (strictly)
		fd--;
		return 3;

	default:
		fd--;
		return 0;
	}
}

bool LoadXML( const char * file, XMLCog & cog )
{
	FILE * f = fopen( file, "rb" );
	if ( f == NULL )
		return false;
	fseek( f, 0, SEEK_END );
	int len = ftell( f );
	fseek( f, 0, SEEK_SET );
	char * buff = (char*)malloc(len+1);
	fread( buff, 1, len, f );
	buff[len] = '\0';
	fclose( f );

	char * backup = buff;
	int dxresult = DoXML( (const char*&)buff, cog );
	free( backup );
//	return dxresult != 0;
	return 1;
}
