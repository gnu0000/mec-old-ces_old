REPORT=rLabor               
!include "..\makeopts"
 
all:
   premake $(REPORT).pre $(REPORT)       
   $(MAKE) -nologo -f $(REPORT)          
