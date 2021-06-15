REPORT_2=rSICbe 
!include "..\makeopts" 
all:   
   premake $(REPORT_2).pre $(REPORT_2) 
   $(MAKE) -nologo -f $(REPORT_2)    
