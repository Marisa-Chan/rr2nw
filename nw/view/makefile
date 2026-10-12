!include ..\make.inc

!ifneq DEBUG 0
TARGETS = $(O)\view.obj $(O)\terrain.obj
!else
TARGETS = $(O)\body.obj $(O)\figure.obj $(O)\object.obj $(O)\terrain.obj &
		$(O)\order.obj $(O)\dynamic.obj $(O)\objmap.obj $(O)\keyframe.obj
!endif

$(BUILD)
