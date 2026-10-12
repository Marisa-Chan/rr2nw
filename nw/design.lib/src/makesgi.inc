
COMMONPREF = standard
include $(ROOT)/usr/include/make/commondefs
sinclude $(ROOT)/usr/include/make/$(PRODUCT)defs

#ifndef	DEBUG
DEBUG=1
#endif

PROJ = /usr/people/engine/design.lib
I = $(PROJ)/h
SRC = $(PROJ)/src

OUT = $(PROJ)/sgi

#if $(DEBUG) != 0
O = $(OUT)/objd
LIBNAME = $(OUT)/designd
#else
O = $(OUT)/obj
LIBNAME = $(OUT)/design
#endif

##CPP = $(C++F) -fullwarn -I$(I) -o $(O)/
CPP = $(C++F) -c -xansi -I$(I) -o $@ 

#if $(DEBUG)
CPP += -g2 -D_DEBUG
#else
CPP += -g0
#endif


#.cpp.o:
#	$(CPP) $(.PREFIX).cpp

