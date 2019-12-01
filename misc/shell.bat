@echo off
call "%VC_VARS_PATH%\vcvarsall.bat" x64
set path=%CD%\misc;%path%
