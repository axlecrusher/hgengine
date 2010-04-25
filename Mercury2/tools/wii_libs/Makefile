include $(DEVKITPPC)/wii_rules

all : libpng.a libz.a

PNG_TGZ:=libpng-1.4.1.tar.gz
PNG_FOLDER:=libpng-1.4.1

Z_TGZ:=zlib-1.2.4.tar.gz
Z_FOLDER:=zlib-1.2.4

$(PNG_TGZ) :
	wget ftp://ftp.simplesystems.org/pub/libpng/png/src/libpng-1.4.1.tar.gz

$(PNG_FOLDER) : $(PNG_TGZ)
	tar xzvf $(PNG_TGZ)

libpng.a : $(PNG_FOLDER) libz.a
	(cd $(PNG_FOLDER)&&make -f../Makefile.png&&cd ..)
	cp $(PNG_FOLDER)/libpng.a .

$(Z_TGZ) :
	wget http://zlib.net/zlib-1.2.4.tar.gz

$(Z_FOLDER) : $(Z_TGZ)
	tar xzvf $(Z_TGZ)

libz.a : $(Z_FOLDER)
	(cd $(Z_FOLDER)&&make -f../Makefile.zlib&&cd ..)
	cp $(Z_FOLDER)/libz.a .

clobber : clean
	rm -rf $(PNG_TGZ) $(PNG_FOLDER) $(Z_TGZ) $(Z_FOLDER)

clean : 
	rm -rf *~ libz.a libpng.a
	(cd $(Z_FOLDER)&&make -f../Makefile.zlib clean&&cd ..)
	(cd $(PNG_FOLDER)&&make -f../Makefile.png clean&&cd ..)
	
