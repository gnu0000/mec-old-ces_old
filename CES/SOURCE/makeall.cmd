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

@rem make the asm stuff first, since it's need by the other modules for linking
cd asm
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
@rem make itiutil, since the buildres requires the dll
cd itiutil
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd itibase
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd iticrt
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd itierror
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd itiglob
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd colid
nmake -nologo %1 %2 
if errorlevel 1 goto end
cd ..
rem cd itiipc
rem nmake -nologo %1 %2
rem if errorlevel 1 goto end
rem cd ..

cd itimbase
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd itidbase
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd itimenu
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd itiwin
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd itifmt
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd itilock
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd tpee
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd itirptut
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd itiprt
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd itirpt
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd itiest
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd itiimp
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd cesutil
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
@rem make the catalogs
cd applicat
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd areatype
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd codetab
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd cbest
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd costsht
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd county
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd crew
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd equip
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd facility
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd fund
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd inflpred
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd labor
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd majitem
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd map
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd material
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd stditem
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd usercat
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd vendor
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd workday
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd zonetype
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd itimodel
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd itiperm
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd dssmodel
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd opendlg
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd itimap
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..

@rem cd ces
@rem nmake -nologo %1 %2
@rem if errorlevel 1 goto end
@rem cd ..

cd dssrpt
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd dssimp
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd basedate
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
@rem make the installable estimation methods
cd average
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd default
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd regress
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd paramet
nmake -nologo %1 %2 
if errorlevel 1 goto end
cd ..
@rem make DS/Shell last, since it depends on itimbase and probably other nicities
cd sqlexec
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd cesinst
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd import
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd export
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd pesexp
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd dssitem
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd dssfacil
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd dssprop
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd sqlbrow
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
rem cd ipcserve
rem nmake -nologo %1 %2
rem if errorlevel 1 goto end
rem cd ..
rem cd pbxserve
rem nmake -nologo %1 %2
rem if errorlevel 1 goto end
rem cd ..
cd dsshell
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd bamspes
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd bamsdss
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd bamsces
nmake -nologo %1 %2
if errorlevel 1 goto end
cd ..
cd exe
call pak.cmd
cd ..
:end
