set SRC_BASE_DIR=../../../../3rd_party/wxWidgets-2.7.0
set DST_BASE_DIR=..\..\BIN

if "%1" == "debug" (
	set SRC_CFG_FILE=config.debug.dll.vc
	set DST_DIR=%DST_BASE_DIR%\Debug
	set DST_SUFFIX=d
) else if "%1" == "release" (
	set SRC_CFG_FILE=config.release.dll.vc
	set DST_DIR=%DST_BASE_DIR%\Release
	set DST_SUFFIX=
) else (
	exit /b
)

set prevcd=%cd%
cd "%SRC_BASE_DIR%/build/msw"

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

copy %SRC_CFG_FILE% "config.vc"
nmake -f "makefile.vc"
cd %prevcd%
copy "%SRC_BASE_DIR%\lib\vc_dll\wxmsw270%DST_SUFFIX%_vc_custom.dll" 			"%DST_DIR%" /Y
copy "%SRC_BASE_DIR%\lib\vc_dll\wxmsw270%DST_SUFFIX%_gl_vc_custom.dll" 		"%DST_DIR%" /Y

exit /b

rem =====================================================
:clean

nmake -f "makefile.vc" clean
rem del "%DST_DIR%\lib\vc_dll\wxmsw270%DST_SUFFIX%_vc_custom.dll" 			
rem del "%DST_DIR%\lib\vc_dll\wxmsw270%DST_SUFFIX%_gl_vc_custom.dll" 		

exit /b