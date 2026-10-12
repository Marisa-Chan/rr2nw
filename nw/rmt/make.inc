DEBUG=0
NT=1
OPENGL=0
GLIDE=0

PROJ = I:\
DLIB = I:\DESIGN.LIB

!ifdef _3DE
DEBUG=1
NT=1
OPENGL=0
GLIDE=0
!endif

!ifneq OPENGL 0
NT=1
GLIDE=0
!endif

!ifneq DEBUG 0
L_DEBUG=D
!else
L_DEBUG=R
!endif

!ifneq NT 0
L_SYS=W
!else
L_SYS=D
!endif

!ifneq OPENGL 0
L_SYS=G
!endif
!ifneq GLIDE 0
L_SYS+=F
!endif

L_TARG=$(L_SYS)$(L_DEBUG)

I=$(PROJ)\INCLUDE;$(DLIB)\H;$(PROJ)\ARENA;$(PROJ)\ARENA\H
LIBPATH=$(DLIB)\WC
OUT=..\..\OUTPUT


EXE=$(OUT)\game$(L_TARG).exe
O0 = OBJ\$(L_TARG)
O = ..\$(O0)
LINKRSP =..\..\link$(L_TARG).rsp

!ifdef _3DE
LIBRARIAN = wlib -n
O0 = OBJ\3$(L_DEBUG)
EXE = 3de$(L_DEBUG).lib
LIBRARIANRSP = $(PROJ)\3de$(L_DEBUG).rsp
!endif

CPP = wpp386 -mf -5s -zq -i$(I) -fo$(O)\
C = wcc386 -mf -5s -zq -i$(I) -fo$(O)\
ASM = tasmx /ml /m /t /zi /w-ALN
ANGEL = angel
LINK = wlink libp $(LIBPATH) d a

!ifneq DEBUG 0
CPP += -d2 -d_DEBUG -wx -we
C += -d2 -d_DEBUG
#ASM += /d_DEBUG
!else
CPP += -s
CPP += -d0 -oneatmilrf -7 -fp5
!endif

!ifneq NT 0
CPP += -bt=nt
ASM += /d_WINNT
!else
CPP += -bt=dos
!endif

!ifneq OPENGL 0
CPP += -d_OPENGL
!endif

!ifneq GLIDE 0
CPP+= -d_3DFX
!endif

ASMSX = ,$(O)\

!ifdef _3DE
CPP += -d_VIEW_ORTHO
ASM += /d_WINNT
!endif

.EXTENSIONS: .ang

.cpp.obj: .AUTODEPEND
	$(CPP) $^&.CPP

.c.obj: .AUTODEPEND
	$(C) $^&.C

.asm.obj: .AUTODEPEND
	$(ASM) $^&.ASM$(ASMSX)

.ang.obj: .AUTODEPEND
	$(ANGEL) $^&.ANG $(O)\$^&.ASX
	$(ASM) $(O)\$^&.ASX$(ASMSX)

BUILD =all: $(TARGETS)

.ERASE
