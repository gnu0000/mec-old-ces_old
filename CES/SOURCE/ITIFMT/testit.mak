
testit.exe: testit.c ..\include\*.h 
   cl -W3 -Od -ALw -G2sc -D_DLL -D_MT -Zip -FPi -nologo testit.c crtexe.obj -link /noe/nod/noi os2.lib iticrt.lib itifmt.lib itibase.lib

   

