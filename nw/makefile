!include make.inc

$(EXE):	.SYMBOLIC
	cd $(O0)
!ifndef _3DE
	$(LINK) n $(EXE) @$(LINKRSP)
	cd $(OUT)
!else
	$(LIBRARIAN) $(EXE) @$(LIBRARIANRSP)
!endif
