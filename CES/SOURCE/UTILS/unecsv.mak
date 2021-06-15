unecsv.exe : unecsv.obj makefile
     link /ST:32767 /NOD  unecsv,unecsv.exe,,slibcep os2,;
     bind unecsv.exe api.lib
     markexe WINDOWCOMPAT unecsv.exe
     markexe LFNS unecsv.exe

unecsv.obj : unecsv.c 
     cl -AS -c -W3  unecsv.c

