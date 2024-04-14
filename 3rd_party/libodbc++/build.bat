if "%1" == "debug" (
	set CFG=debug-dll
	set SRC_FILE=odbc++d.dll
	set DST_DIR=Debug
)

if "%1" == "release" (
	set CFG=prod-dll
	set SRC_FILE=odbc++.dll
	set DST_DIR=Release
)
set prevcd=%cd%
cd "../../../../3rd_party/libodbc++-0.2.3/win32"
if "%2" == "build" (
call :build
)

if "%2" == "clean" (
call :clean
)

if "%2" == "rebuild" (
call :clean
call :build
)
cd %prevcd%

exit /b

rem =====================================================
:build

nmake -f "Makefile.w32" cfg=%CFG%
copy ".\%CFG%\%SRC_FILE%" "..\..\..\DVR\BIN\%DST_DIR%" /Y

exit /b

rem =====================================================
:clean

nmake -f "Makefile.w32" cfg=%CFG% clean
del "..\..\..\DVR\BIN\%DST_DIR%\%SRC_FILE%"

exit /b