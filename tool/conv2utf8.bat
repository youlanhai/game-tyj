@echo off

::convert code to utf-8 bom

encodeconv -p MyGame h;cpp;c -d utf-8 -s utf-8

encodeconv -p Font h;cpp;c -d utf-8 -s utf-8

encodeconv -p LZ3DEngine h;cpp;c -d utf-8 -s utf-8

encodeconv -p LZData h;cpp;c -d utf-8 -s utf-8

encodeconv -p LZGUI h;cpp;c -d utf-8 -s utf-8

encodeconv -p Navigation h;cpp;c -d utf-8 -s utf-8

encodeconv -p Physics h;cpp;c -d utf-8 -s utf-8

encodeconv -p PyWraper h;cpp;c -d utf-8 -s utf-8

encodeconv -p utility h;cpp;c -d utf-8 -s utf-8

pause
