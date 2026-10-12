@echo off
if '%1==' goto name
echo ----------------------- Processing %1 ----------------------------
cd %1
wmake /h /s %2 %3 %4 %5
if errorlevel 1 goto err
goto end
:name
echo Module name missing
:err
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo !!!!!!!!! Error in module %1
pause
:end
cd ..