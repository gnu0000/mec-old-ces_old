REPORT_0=rCstSh               

!include "..\makeopts" 

all:
   premake $(REPORT_0).pre $(REPORT_0)       
   $(MAKE) -nologo -f $(REPORT_0)          
