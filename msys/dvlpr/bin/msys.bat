@echo off
rem Copyright (C):  2001, 2002, 2003, 2004  Earnie Boyd
rem   mailto:earnie@users.sf.net
rem This file is part of Minimal SYStem
rem   http://www.mingw.org/msys.shtml
rem
rem File:	    msys.bat
rem Revision:	    2.2
rem Revision Date:  March 30th, 2004

rem ember to set the "Start in:" field of the shortcut.
rem A value similar to C:\msys\1.0\bin is what the "Start in:" field needs
rem to represent.

rem ember value of GOTO: is used to know recursion has happened.
if "%1" == "GOTO:" goto %2

if NOT "x%WD%" == "x" set WD=

rem ember command.com only uses the first eight characters of the label.
goto _WindowsNT

rem ember that we only execute here if we are in command.com.
:_Windows

set WD=%0\..\

if "x%COMSPEC%" == "x" set COMSPEC=command.com
start %COMSPEC% /e:4096 /c %0 GOTO: _Resume %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto EOF

rem ember that we execute here if we recursed.
:_Resume
for %%F in (1 2 3) do shift

rem ember that we get here even in command.com.
:_WindowsNT

if "x%WD%" == "x" set WD=%~dp0

rem ember Set up option to use rxvt based on value of %1
if "x%MSYSCON%" == "x" set MSYSCON=rxvt.exe
if "x%1" == "x-norxvt" set MSYSCON=sh.exe
if "x%1" == "x--norxvt" set MSYSCON=sh.exe
if "x%MSYSCON%" == "xsh.exe" shift

if "x%MSYSTEM%" == "x" set MSYSTEM=MINGW32
if "%1" == "MINGW32" set MSYSTEM=MINGW32
if "%1" == "MSYS" set MSYSTEM=MSYS

if NOT "x%DISPLAY%" == "x" set DISPLAY=

if "x%MSYSCON%" == "xrxvt.exe" goto startrxvt
if "x%MSYSCON%" == "xsh.exe" goto startsh

:unknowncon
echo %MSYSCON% is an unknown option for msys.bat.
pause
exit 1

:notfound
echo Cannot find the rxvt.exe or sh.exe binary -- aborting.
pause
exit 1

rem If you don't want to use rxvt then rename the file rxvt.exe to something
rem else.  Then sh.exe will be used instead.
:startrxvt
if NOT EXIST %WD%bin\rxvt.exe goto notfound

rem Setup the default colors for rxvt.
if "x%MSYSBGCOLOR%" == "x" set MSYSBGCOLOR=White
if "x%MSYSFGCOLOR%" == "x" set MSYSFGCOLOR=Black
if "x%MINGW32BGCOLOR%" == "x" set MINGW32BGCOLOR=LightYellow
if "x%MINGW32FGCOLOR%" == "x" set MINGW32FGCOLOR=Navy
if "%MSYSTEM%" == "MSYS" set BGCOLOR=%MSYSBGCOLOR%
if "%MSYSTEM%" == "MSYS" set FGCOLOR=%MSYSFGCOLOR%
if "%MSYSTEM%" == "MINGW32" set BGCOLOR=%MINGW32BGCOLOR%
if "%MSYSTEM%" == "MINGW32" set FGCOLOR=%MINGW32FGCOLOR%

start %WD%bin\rxvt -backspacekey  -sl 2500 -fg %FGCOLOR% -bg %BGCOLOR% -sr -fn Courier-12 -tn msys -geometry 80x25 -e /bin/sh --login -i
exit

:startsh
if NOT EXIST %WD%bin\sh.exe goto notfound
start %WD%bin\sh --login -i
exit

:EOF

rem ChangeLog:
rem 2002.03.07  Earnie Boyd  mailto:earnie@users.sf.net
rem	* Move the @echo off to the top.
rem	* Change the binmode setting to nobinmode.
rem     * Remove the angle brackets around email address to workaround MS 
rem	buggy command processor.
rem
rem 2002.03.12  Earnie Boyd  mailto:earnie@users.sf.net
rem	* Add filter logic to find rxvt.exe
rem
rem 2002.03.13  Earnie Boyd  mailto:earnie@users.sf.net
rem	* Revert the nobinmode change.
rem
rem 2002.03.20  Earnie Boyd  mailto:earnie@users.sf.net
rem     * Add logic for stating bash.
rem
rem 2002.04.11  Earnie Boyd  mailto;earnie@users.sf.net
rem	* Add logic for setting MSYSTEM value based on parameter.
rem
rem 2002.04.15  Olivier Gautherot  mailto:olivier_gautherot@mentorg.com
rem	* Reduce number test conditions for finding an executable.
rem
rem 2002.04.15  Earnie Boyd  mailto:earnie@users.sf.net
rem	* Unset DISPLAY if set before starting shell.
rem
rem 2002.04.16  Earnie Boyd  mailto:earnie@users.sf.net
rem	* Remove use of DEFINED in conditional statments for variables for
rem	command.com support.
rem	* Add check for nonexistance of USERNAME variable for Win9x support.
rem
rem 2002.04.17  Earnie Boyd  mailto:earnie@users.sf.net
rem	* Add foreground and background color defaults based on MSYSTEM value.
rem
rem 2002.04.22  Earnie Boyd  mailto:earnie@users.sf.net
rem	* More Win 9x changes.
rem
rem 2002.05.04  Earnie Boyd  mailto:earnie@users.sf.net
rem	* Remove the SET of USERNAME and HOME.
rem
rem 2002.11.18  Earnie Boyd  mailto:earnie@users.sf.net
rem	* Add command.com detection and restart with a larger environment to
rem	avoid errors on w9x.
rem     Many thanks to Randy W. Sims mailto:RandyS@ThePierianSpring.org.
rem	See Randy's response to "RE: [Mingw-msys] Installation on WindowsME" 
rem	from 11/06/2002 in the archives of mingw-msys@lists.sf.net.
rem
rem 2002.11.19  Paul Garceau  mailto:pgarceau@attbi.com
rem	* Fix a typo: Change COMPSPEC to COMSPEC.
rem
rem 2002.11.25  Earnie Boyd  mailto:earnie@users.sf.net
rem	* Remove the SET CYGWIN since it doesn't matter any longer.
rem
rem 2003.02.03  Earnie Boyd  mailto:earnie@users.sf.net
rem	* Win9x doesn't like ``EXISTS dir'' so change it to ``EXISTS dir\nul''.
rem	Thanks to Nicolas Weber mailto:nicolasweber@gmx.de.
rem
rem 2003.03.06  Earnie Boyd  mailto:earnie@users.sf.net
rem	* Add -backspacekey switch to rxvt startup.
rem	* Move RXVT color setup to startrxvt label
rem
rem 2004.01.30  Earnie Boyd  mailto:earnie@users.sf.net
rem	* Add -geometry parameter to work around an off by one issue with
rem       the default values.
rem	Thanks to Dave Schuyler mailto:parameter@users.sf.net
rem
rem 2004.03.30  Earnie Boyd  mailto:earnie@users.sf.net
rem	* Add -norxvt or --norxvt switch argument.
rem	Thanks to Keith Marshal mailto:Keith.Marshall@total.com.
rem	* Add method to determine absolute path of msys.bat so that we no
rem	longer need to change to the bin directory.  This allows msys.bat to be
rem	called from any working directory.
rem	Thanks to Kevin Mack  mailto:kevin.mack@us.cd-adapco.com
rem
