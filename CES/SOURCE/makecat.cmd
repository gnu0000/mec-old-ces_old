@echo off
cd applicat
nmake -nologo %1
if errorlevel 1 goto end

cd ..
cd areatype
nmake -nologo %1
if errorlevel 1 goto end

cd ..
cd average
nmake -nologo %1
if errorlevel 1 goto end

cd ..
cd codetab
nmake -nologo %1
if errorlevel 1 goto end

cd ..
cd costsht
nmake -nologo %1
if errorlevel 1 goto end

cd ..
cd county
nmake -nologo %1
if errorlevel 1 goto end

cd ..
cd crew
nmake -nologo %1
if errorlevel 1 goto end

cd ..
cd default
nmake -nologo %1
if errorlevel 1 goto end

cd ..
cd equip
nmake -nologo %1
if errorlevel 1 goto end

cd ..
cd facility
nmake -nologo %1
if errorlevel 1 goto end

cd ..
cd fund
nmake -nologo %1
if errorlevel 1 goto end

cd ..
cd inflpred
nmake -nologo %1
if errorlevel 1 goto end

cd ..
cd labor
nmake -nologo %1
if errorlevel 1 goto end

cd ..
cd majitem
nmake -nologo %1
end
if errorlevel 1 goto end

cd ..
cd map
nmake -nologo %1
if errorlevel 1 goto end

cd ..
cd material
nmake -nologo %1
if errorlevel 1 goto end

cd ..
cd stditem
nmake -nologo %1
if errorlevel 1 goto end

cd ..
cd user
nmake -nologo %1
if errorlevel 1 goto end

cd ..
cd regress
nmake -nologo %1
if errorlevel 1 goto end

cd ..
cd vendor
nmake -nologo %1
if errorlevel 1 goto end

cd ..
cd workday
nmake -nologo %1
if errorlevel 1 goto end

cd ..
cd zonetype
nmake -nologo %1
if errorlevel 1 goto end

:end
cd ..
