@echo off
rem secho d [
rem call compile.bat echodir.bat %1 %2 %3 %4
rem secho n ]

rem secho d [
call compile.bat makemod.bat %1 %2 %3 %4
rem secho n ]

wmake %1 %2 %3 %4
