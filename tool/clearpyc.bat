@echo off
echo clear pyc files

cd ..\res\entities
erase /S *.pyc;*.pyo;*.bak;

echo clear finished.
pause