CFLAGS=-O2 -g0 -Wall -I/usr/include/libxml2 -I.. -msse2 -DHAVE_CONFIG -DHGENGINE -fexceptions -fPIC -I../src -I../src/DataTypes -I../src/DataStructures -g 
CXXFLAGS=${CFLAGS}
LDFLAGS=-shared

all : BillboardNode.so TextNode.so Terrain.so

clean :
	rm -rf *~ *.o *.so

BillboardNode.so : BillboardNode.o
	g++ -o$@ $^ ${LDFLAGS}

TextNode.so : TextNode.o
	g++ -o$@ $^ ${LDFLAGS}

Terrain.so : Terrain.o
	g++ -o$@ $^ ${LDFLAGS}
