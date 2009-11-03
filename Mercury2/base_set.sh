#Mercury's setter for the mini configurator
#Copyright 2007 Charles Lohr, licensed under the MIT/X11 License.

ISMAC=0
if test $OSTYPE = "darwin"; then
	ISMAC=1; fi
if test $OSTYPE = "darwin8.0"; then
	ISMAC=1; fi


OPTIONS="X11 libxml OGL sse gprof glprofile instancewatch"
OPT_X11=1
OPT_OGL=1
OPT_libxml=1
OPT_sse=0
OPT_gprof=0
OPT_glprofile=0
OPT_instancewatch=1

DEFINES="WAS_CONFIGURED USE_MSTRING"
CC_BASE="-O2 -g0 -Wall"

for i in $*; do
	if test $i = "--help"; then 
		echo "CN's QuickConfigure - Help Options"
		echo " --help - Display this message"
		echo " --with-[OPTION] - enable option"
		echo " --without-[OPTION] - disable option"
		echo -n " Available options are: "
		for j in $OPTIONS; do
			echo -n "$j($(eval echo \$$(eval 'echo OPT_$j'))) "
		done
		echo ""
		exit -1
	fi

	for j in $OPTIONS; do
		if test $i = "--with-$j"; then
			eval OPT_$j=1;
		fi
		if test $i = "--without-$j"; then
			eval OPT_$j=0;
		fi
	done
done

NEED_H="stdio.h stdlib.h"
WANT_H="time.h"
WANT_L="iberty"
CC_BASE="$CC_BASE -I."

NEED_L="m c z pthread png pthread";


if test $OPT_libxml = 1; then
	CC_BASE="$CC_BASE -I/usr/include/libxml2"
	NEED_H="$NEED_H libxml/parser.h"
	NEED_L="$NEED_L xml2"
	DEFINES="$DEFINES USE_LIBXML"
fi


if test $OPT_X11 = 1; then
	NEED_L="$NEED_L X11"	
	NEED_H="$NEED_H X11/Xlib.h"
	NEED_H="$NEED_H GL/glx.h"
	DEFINES="$DEFINES USE_X11"
fi

if test $OPT_gprof = 1; then
	CC_BASE="$CC_BASE -pg"
	LD_BASE="$LD_BASE -pg"
fi

if test $OPT_glprofile = 1; then
	DEFINES="$DEFINES GL_PROFILE"
fi

if test $OPT_OGL = 1; then
	NEED_H="GL/gl.h"
	NEED_L="$NEED_L GL GLU"
fi

if test $OPT_sse = 1; then
	DEFINES="$DEFINES USE_SSE"
fi

if test $ISMAC = 1; then
	CC_BASE="$CC_BASE -I"/System/Library/Frameworks/OpenGL.framework/Headers" -Isrc -I/usr/X11R6/include -I.  -Isrc/ode -Iode -Isrc/png -Ipng"
	LD_BASE="-Lsrc/maclib -framework OpenGL -lobjc -framework GLUT -framework ApplicationServices"
fi

if test $OPT_instancewatch = 1; then
	DEFINES="$DEFINES INSTANCE_WATCH"
fi

ARCH=`uname -m`

if test $ARCH = "i686" || test $ARCH = "i586"; then
	if test $OPT_sse = 1; then
		CC_BASE="$CC_BASE -march=pentium3"
	else
		CC_BASE="$CC_BASE -march=pentium"
	fi
fi
