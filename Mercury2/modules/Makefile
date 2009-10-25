CFLAGS=-O2 -g0 -Wall -I/usr/include/libxml2 -I.. -msse2 -DHAVE_CONFIG -DHGENGINE -fexceptions -fPIC -I../src -I../src/DataTypes -I../src/DataStructures -g  -I.
CXXFLAGS=${CFLAGS}
LDFLAGS=-shared

all : BillboardNode.so TextNode.so Terrain.so TextPlate.so

clean :
	rm -rf *~ *.o *.so

%.so : %.o
	g++ -o$@ $^ ${LDFLAGS}

