
!ifndef DEBUG
DEBUG = 0
!endif

PROJ = D:\DESIGN.LIB
I = $(PROJ)\H
SRC = $(PROJ)\SRC

OUT = $(PROJ)\BC

!if $(DEBUG)!=0
O = $(OUT)\OBJ\DD
LIBNAME = $(OUT)\designdd
!else
O = $(OUT)\OBJ\DR
LIBNAME = $(OUT)\designdr
!endif

ASM = tasm /ml /m /t /zi
ASMSX = ,$(O)\

CPPBASE = bcc -c -3 -ml -d -rd -I$(I) -n$(O)
LIBR = tlib /C

!if $(DEBUG)!=0
CPP = $(CPPBASE) -N -v -D_DEBUG
!else
CPP = $(CPPBASE) -v -vi
!endif

.autodepend

{.\}.cpp{$(O)\}.obj:
	$(CPP) $&.CPP

{.\}.asm{$(O)\}.obj:
	$(ASM) $&.ASM$(ASMSX)

