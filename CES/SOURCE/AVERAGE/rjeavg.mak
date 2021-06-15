RPT_2=rJEAvg               

!include "..\makeopts" 

all:
   premake $(RPT_2).pre $(RPT_2)       
   $(MAKE) -nologo -f $(RPT_2)          
