set QT_LOC="C:\Qt\Qt5.1.0\5.1.0\mingw48_32\bin"
set BENDER_PLAYER="bin - Bender"
set FRY_PLAYER="bin - Fry"
set ZOIDBERG_PLAYER="bin - Zoidberg"

mkdir %BENDER_PLAYER%
mkdir %FRY_PLAYER%
mkdir %ZOIDBERG_PLAYER%

for %%v in (%BENDER_PLAYER% %FRY_PLAYER% %ZOIDBERG_PLAYER%) do
	copy /Y %QT_LOC%/Qt5CLucened.dll %BENDER_PLAYER%
	
	
Qt5Scriptd.dll
libstdc++-6.dll
Qt5Cored.dll
Qt5Sqld.dll
libwinpthread-1.dll
Qt5Guid.dll
Qt5Svgd.dll
icudt51.dll
Qt5Helpd.dll
Qt5Widgetsd.dll
icuin51.dll
Qt5Networkd.dll
Qt5Xmld.dll
icuuc51.dll
Qt5ScriptToolsd.dll
libgcc_s_dw2-1.dll


