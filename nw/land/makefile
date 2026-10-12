!include ..\make.inc

TARGETS=

!ifeq OPENGL 0
TARGETS += $(O)\mskln32a.obj $(O)\sbloop.obj
!endif

!ifneq DEBUG 0
TARGETS+= $(O)\landd.obj
!else
TARGETS+= $(O)\clip.obj $(O)\land.obj $(O)\contour.obj &
	$(O)\maptools.obj $(O)\ship.obj $(O)\triang.obj $(O)\poly.obj &
	$(O)\scrlinea.obj $(O)\text.obj $(O)\clip2d.obj $(O)\movies.obj
!endif

$(BUILD)
