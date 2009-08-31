all : ColladaConvert

CFLAGS=-g

ColladaConvert : ColladaConvert.o XMLCog.o
	g++ -o $@ $^

clean :
	rm -rf *.o *~ ColladaConvert