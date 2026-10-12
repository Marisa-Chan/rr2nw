
!ifndef	DEBUG
DEBUG=0
!endif

PROJ = C:\nw\DESIGN.LIB\SRC
I = $(PROJ)\H
SRC = $(PROJ)\SRC

OUT = $(PROJ)\WC

!ifneq DEBUG 0
O = $(OUT)\OBJ\DD
LIBNAME = $(OUT)\designwd
!else
O = $(OUT)\OBJ\DR
LIBNAME = $(OUT)\designwr
!endif


ASM = tasm /ml /m /t /zi
ASMSX = ,$(O)\

CPP = wpp386 -mf -5s -zq -i$(I) -fo$(O)\
LIBR = wlib -c -b -n -l=$(LIBNAME).lst

!ifeq DEBUG 1
CPP += -d2 -d_DEBUG -w4 -bt=nt
!endif
!ifeq DEBUG 0
CPP += -d0 -oneatmilrf -s -fpi87 -fp5 -w4 -bt=nt
!endif
!ifeq DEBUG 2
O = $(OUT)\OBJ\WD
LIBNAME = $(OUT)\designwd
CPP += -d2 -d_DEBUG -w4
CPP += -bt=nt
!endif
!ifeq DEBUG 3
O = $(OUT)\OBJ\WR
LIBNAME = $(OUT)\designwr
CPP += -d0 -oneatmilrf -s -fpi87 -fp5 -w4
CPP += -bt=nt
!endif

.cpp.obj: .AUTODEPEND
	$(CPP) $^&.CPP

.asm.obj: .AUTODEPEND
	$(ASM) $^&.ASM$(ASMSX)

.ERASE
