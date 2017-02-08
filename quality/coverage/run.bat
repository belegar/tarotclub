
echo off

set TESTS_ROOT=%CD%\..\..\build\tests

perl coverage.pl %TESTS_ROOT%

pause
