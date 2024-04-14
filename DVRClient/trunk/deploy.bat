@echo off

rem Deploy.bat Debug or Release

@set DST_DIR=..\..\BIN\%1
@set DST_CLIENT_RES=%DST_DIR%\res\
@set DST_WDR_RES=%DST_CLIENT_RES%\DVRClient_wdr
@set FILE_UI=res\Generated\DVRClientUIStr.tmp
@set UTIL_DIR=..\..\utility

rem run transformation script
cscript //nologo //E:jscript Transform.js "res\DVRClient_wdr.xrc" "res\DVRClient.xrc" "%FILE_UI%"

@mkdir "%DST_CLIENT_RES%
cd res
..\..\..\utility\wxrc.exe DVRClient.xrc --output=..\%DST_CLIENT_RES%\DVRClient.xrs 
cd ..

rem create i18n template - extract strings from sources 
rem and concatinate it to the xrc extracted strings

set FILE_XGETTEXT_FILES=res\Generated\xgettext_files.tmp
set FILE_STRING=res\Generated\DVRClientSrcStr.tmp 
set FILE_FINAL_PO=res\Generated\DVRClient.tmp

dir /b /s *.h *.c *.cpp > %FILE_XGETTEXT_FILES%
%UTIL_DIR%\gettext\xgettext.exe --omit-header -o %FILE_STRING% --files-from=%FILE_XGETTEXT_FILES% --keyword=_
%UTIL_DIR%\gettext\msgcat.exe %FILE_UI% %FILE_STRING% -o %FILE_FINAL_PO%

del %FILE_XGETTEXT_FILES% /Q
del %FILE_STRING% /Q
del %FILE_UI% /Q

cmd /c deploy_i18n.bat "%DST_DIR%" ru "%FILE_FINAL_PO%"

del %FILE_FINAL_PO% /Q