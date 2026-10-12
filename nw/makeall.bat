SET PATH=%PATH%;C:\TASM\BIN;C:\NW
@call makemods %1 %2 %3 %4 %5
cd ARENA
@call make.bat %1 %2 %3 %4 %5
cd ..
@call link %1 %2 %3 %4 %5
