TARGET=rProp               

!include "..\makeopts" 

all: $(EXEDIR)$(TARGET).dll   $(LIBDIR)itiprt.lib  $(LIBDIR)ItiRptUt.lib   

$(EXEDIR)$(TARGET).dll: *.c *.h *.pre ..\include\itiprt.h ..\include\ItiRptUt.h ..\include\ItiRpt.h  
   premake $(TARGET).pre $(TARGET)       
   $(MAKE) -nologo -f $(TARGET)          
