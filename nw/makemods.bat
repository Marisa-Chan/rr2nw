@echo off
call makemod INPUT %1 %2 %3 %4
call makemod GRAPH %1 %2 %3 %4
call makemod PATTERNS %1 %2 %3 %4
call makemod VIEW %1 %2 %3 %4
call makemod BUSH %1 %2 %3 %4
call makemod SCENE %1 %2 %3 %4
call makemod SOUND %1 %2 %3 %4
call makemod MAIN %1 %2 %3 %4
call makemod BRIEFING %1 %2 %3 %4
rem call makemod LAND %1 %2 %3 %4
