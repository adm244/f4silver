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

SET debug=/Od /Zi /Fe%project% /nologo /EHsc
SET release=/W2 /Ot /Oi /O2 /WX /Fe%project% /nologo /EHsc
SET args=%debug% %files% /LD /link %objfiles% %libs%

SET includepath=C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include
SET hookargs=/I "%includepath%" /W2 /O2 /WX /nologo /EHsc /Fe%hookname% %hookfile% /LD /link /DEF:%deffile% %libs%

SET compiler=CL
SET masm=ML64
REM ###########################

SET edit=edit
SET setprjname=setname
SET hook=hook

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

REM COPY "mwsilver.ini" "%bin%"
ECHO: Build finished.
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
