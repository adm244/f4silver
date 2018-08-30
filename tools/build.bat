@ECHO OFF
REM This file is a part of cdev project
REM https://github.com/adm244/cdev

SETLOCAL
REM [customize those variables]
SET libs=kernel32.lib user32.lib
SET files=%source%\main.cpp
SET hookfile=%source%\x3daudio1_7_hook\main.cpp
SET deffile=%source%\x3daudio1_7_hook\exports.def
SET hookname=X3DAudio1_7

SET asmfiles=%source%\hooks.asm
SET objfiles=hooks.obj
SET masm_args=/c /nologo %asmfiles%

SET debug=/Od /Zi /nologo /EHsc /DDEBUG
SET release=/W2 /Ot /Oi /O2 /WX /nologo /EHsc
SET args=%debug% /Fe%project% %files% /LD /link %objfiles% %libs% Psapi.lib

SET libargs=%release% /c

SET includepath=%ProgramFiles(x86)%\Microsoft DirectX SDK (June 2010)\Include
SET hookargs=/I "%includepath%" %release% /Fe%hookname% %hookfile% /LD /link /DEF:%deffile% %libs%
REM SET hookargs=/I "%includepath%" %release% /Fe%hookname% %hookfile% /LD /link %libs%

SET injector=tools\injector
SET injectorfile=..\..\%source%\%injector%\main.c
SET injectorlibs=%libs%
SET injectorname=injector
SET injectorargs=%debug% /Fe%injectorname% %injectorfile% /link %injectorlibs%

SET compiler=CL
SET masm=ML64
SET libmng=LIB
REM ###########################

SET edit=edit
SET setprjname=setname
SET hook=hook
SET arglib=lib
SET arginjector=injector

IF [%1]==[%arginjector%] GOTO BuildInjector
IF [%1]==[%arglib%] GOTO BuildLib
IF [%1]==[%hook%] GOTO BuildHook
IF [%1]==[%setprjname%] GOTO SetProjectName
IF [%1]==[%edit%] GOTO EditBuildFile
IF [%1]==[] GOTO Build
GOTO Error

:Build
ECHO: Build started...

IF NOT EXIST "%bin%" MKDIR "%bin%"
PUSHD "%bin%"
"%masm%" %masm_args%
"%compiler%" %args%
POPD

COPY "f4silver.ini" "%bin%"
ECHO: Build finished.
GOTO:EOF

:BuildInjector
SET injector_path=%source%\%injector%
IF NOT EXIST "%injector_path%" ECHO: Injector tool not found in %injector_path% && GOTO:EOF

ECHO: Building dll injector...
IF NOT EXIST "%bin%" MKDIR "%bin%"
IF NOT EXIST "%bin%\%injector%" MKDIR "%bin%\%injector%"

PUSHD "%bin%\%injector%"
"%compiler%" %injectorargs%
POPD

ECHO: Done!
GOTO:EOF

:BuildLib
IF [%2]==[] ECHO: ERROR: Library name was NOT specified! && GOTO:EOF

SET libsfolder=%source%\libs
IF NOT EXIST "%libsfolder%\%2" ECHO: ERROR: Library %2 was NOT found in %libsfolder% folder! && GOTO:EOF

ECHO: Building %2 library...

IF NOT EXIST "%bin%" MKDIR "%bin%"
IF NOT EXIST "%bin%\%2" MKDIR "%bin%\%2"

PUSHD "%bin%\%2"
"%compiler%" %libargs% ..\%libsfolder%\%2\main.c
"%libmng%" /NOLOGO /OUT:%2.lib main.obj
COPY %2.lib "..\"
POPD

ECHO: Done!
GOTO:EOF

:BuildHook
ECHO: Building hook...

IF NOT EXIST "%bin%" MKDIR "%bin%"
PUSHD "%bin%"
"%compiler%" %hookargs%
POPD

COPY "x3daudio.ini" "%bin%"
GOTO:EOF

:SetProjectName
IF [%2]==[] ECHO: ERROR: Name for a project was NOT specified! && GOTO:EOF

ECHO: Changing project name to %2...
ENDLOCAL
SET project=%2
ECHO: Done!
GOTO:EOF

:EditBuildFile
"%editor%" "%tools%\%~n0.bat"
GOTO:EOF

:Error
ECHO: ERROR: wrong arguments passed!
GOTO:EOF
