REPORT_=rSIReg 
!include "..\makeopts" 
all:   
   premake $(REPORT_).pre $(REPORT_) 
   $(MAKE) -nologo -f $(REPORT_)    
