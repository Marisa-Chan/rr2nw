!include ..\..\..\make.inc

CPP += -i..\..

OTAR  = $(O)\object.obj $(O)\s_debug.obj $(O)\setofid.obj $(O)\context.obj
OTAR += $(O)\session.obj $(O)\echo.obj $(O)\s_evdata.obj $(O)\active.obj



kernelw.lib: $(OTAR)
	wlib kernelw.lib $(OTAR)


TARGETS = kernelw.lib

$(BUILD)




