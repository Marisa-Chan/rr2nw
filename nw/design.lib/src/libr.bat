@echo off
if '%2==' goto usage
if %1==BC goto bc
if %1==WC goto wc
echo Usage: LIBR BC³WC dbgflag { opt}
pause
goto end
:bc
make -f makebc -s -DDEBUG=%2 %3 %4 %5 %6 %7
if errorlevel 1 goto err
goto end
:wc
wmake -f makewc -h -s DEBUG=%2 %3 %4 %5 %6 %7
if errorlevel 1 goto err
goto end
:err
echo !!!!!!!!!!!!!!!!! Error in %1 !!!!!!!!!!!!!!!!!!
pause
:end
