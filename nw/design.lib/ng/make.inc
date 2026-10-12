
PROJ = C:\DESIGN.LIB
NG = $(PROJ)\NG\design

NGC = c:\help\ng\ngc.exe
NGML = c:\help\ng\ngml.exe

.EXTENSIONS:
.EXTENSIONS: .ng .lnk .ngo .ngs


.ngs.ngo:
	$(NGC) $^&.ngs

.lnk.ng:
	$(NGML) $^&.lnk

BUILD =all: $(TARGETS)

.ERASE
