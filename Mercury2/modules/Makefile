CFLAGS=-O2 -g0 -Wall -I/usr/include/libxml2  -DHAVE_CONFIG -DHGENGINE -fno-exceptions -fPIC -I../src -g 
CXXFLAGS=${CFLAGS}
LDFLAGS=-shared

all : BillboardNode.so

clean :
	rm -rf *~ *.o *.so

BillboardNode.so : BillboardNode.o
	g++ -o$@ $^ ${LDFLAGS}

