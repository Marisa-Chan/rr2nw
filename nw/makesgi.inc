
COMMONPREF = standard
include $(ROOT)/usr/include/make/commondefs
sinclude $(ROOT)/usr/include/make/$(PRODUCT)defs

#ifndef	DEBUG
DEBUG=1
#endif

PROJ = /usr/people/engine/game.2
DLIB = /usr/people/engine/design.lib
I = -I$(PROJ)/include -I$(DLIB)/h

OUT = $(PROJ)/output
TOOLKITDIR =/usr/share/src/OpenGL/toolkits
LDOPTS = -cxx -_SYSTYPE_SVR4 -require_dynamic_link _rld_new_interface -Wx,-G 0
STDOBJS = /usr/lib/crt1.o /usr/lib/c++init.o
STDLIBS = -nostdlib -L/lib -L/usr/lib
STDLIBS += -L$(TOOLKITDIR)/lib -lGLU -lGLw -lGL -lXmu -lXt -lXext -lX11 -lm
STDLIBS += -lC -lc
STDOBJSE = /usr/lib/crtn.o

#if $(DEBUG) != 0
L_TARG=d
LIBS = $(DLIB)/sgi/designd.a
#else
L_TARG=r
LIBS = $(DLIB)/sgi/design.a
#endif

EXE = $(OUT)/game$(L_TARG)
O = $(PROJ)/obj/s$(L_TARG)

WOFF = -woff 3577,3665,3101,3365
LDWOFF = -woff 131

##CPP = $(C++F) -fullwarn -I$(I) -o $(O)/
CPP = $(C++F) -c -xansi $(I) $(WOFF) -o $@

#if $(DEBUG)
CPP += -g2 -D_DEBUG
#else
CPP += -g0
#endif


#.cpp.o:
#	$(CPP) $(.PREFIX).cpp

