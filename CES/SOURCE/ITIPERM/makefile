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

TARGET=itiperm

all: $(EXEDIR)$(TARGET).dll $(LIBDIR)$(TARGET).lib

$(EXEDIR)$(TARGET).dll: *.c *.h ..\include\*.h $(TARGET).def *.pre $(TARGET).rc $(TARGET).dlg $(TARGET).met
   premake /AddDot $(TARGET).pre $(TARGET)
   nmake -nologo -f $(TARGET)

$(LIBDIR)$(TARGET).lib: $(TARGET).def *.pre 
   premake /AddDot $(TARGET).pre $(TARGET)
   nmake -nologo -f $(TARGET) $(LIBDIR)$(TARGET).lib
