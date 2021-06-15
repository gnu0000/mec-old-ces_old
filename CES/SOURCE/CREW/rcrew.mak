TARGET=rCrew               

!include "..\makeopts" 
all:
   premake $(TARGET).pre $(TARGET)       
   $(MAKE) -nologo -f $(TARGET)          
