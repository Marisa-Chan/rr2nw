!include ../make.inc

TARGETS = $(O)/keyboard.obj

!ifeq	NT 0
TARGETS += $(O)/handler.obj
!endif

!ifneq	DEBUG 0
TARGETS += $(O)/input.obj
!else
TARGETS += $(O)/mouse.obj $(O)/keydefs.obj
!endif

$(BUILD)
