;+--------------------------------------------------------------------------+
;|                                                                          |
;|       Copyright (c) 1992 by Info Tech, Inc.  All Rights Reserved.        |
;|                                                                          |
;|       This program module is part of DS/Shell (PC), Info Tech's          |
;|       database interfaces for OS/2, a proprietary product of             |
;|       Info Tech, Inc., no part of which may be reproduced or             |
;|       transmitted in any form or by any means, electronic,               |
;|       mechanical, or otherwise, including photocopying and recording     |
;|       or in connection with any information storage or retrieval         |
;|       system, without permission in writing from Info Tech, Inc.         |
;|                                                                          |
;+--------------------------------------------------------------------------+


; ITIDLLI.ASM
; Mark Hampton
; Copyright (C) 1991 Info Tech, Inc.
;
; This file provides initialization code for DLLs that do not
; use the C run time library.  This module calls a function
; called DLLINIT.  The function prototype for this function MUST
; BE AS FOLLOWS:
;
; BOOL DLLINITPROC DllInit (HMODULE hmodModule);
;
; If your DLL uses any C run time code, you must use ITIDLLC.ASM instead
; of this version, or the C run time library WILL NOT WORK.
;
;
; You will need the following command in you makefile to assemble this file:
; 
; itidlli.obj: itidlli.asm
;     masm itidlli.asm;
;
; (NOTE: The semicolon on the MASM line is required)
;
; When linking, add itidlli.obj to your link line or to your link response
; file.
;
        extrn   DLLINIT:FAR

        assume  CS:_TEXT

_TEXT   segment byte public 'CODE'

ITIDLLI proc    far

; put the handle to the module on the stack -- ASSUMES PASCAL CALLING!!!
        push    AX
        call    DLLINIT

        ret
ITIDLLI endp

_TEXT   ends

        end     ITIDLLI

