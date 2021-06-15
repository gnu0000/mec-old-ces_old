@rem Make sure to make the link libraries first!
cd asm
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd itibase
nmake -nologo %1 %2 ..\lib\itibase.lib
if errorlevel 1 goto end
cd ..
cd iticrt
nmake -nologo %1 %2 ..\lib\iticrt.lib
if errorlevel 1 goto end
cd ..
cd itiutil
nmake -nologo %1 %2 ..\lib\itiutil.lib
if errorlevel 1 goto end
cd ..
cd itidbase
nmake -nologo %1 %2 ..\lib\itidbase.lib
if errorlevel 1 goto end
cd ..
cd itilock
nmake -nologo %1 %2 ..\lib\itilock.lib
if errorlevel 1 goto end
cd ..
cd itierror
nmake -nologo %1 %2 ..\lib\itierror.lib
if errorlevel 1 goto end
cd ..
cd itiglob
nmake -nologo %1 %2 ..\lib\itiglob.lib
if errorlevel 1 goto end
cd ..
cd itimbase
nmake -nologo %1 %2 ..\lib\itimbase.lib
if errorlevel 1 goto end
cd ..
cd itimenu
nmake -nologo %1 %2 ..\lib\itimenu.lib
if errorlevel 1 goto end
cd ..
cd itifmt
nmake -nologo %1 %2 ..\lib\itifmt.lib
if errorlevel 1 goto end
cd ..
cd itiwin
nmake -nologo %1 %2 ..\lib\itiwin.lib
if errorlevel 1 goto end
cd ..
cd buildres
nmake -nologo %1 %2 
if errorlevel 1 goto end
cd ..
cd colid
nmake -nologo %1 %2 ..\lib\colid.lib
if errorlevel 1 goto end
cd ..
rem cd itiipc
rem nmake -nologo %1 %2 ..\lib\itiipc.lib
rem if errorlevel 1 goto end
rem cd ..
cd itiest
nmake -nologo %1 %2 ..\lib\itiest.lib
if errorlevel 1 goto end
cd ..
cd itiimp
nmake -nologo %1 %2 ..\lib\itiimp.lib
if errorlevel 1 goto end
cd ..
cd average
nmake -nologo %1 %2 ..\lib\average.lib
if errorlevel 1 goto end
cd ..
cd default
nmake -nologo %1 %2 ..\lib\default.lib
if errorlevel 1 goto end
cd ..
cd regress
nmake -nologo %1 %2 ..\lib\regress.lib
if errorlevel 1 goto end
cd ..
cd cesutil
nmake -nologo %1 %2 ..\lib\cesutil.lib
if errorlevel 1 goto end
cd ..

:end
