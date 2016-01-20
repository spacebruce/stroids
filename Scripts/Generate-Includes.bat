@ECHO #pragma once > %1.h
@ECHO. >> %1.h
@FOR /F %%G IN ('@DIR "%1\*.h" /B') DO @ECHO #include "%~n1/%%~nxG" >> %1.h
