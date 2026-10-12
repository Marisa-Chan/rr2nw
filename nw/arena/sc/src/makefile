!include ..\..\..\make.inc


OTAR  = $(O)\pcgen.obj $(O)\pheap.obj $(O)\pinstrm.obj $(O)\plex.obj
OTAR += $(O)\plexdesc.obj $(O)\poutstrm.obj $(O)\pparser.obj
OTAR += $(O)\pstrequ.obj $(O)\ptr_code.obj $(O)\ptr_data.obj $(O)\pusestk.obj




sc0w.lib: $(OTAR)
	wlib sc0w.lib $(OTAR)


TARGETS = sc0w.lib

$(BUILD)




