@echo off
call makemod DEBUGEXT %1 %2 %3 %4
call makemod FILESYS %1 %2 %3 %4
call makemod MATHLIB %1 %2 %3 %4
call makemod LANG %1 %2 %3 %4
