
@echo off

set cur_dir=%cd%

del /s /q ..\package\mygame\*

call _package_program.bat
cd /d %cur_dir%

call _package_py.bat
cd /d %cur_dir%

xcopy ..\res ..\package\mygame\res /i /s /y /EXCLUDE:res_exclude.txt
pause
