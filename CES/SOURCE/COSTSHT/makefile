#+--------------------------------------------------------------------------+
#|                                                                          |
#|       Copyright (c) 1992 by AASHTO.  All Rights Reserved.                |
#|                                                                          |
#|       This program module is part of BAMS/CES, a module of BAMS,         |
#|       The American Association of State Highway and Transportation       |
#|       Officials' (AASHTO) Information System for Managing Transportation |
#|       Programs, a proprietary product of AASHTO, no part of which may be |
#|       reproduced or transmitted in any form or by any means, electronic, |
#|       mechanical, or otherwise, including photocopying and recording     |
#|       or in connection with any information storage or retrieval         |
#|       system, without permission in writing from AASHTO.                 |
#|                                                                          |
#+--------------------------------------------------------------------------+


!include "..\makeopts"

# Set target to your module name
TARGET=CostSht
REPORT_0=rCstSh               
REPORT_1=rJECbe               
REPORT_2=rSICbe 

all: 
   premake /AddDot $(TARGET).pre $(TARGET)
   $(MAKE) -nologo -f $(TARGET)
   premake /AddDot $(REPORT_1).pre $(REPORT_1)       
   $(MAKE) -nologo -f $(REPORT_1)          
   premake /AddDot $(REPORT_0).pre $(REPORT_0)       
   $(MAKE) -nologo -f $(REPORT_0)          
   premake /AddDot $(REPORT_2).pre $(REPORT_2) 
   $(MAKE) -nologo -f $(REPORT_2)    
