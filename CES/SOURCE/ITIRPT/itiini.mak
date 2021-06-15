# ItiIni.exe makefile

!include "..\makeopts"

TARGETNAME=ItiIni

$(EXEDIR)$(TARGETNAME).exe: *.c *.def ..\itirptut\*.c ..\itiprt\*.c
   premake $(TARGETNAME).pre $(TARGETNAME)
   $(MAKE) -nologo -f $(TARGETNAME)

