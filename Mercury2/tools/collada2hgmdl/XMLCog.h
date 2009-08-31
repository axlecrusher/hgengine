#ifndef _XML_COG_H
#define _XML_COG_H

#pragma warning (disable : 4786) // turn off broken debugger warning
#include <vector>
#include <string>
#include <map>

using namespace std;

struct XMLCog
{
	void Clear();
	vector< XMLCog > children;
	int FindChild( const string & sName );
	map< string, string > leaves;
	string data;
	string payload;
};

bool LoadXML( const char * file, XMLCog & cog );

#endif