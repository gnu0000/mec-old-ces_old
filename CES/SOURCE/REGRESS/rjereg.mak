REPORT_1=rJEReg               

!include "..\makeopts" 

all: 
   premake $(REPORT_1).pre $(REPORT_1)       
   $(MAKE) -nologo -f $(REPORT_1)          
