CFLAGS=-O2 -g0 -Wall -I/usr/include/libxml2  -DHAVE_CONFIG -DHGENGINE -fno-exceptions -fPIC -I../src -g 
CXXFLAGS=${CFLAGS}
LDFLAGS=-shared

all : BillboardNode.so TextNode.so

clean :
	rm -rf *~ *.o *.so

BillboardNode.so : BillboardNode.o
	g++ -o$@ $^ ${LDFLAGS}

TextNode.so : TextNode.o
	g++ -o$@ $^ ${LDFLAGS}
