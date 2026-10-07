!include makewc.inc

OTAR  = $(O)\assert.obj  
OTAR += $(O)\filesys.obj  
OTAR += $(O)\mathlib.obj 
OTAR += $(O)\config.obj 


designwr.lib: $(OTAR)
	wlib designwr.lib $(OTAR)


$(O)\mathlib.obj: mathlib\mathlib.cpp
      $(CPP)  mathlib\mathlib.cpp

$(O)\assert.obj: DEBUGEXT\assert.cpp
      $(CPP)  DEBUGEXT\assert.cpp

$(O)\filesys.obj: FILESYS\FILESYS.cpp
      $(CPP)  FILESYS\FILESYS.cpp
	  
$(O)\config.obj: FILESYS\config.cpp
      $(CPP)  FILESYS\config.cpp


TARGETS = designwr.lib 


$(BUILD)
