@ECHO OFF
SET injector=tools\injector
SET injectorpath=%injector%\injector.exe
SET procname=Fallout4.exe
SET hookname=f4silver.dll

PUSHD "%bin%"
"%injectorpath%" "%procname%" "%cd%\%hookname%"
POPD
