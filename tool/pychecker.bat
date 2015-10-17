@echo off
echo python syntax check
pause 
:start
cls
echo start check ...

set SCRIPTS=D:\Python27\Lib\site-packages\pyflake.py
::set SCRIPTS=D:\Python27\Scripts\pyflakes.py

python %SCRIPTS%  ..\res\entities

choice /M "check finish continue\exit"
if errorlevel 2 goto end
if errorlevel 1 goto start

:end
