RPT_TARGET=rAreas               

!include "..\makeopts" 

all:
   premake $(RPT_TARGET).pre $(RPT_TARGET)       
   $(MAKE) -nologo -f $(RPT_TARGET)          
