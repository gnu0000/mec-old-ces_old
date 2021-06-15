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
TARGET=itiest

all: $(EXEDIR)$(TARGET).dll $(LIBDIR)$(TARGET).lib

$(EXEDIR)$(TARGET).dll: *.c *.h ..\include\*.h *.pre *.def 
   premake /AddDot $(TARGET).pre $(TARGET)
   $(MAKE) -nologo -f $(TARGET)

$(LIBDIR)$(TARGET).lib: $(TARGET).def
   premake /AddDot $(TARGET).pre $(TARGET)
   $(MAKE) -nologo -f $(TARGET) $*.lib
