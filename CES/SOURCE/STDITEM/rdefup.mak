TARGET2=rDefUP               


!include "..\makeopts" 

all: $(EXEDIR)$(TARGET2).dll  $(LIBDIR)itiprt.lib  $(LIBDIR)ItiRptUt.lib   
   
$(EXEDIR)$(TARGET2).dll: *.c *.h *.pre ..\include\itiprt.h ..\include\ItiRptUt.h ..\include\ItiRpt.h  
   premake $(TARGET2).pre $(TARGET2)       
   $(MAKE) -nologo -f $(TARGET2)          

