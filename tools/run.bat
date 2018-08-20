@ECHO OFF
SET gamepath=%F4PATH%
SET gameexe=Fallout4.exe
SET injectorname=inject.bat

PUSHD "%gamepath%"
START "" "%gamepath%\%gameexe%" %*
POPD

PUSHD "%bin%"
CALL %injectorname%
POPD
