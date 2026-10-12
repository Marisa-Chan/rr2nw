path c:\wc\binnt;c:\bc\bin
set WATCOM=c:\wc
set INCLUDE=c:\wc\h;c:\wc\h\nt
rem set EDPATH=c:\wc\eddat
set TMP=c:\temp
set TEMP=c:\temp
set LOGNAME=ZAV
set TZ=MSK-3MSD
set HLP=c:\help\helppc

copy ..\make.inc make.incsav
copy make.inc ..\make.inc
cd ..
call makeall.bat %1
cd RMT
copy make.incsav ..\make.inc
