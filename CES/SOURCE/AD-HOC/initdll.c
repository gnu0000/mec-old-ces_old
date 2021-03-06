/*
 * InitDll.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This is an almost generic DLL initialization module.  The function 
 * DllInit is called by either the ITIDLLI.OBJ or ITIDLLC.OBJ module.
 * 
 * Use the ITIDLLI.OBJ module when your DLL does not use the C run time
 * library.  Use the ITIDLLC.OBJ module when you need to use the C run
 * time library, and you need to initialize your DLL.  Note that when
 * you do this, you will have to link with CRTDLL_I.OBJ, a standard 
 * Microsoft module.
 *
 * Note: Do not rename this file to DLLINIT, since this file name
 * is already used by Microsoft for C run time library initialization.
 */

#include "..\include\iti.h"
#include "initdll.h"


HMODULE hmod;


/*
 * DllInit is called by either ITIDLLI.OBJ or ITIDLLC.OBJ.  This functino
 * must return non-zero if the initialization succeded, or FALSE
 * if initialization failed.
 */

BOOL DLLINITPROC DllInit (HMODULE hmodDll)
   {
   /* call your module initialization functions here. */
   hmod = hmodDll;
   return TRUE;
   }


