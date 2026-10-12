@echo off
if '%1==' goto err
if exist BACKUP.ARC\%1.ZIP goto exist
pkzip -rP BACKUP.ARC\%1.ZIP @arc.rsp
goto end
:exist
echo File BACKUP.ARC\%1.ZIP already exists
goto end
:err
echo Usage: arc arcname
:end
