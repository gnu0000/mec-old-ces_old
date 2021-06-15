#+--------------------------------------------------------------------------+
#|                                                                          |
#|       Copyright (c) 1992 by Info Tech, Inc.  All Rights Reserved.        |
#|                                                                          |
#|       This program module is part of DS/Shell (PC), Info Tech's          |
#|       database interfaces for OS/2, a proprietary product of             |
#|       Info Tech, Inc., no part of which may be reproduced or             |
#|       transmitted in any form or by any means, electronic,               |
#|       mechanical, or otherwise, including photocopying and recording     |
#|       or in connection with any information storage or retrieval         |
#|       system, without permission in writing from Info Tech, Inc.         |
#|                                                                          |
#+--------------------------------------------------------------------------+


!INCLUDE "..\makeopts"
TARGET=itiglob
TARGETDIR=..\itiglob^\

all: $(DLLDIR)$(TARGET).dll $(LIBDIR)$(TARGET).lib

$(DLLDIR)$(TARGET).dll: $(TARGET).obj $(TARGET).def
   $(LINK) $(LFLAGS)$(LDEBUG)/NOI/NOD/NOE crtdll.obj $(TARGET),$*.dll,nul.map,$(LIBDIR)itiutil $(LIBDIR)itierror os2 $(LIBDIR)iticrt,$(TARGET).def

$(TARGET).obj: $(TARGET).c $(INCLUDEDIR)*.h 
   $(CC) $(CSDLLFLAGS) $(CDEBUG) $(TARGETDIR)$*.c


$(LIBDIR)$(TARGET).lib: $(TARGET).def
   implib -nologo $*.lib $(TARGET).def
