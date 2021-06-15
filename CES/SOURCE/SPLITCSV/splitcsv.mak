# splitcsv.exe makefile

!include "..\makeopts"

TARGETNAME=splitcsv

$(EXEDIR)$(TARGETNAME).exe: *.c *.def 
   premake /AddDot $(TARGETNAME).pre $(TARGETNAME)
   $(MAKE) -nologo -f $(TARGETNAME)

