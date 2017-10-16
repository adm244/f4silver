@ECHO OFF
SETLOCAL
SET data=..\data
SET ship=..\ship

IF [%1]==[] GOTO Error
GOTO CreateShipVersion

:CreateShipVersion
SET fullshippath=%ship%\%1

ECHO: Creating shipping version in %fullshippath%

IF NOT EXIST "%ship%" MKDIR "%ship%"
IF NOT EXIST "%fullshippath%" MKDIR "%fullshippath%"

COPY "%bin%\X3DAudio1_7.dll" "%fullshippath%"
COPY "%bin%\f4silver.dll" "%fullshippath%"

COPY "%data%\*" "%fullshippath%"

ECHO: Done!
GOTO:EOF

:Error
ECHO: Enter a version number!
GOTO:EOF
