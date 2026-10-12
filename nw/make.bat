@echo off
@echo ============ ARENA KERNEL =======================================
cd kernel\src
wmake /s /h %1 %2 %3 %4
cd ..\..
if errorlevel 1 goto err
@echo ============ ARENA Object Base ==================================
cd obase
call make.bat  %1 %2 %3 %4
cd ..
if errorlevel 1 goto err
@echo ============ ARENA Suacript SRC =================================
cd sc\src
wmake /s /h  %1 %2 %3 %4
cd ..\..
if errorlevel 1 goto err
@echo ============ ARENA Suacript ENV =================================
cd sc\env
wmake /s /h  %1 %2 %3 %4
cd ..\..
if errorlevel 1 goto err
@echo ============ ARENA Storage ======================================
cd storage
wmake /s /h %1 %2 %3 %4
cd ..
if errorlevel 1 goto err
wmake /s /h %1 %2 %3 %4
if errorlevel 1 goto err
goto end

:err
pause
:end
