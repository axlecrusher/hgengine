all : hgfonter

CFLAGS= -I/usr/include/freetype2
CXXFLAGS=$(CFLAGS)

hgfonter : main.o
	g++ -o $@ $^ -lpng -lfreetype
