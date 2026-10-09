!include ..\make.inc


OTAR  = $(O)\mproj.obj $(O)\plinkex.obj $(O)\olevel.obj $(O)\super.obj $(O)\phisics.obj
OTAR += $(O)\lightobj.obj
OTAR += $(O)\dyntrace.obj
OTAR += kernel\src\kernelw.lib obase\obasew.lib sc\src\sc0w.lib sc\env\sc1w.lib
OTAR += storage\strgw.lib
OTAR += $(O)\hardware.obj
OTAR += $(O)\cnststr.obj


arenawd.lib: $(OTAR)
	wlib -p=2048 arenawd.lib $(OTAR)


$(O)\mproj.obj: mproj\mproj.cpp
      $(CPP)  mproj\mproj.cpp

$(O)\plinkex.obj: sc\plinkex.cpp
      $(CPP) sc\plinkex.cpp


$(O)\lightobj.obj: obase\light\lightobj.cpp
      $(CPP) obase\light\lightobj.cpp

$(O)\dyntrace.obj: obase\dynobj\dyntrace.cpp
      $(CPP) obase\dynobj\dyntrace.cpp


TARGETS = arenawd.lib

$(BUILD)




