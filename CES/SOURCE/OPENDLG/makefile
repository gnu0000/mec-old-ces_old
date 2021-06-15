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


# Makefile for OPENDLG.DLL
# Created by Microsoft Corporation, 1989
#

# Abbreviations
#
ASM   = masm
CC = cl -Zip -W3 -c -u -J -Asnw -G2sc -Od -NT
LF = /far /align:16 /map /nod /codeview
OBJS  = tool.obj tool1.obj toola.obj data.obj\
     file.obj file1.obj init.obj gpi.obj

# Inference Rules
#

all:  opendlg.lib opendlg.dll ..\lib\opendlg.lib ..\exe\opendlg.dll

.asm.obj:
    $(ASM) $*.asm;

.c.obj:
    $(CC) _TEXT $*.c

# Resources
#
opendlg.res:   makefile opendlg.dlg opendlg.h opendlg.rc tool.h
    rc -r opendlg.rc

# Import Libraries
#
opendlg.lib:   makefile opendlg.def
    implib $*.lib $*.def

# MASM files
#
toola.obj:  makefile toola.asm

# C files
#
tool.obj:   makefile tool.c

data.obj:   makefile data.c
    $(CC) _INIT data.c

gpi.obj: makefile gpi.c
    $(CC) _INIT gpi.c

tool1.obj:  makefile tool1.c
    $(CC) _INIT tool1.c

init.obj:   makefile init.c
    $(CC) _INIT init.c

file.obj:   makefile file.c
    $(CC) _FILE file.c

file1.obj:  makefile file1.c
    $(CC) _FILE file1.c

# Main files:  OPENDLG.DLL
#
opendlg.lnk:   makefile
    echo $(OBJS)  >  opendlg.lnk
    echo opendlg.dll >> opendlg.lnk
    echo opendlg.map >> opendlg.lnk
    echo os2            >> opendlg.lnk
    echo opendlg.def >> opendlg.lnk

opendlg.dll:   makefile opendlg.def opendlg.res opendlg.lnk $(OBJS)
    link $(LF) @opendlg.lnk
    rc opendlg.res opendlg.dll
    del opendlg.lnk

# Files used by other apps
#

..\lib\opendlg.lib:  makefile opendlg.lib
    copy opendlg.lib ..\lib
    del opendlg.lib

..\exe\opendlg.dll:  makefile opendlg.dll
    copy opendlg.dll ..\exe
    del opendlg.dll
