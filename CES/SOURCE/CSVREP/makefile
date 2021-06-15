# csvrep.exe makefile

!include "..\makeopts"

TARGETNAME=csvrep

$(EXEDIR)$(TARGETNAME).exe: csvrep.c csvrep.def 
   premake /AddDot $(TARGETNAME).pre $(TARGETNAME)
   $(MAKE) -nologo -f $(TARGETNAME)

