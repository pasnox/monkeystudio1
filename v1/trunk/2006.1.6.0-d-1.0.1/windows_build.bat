@cls
set make=mingw32-make
%make% clean
del Makefile* /s
del object_script* /s
qmake monkey.pro
%make%
pause
