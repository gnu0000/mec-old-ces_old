# splitcsv.exe makefile

!include "..\makeopts"

TARGETNAME=splitcsv

#$(EXEDIR)$(TARGETNAME).exe: splitcsv.c splitcsv.def 
#   premake /AddDot $(TARGETNAME).pre $(TARGETNAME)
#   $(MAKE) -nologo -f $(TARGETNAME)

# DEBUG = TRUE
# 
!IF "$(DEBUG)" == "TRUE"
COPT = /c /W3 /AL /Od /Zip /nologo
LOPT = /CO/NOLOGO/NOD/NOE
LIBS = os2 llibcep GnuLibD
BIND = rem
!ELSE
COPT = /c /W3 /AL /Ox /Grs /Zp /nologo
LOPT = /E/F/PACKC/NOLOGO/NOD/NOE
LIBS = os2 llibcep GnuLibO
BIND = bind $*.exe
!ENDIF
 
 
all : $(TARGETNAME).exe

$(TARGETNAME).obj : $(TARGETNAME).c  
  cl $(COPT) $*.c

$(TARGETNAME).exe : $(TARGETNAME).obj
  link $* $(LOPT),,NUL,$(LIBS),$*.def
  $(BIND)
  copy $(TARGETNAME).exe $(EXEDIR)
