#include <iostream>
#include <fstream>
#include <vector>
#include <stdint.h>
#include <MercuryPoint.h>
#include <map>
#include <math.h>

using namespace std;

float min(float x, float y) { return x<y?x:y; }
float max(float x, float y) { return x>y?x:y; }

vector< MercuryPoint > v;
vector< MercuryPoint > vt;
vector< MercuryPoint > vn;

float minX, maxX, minY, maxY, minZ, maxZ;

struct Vertex
{
	MercuryPoint uv;
	MercuryPoint normal;
	MercuryPoint position;
	
	inline bool operator==(const Vertex& rhs)
	{
		return (position == rhs.position) && (uv == rhs.uv) && (normal == rhs.normal);
	}
};

vector< Vertex > vertice;
vector< uint16_t > indice;

void LineParser(const string &line)
{
	if (line.empty()) return;
	string token = line.substr(0,2);
	if (token == "v ")
	{
		MercuryPoint tv;
		sscanf(line.c_str(), "v %f %f %f", &tv.x, &tv.y, &tv.z);
		v.push_back(tv);
		
		minX = min(minX, tv.x);
		minY = min(minY, tv.y);
		minZ = min(minZ, tv.z);

		maxX = max(maxX, tv.x);
		maxY = max(maxY, tv.y);
		maxZ = max(maxZ, tv.z);
	}
	else if (token == "vt")
	{
		MercuryPoint tv;
		sscanf(line.c_str(), "vt %f %f", &tv.x, &tv.y);
		tv.y = 1 - tv.y; //XXX reverse
		vt.push_back(tv);
	}
	else if (token == "vn")
	{
		MercuryPoint tv;
		sscanf(line.c_str(), "vn %f %f %f", &tv.x, &tv.y, &tv.z);
		vn.push_back(tv);
	}
	else if (token == "f ")
	{
		uint32_t iv[3], ivt[3], ivn[3];
		int r;
		
		r = sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &iv[0], &ivt[0], &ivn[0], &iv[1], &ivt[1], &ivn[1], &iv[2], &ivt[2], &ivn[2]);

		if (r != 9)
		{
			r = sscanf(line.c_str(), "f %d/%d %d/%d %d/%d", &iv[0], &ivt[0], &iv[1], &ivt[1], &iv[2], &ivt[2]);
			ivn[0] = ivn[1] = ivn[2] = 1;
			MercuryPoint tv;
			vn.push_back(tv);
		}
		
		Vertex tv[3];
		
		for (int i=0; i < 3; ++i)
		{
			bool found = false;
			tv[i].position = v[ iv[i]-1 ];
			tv[i].normal = vn[ ivn[i]-1 ];
			tv[i].uv = vt[ ivt[i]-1 ];
			
			for (unsigned long j = 0; j < vertice.size(); ++j)
			{
				if (tv[i] == vertice[j])
				{
					indice.push_back( j );
					found = true;
					break;
				}
			}
			
			if ( !found )
			{
				indice.push_back( vertice.size() );
				vertice.push_back(tv[i]);
			}
		}
	}
}

void WriteOBB(FILE *mbmf)
{
	char type[] = "OBB ";
	fwrite(type, sizeof(char)*4, 1, mbmf);
	
	uint32_t tmp32 = sizeof(float)*6; fwrite(&tmp32, sizeof(uint32_t), 1, mbmf);
	float tmp[3];
	
	//center
	tmp[0] = (maxX+minX)/2.0f;
	tmp[1] = (maxY+minY)/2.0f;
	tmp[2] = (maxZ+minZ)/2.0f;
	fwrite(tmp, sizeof(float)*3, 1, mbmf);
	printf("center: %f %f %f\n", tmp[0], tmp[1], tmp[2]);

	//extends
	tmp[0] = (maxX-minX)/2.0;
	tmp[1] = (maxY-minY)/2.0;
	tmp[2] = (maxZ-minZ)/2.0;
	fwrite(tmp, sizeof(float)*3, 1, mbmf);
	printf("extends: %f %f %f\n", tmp[0], tmp[1], tmp[2]);
}

void WriteMBMF( FILE *mbmf )
{
	uint32_t tmp32;
	uint16_t tmp16;
	bool b;
	
	fwrite("MBMF", 4, 1, mbmf);
	tmp16 = 2; fwrite(&tmp16, sizeof(uint16_t), 1, mbmf);
	tmp16 = 3; fwrite(&tmp16, sizeof(uint16_t), 1, mbmf);
	
	tmp16 = 1; fwrite(&tmp16, sizeof(uint16_t), 1, mbmf);
	
	//mesh
	{
		tmp32 = 4; fwrite(&tmp32, sizeof(uint32_t), 1, mbmf);
		fwrite("test", 4, 1, mbmf);
	
		b = false; fwrite(&b, sizeof(bool), 1, mbmf);

		tmp32 = vertice.size()*sizeof(float)*8; fwrite(&tmp32, sizeof(uint32_t), 1, mbmf);
		cout << tmp32 << endl;
		for (uint32_t i = 0; i < vertice.size(); ++i)
		{
			fwrite(&vertice[i].uv, sizeof(float)*2, 1, mbmf);
			fwrite(&vertice[i].normal, sizeof(MercuryPoint), 1, mbmf);
			fwrite(&vertice[i].position, sizeof(MercuryPoint), 1, mbmf);
		}
	
		tmp16 = indice.size(); fwrite(&tmp16, sizeof(uint16_t), 1, mbmf);
		for (uint16_t i = 0; i < indice.size(); ++i)
			fwrite(&indice[i], sizeof(uint16_t), 1, mbmf);
		
		tmp32 = 1; fwrite(&tmp32, sizeof(uint32_t), 1, mbmf);
		WriteOBB(mbmf);
	}
	
	tmp32 = 0;
	
	fwrite(&tmp32, sizeof(uint32_t), 1, mbmf);		
	fwrite(&tmp32, sizeof(uint32_t), 1, mbmf);		
	fwrite(&tmp32, sizeof(uint32_t), 1, mbmf);		
	fwrite(&tmp32, sizeof(uint32_t), 1, mbmf);
}

int main(int argc, char** argv)
{
	FILE *mbmf;
	ifstream obj;

	obj.open(argv[1]);
	mbmf = fopen(argv[2], "wb");

	string line;

	minX = minY = minZ = 1000000.0f;
	maxX = maxY = maxZ = -1000000.0f;	
	
	while ( getline(obj, line) )
	{
		if (line.length() > 0) LineParser(line);
	}
		
	WriteMBMF( mbmf );
	
	fclose( mbmf );
	
	return 0;
}
