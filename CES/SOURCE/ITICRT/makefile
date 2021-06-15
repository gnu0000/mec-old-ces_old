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


!include "..\makeopts"

all: $(LIBDIR)iticrt.lib $(DLLDIR)iticrt.dll

$(LIBDIR)iticrt.lib: iticrt.def
   implib /nologo $(LIBDIR)iticrt.lib iticrt.def
   lib $(LIBDIR)iticrt.lib /nologo +cdllsupp.lib;
   del $(LIBDIR)iticrt.bak


$(DLLDIR)iticrt.dll: iticrt.def
   link /nologo crtlib.obj,$(DLLDIR)iticrt.dll/NOI,,cdllobjs os2/nod/noe,iticrt.def
