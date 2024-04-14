@echo off

set UTIL_DIR=..\..\utility
set DST_DIR=%1
set LOCALE_NAME=%2
set TEMPLATE_FILE_NAME=%3

@mkdir "%DST_DIR%\res\%LOCALE_NAME%"
%UTIL_DIR%\gettext\msgmerge "res\Generated\%LOCALE_NAME%\DVRClient.po" %TEMPLATE_FILE_NAME% -U
%UTIL_DIR%\gettext\msgfmt "res\Generated\%LOCALE_NAME%\DVRClient.po" -o "res\Generated\%LOCALE_NAME%\DVRClient.mo"
copy "res\Generated\%LOCALE_NAME%\DVRClient.mo" "%DST_DIR%\res\%LOCALE_NAME%\DVRClient.mo"