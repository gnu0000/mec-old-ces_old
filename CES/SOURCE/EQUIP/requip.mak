!include "..\makeopts" 

RPT_1=rEquip               


all: $(EXEDIR)$(RPT_1).dll $(LIBDIR)itiprt.lib  $(LIBDIR)ItiRptUt.lib   

$(EXEDIR)$(RPT_1).dll: *.c *.h *.pre ..\include\itiprt.h ..\include\ItiRptUt.h ..\include\ItiRpt.h  
   premake $(RPT_1).pre $(RPT_1)       
   $(MAKE) -nologo -f $(RPT_1)          
