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


; ITIDLLC.ASM
; Mark Hampton
; Copyright (C) 1991 Info Tech, Inc.
;
; This file provides initialization code for DLLs that use the 
; C run time library.  This module calls a function called 
; DLLINIT.  The function prototype for this function MUST
; BE AS FOLLOWS:
;
; BOOL DLLINITPROC DllInit (HMODULE hmodModule);
;
; If your DLL does not use any C run time code, you must use 
; ITIDLLI.ASM instead of this version, or you will not be able to link.
;
; You will need the following command in you makefile to assemble this file:
; 
; itidllc.obj: itidllc.asm
;     masm itidllc.asm;
;
; (NOTE: The semicolon on the MASM line is required)
;
; When linking, add itidllc.obj and crtdll_i.obj to your link line or 
; to your link response file.  dllinit.obj is part of the standard
; Microsoft 6.00 distribution, and is required to properly initialize
; the C run time library.
;
        extrn   DLLINIT:FAR
        extrn   C_INIT:FAR

        assume  CS:_TEXT

_TEXT   segment byte public 'CODE'

ITIDLLC proc    far

; put the handle to the module on the stack -- ASSUMES PASCAL CALLING!!!
        push    AX
        call    C_INIT
        call    DLLINIT

        ret
ITIDLLC endp

_TEXT   ends

        end     ITIDLLC

