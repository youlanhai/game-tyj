
@echo off

cd ..\

if not exist package (
md package
)

if not exist package\mygame (
md package\mygame
)

copy audiere.dll package\mygame
copy python*.dll package\mygame
copy MyGame*.exe package\mygame
copy gameconfig.lzd package\mygame\
