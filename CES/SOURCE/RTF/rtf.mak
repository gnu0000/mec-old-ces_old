# csvrep.exe makefile

!include "..\makeopts"

TARGETNAME=csvrep

$(EXEDIR)$(TARGETNAME).exe: *.c *.def 
   premake /AddDot $(TARGETNAME).pre $(TARGETNAME)
   $(MAKE) -nologo -f $(TARGETNAME)

