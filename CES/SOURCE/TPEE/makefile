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


all: ..\exe\tpee.exe

tpee.exe: tpee.obj errorcod.obj arg.obj
   link /co/nod/noe $**,$*.exe,nul,os2 slibcep;
   cvpack /p $*.exe
   markexe windowcompat $*.exe
   markexe lfns $*.exe

..\exe\tpee.exe: tpee.exe
   copy tpee.exe ..\exe

.c.obj:
   cl -c -Od -W3 -Zip -AS $*.c