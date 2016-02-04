@echo off

set fn=%1
echo Zpracovani souboru: %fn%.asm

if not exist %fn%.obj goto j1
del %fn%.obj

:j1
if not exist %fn%.exe goto j2
del %fn%.exe

:j2
if not exist %fn%.lst goto j3
del %fn%.lst

:j3
if not exist %fn%.map goto j4
del %fn%.map

:j4
if not exist %fn%.asm goto input_err
if not exist NASM.EXE goto no_compiler

echo Preklad souboru "%fn%.asm" pomoci NASM.EXE.
nasm -f obj %fn%.asm
rem -l %fn%.lst

if not exist %fn%.obj goto compile_err

if not exist LINK.EXE goto no_linker
echo Sestavovani pomoci LINK.EXE:
link "%fn%.obj","%fn%.exe",nul.map,,nul.def

rem if not exist aLINK.EXE goto no_linker
rem echo Sestavovani pomoci aLINK.EXE:
rem alink "%fn%.obj" -oEXE -m-

if not exist %fn%.exe goto link_err

echo -------------------------------------------------------------
echo Spusteni %fn%.EXE:
echo -------------------------------------------------------------
%fn%.EXE
echo -------------------------------------------------------------
echo Program spravne ukoncen, rizeni vraceno operacnimu systemu.
echo -------------------------------------------------------------
goto exit

:input_err
	echo Soubor %fn%.asm neexistuje.
	goto exit
:no_compiler
	echo Nemohu nalezt program NASM.EXE.
	goto exit
:no_linker
	echo Nemohu nalezt program LINK.EXE.
	goto exit
:compile_err
	echo Chyba pri prekladu programem NASM.EXE.
	goto exit
:link_err
	echo Chyba pri sestavovani programem LINK.EXE.
:exit
	pause