/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by AASHTO.  All Rights Reserved.                 |
|                                                                           |
|       This program module is part of BAMS/CES, a module of BAMS,          |
|       The American Association of State Highway and Transportation        |
|       Officials' (AASHTO) Information System for Managing Transportation  |
|       Programs, a proprietary product of AASHTO, no part of which may be  |
|       reproduced or transmitted in any form or by any means, electronic,  |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from AASHTO.                  |
|                                                                           |
+--------------------------------------------------------------------------*/


/*
 * init.c
 * Mark Hampton
 */

#include "..\include\iti.h"
#include "..\include\itierror.h"
#include "..\include\itibase.h"
#include "..\include\itiglob.h"
#include "..\include\winid.h"
#include "..\include\itimbase.h"
#include "..\include\itidbase.h"
#include "..\include\itiwin.h"
#include <stdio.h>
#include "init.h"

/* globals */

HMODULE hmod;

PGLOBALS pglobals = NULL;

/*
 * DllInit is called by either ITIDLLI.OBJ or ITIDLLC.OBJ.  This function
 * must return non-zero if the initialization succeded, or FALSE
 * if initialization failed.
 */

BOOL DLLINITPROC DllInit (HMODULE hmodDll)
   {
   hmod = hmodDll;
   pglobals = ItiGlobQueryGlobalsPointer ();

   /* call your module initialization functions here. */
   return TRUE;
   }




