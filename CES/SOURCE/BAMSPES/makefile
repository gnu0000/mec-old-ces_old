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

TARGET=bamspes
REPORT_0=rPESpj 
REPORT_1=rPESjo               


all:
#   $(EXEDIR)$(TARGET).exe: *.c *.h *.bkm *.dlg *.rc *.bin
   premake /AddDot $(TARGET).pre $(TARGET)
   nmake -nologo -f $(TARGET)
   premake /AddDot $(REPORT_0).pre $(REPORT_0)       
   $(MAKE) -nologo -f $(REPORT_0)          
   premake /AddDot $(REPORT_1).pre $(REPORT_1)       
   $(MAKE) -nologo -f $(REPORT_1)          
