rem @echo off
echo GenBIOS.bat
echo %WROKSPACE%
find /v "#" BraswellPlatformPkg\%BIOS_ID_FILE% > string
for /f "tokens=1,2,3" %%i in (string) do set %%i=%%k
set SPIROM=Build\%PLATFORM_PACKAGE%\%TARGET%_%TOOL_CHAIN_TAG%\ROM
copy /b /y %SPIROM%\bios.rom %SPIROM%\%BOARD_ID%_%1_%BUILD_TYPE%_%VERSION_MAJOR%_%VERSION_MINOR%.ROM
copy /b /y %SPIROM%\%BOARD_ID%_%1_%BUILD_TYPE%_%VERSION_MAJOR%_%VERSION_MINOR%.ROM Stitch
if exist %SPIROM%\bios.rom del %SPIROM%\bios.rom
if exist %WORKSPACE%\string del string

