@echo off
if '%1==' goto usage
if '%3==' goto usage
if %2==BC goto bc
if %2==WC goto wc
:usage
echo Usage: MAKEMOD module BC³WC dbgflag { opt}
pause
goto end
:bc
echo --------------------------- %1 ------------------------------
cd %1
make -f makebc -s -DDEBUG=%3 %4 %5 %6 %7
if errorlevel 1 goto err
goto ascend
:wc
echo --------------------------- %1 ------------------------------
cd %1
wmake -f makewc /h /s DEBUG=%3 %4 %5 %6 %7
if errorlevel 1 goto err
goto ascend
:err
echo !!!!!!!!!!!!!!!!! Error in %1 !!!!!!!!!!!!!!!!!!
pause
:ascend
cd ..
:end
