# rtf.exe makefile

!include "..\makeopts"

TARGETNAME=rtf

$(EXEDIR)$(TARGETNAME).exe: rtf.c rtf.def 
   premake /AddDot $(TARGETNAME).pre $(TARGETNAME)
   $(MAKE) -nologo -f $(TARGETNAME)

