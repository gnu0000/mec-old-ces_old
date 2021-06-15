# itirmath.exe makefile

!include "..\makeopts"

TARGETNAME=itirmath

$(EXEDIR)$(TARGETNAME).exe: *.c *.def ..\include\iti*.h
   premake $(TARGETNAME).pre $(TARGETNAME)
   $(MAKE) -nologo -f $(TARGETNAME)

   
