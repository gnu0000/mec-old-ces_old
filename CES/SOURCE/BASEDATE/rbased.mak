TARGET=rBased

!include "..\makeopts" 

all:
   premake $(TARGET).pre $(TARGET)       
   nmake -nologo -f $(TARGET)          
