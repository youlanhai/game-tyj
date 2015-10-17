@echo off

cd ..\

echo 正在删除旧文件...
del /S /Q package\python27\*.*

echo 正在抽取...

python.exe res\entities\tools\package.py

cd package\python27

"D:\Program Files (x86)\HaoZip\HaoZipC.exe" a -tzip python27.zip -r *.*

move python27.zip ..\mygame
