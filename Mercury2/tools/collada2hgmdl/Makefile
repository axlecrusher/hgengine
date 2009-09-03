all : ColladaConvert

CFLAGS=-g
CXXFLAGS=-g

ColladaConvert : ColladaConvert.o XMLCog.o ModelContainer.o MercuryTypes.o MercuryMath.o MercuryMatrix.o
	g++ -g -o $@ $^

clean :
	rm -rf *.o *~ ColladaConvert