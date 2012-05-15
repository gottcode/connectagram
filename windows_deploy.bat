@ECHO OFF
ECHO Copying libraries
MKDIR Connectagram
COPY release\Connectagram.exe Connectagram
COPY %QTDIR%\bin\mingwm10.dll Connectagram
COPY %QTDIR%\bin\QtCore4.dll Connectagram
COPY %QTDIR%\bin\QtGui4.dll Connectagram
COPY %QTDIR%\bin\QtNetwork4.dll Connectagram
COPY share\connectagram\words Connectagram
