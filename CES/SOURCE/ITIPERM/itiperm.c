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
 * itiperm.c
 * Mark Hampton
 */

#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itierror.h"
#include "..\include\itibase.h"
#include "..\include\itiperm.h"
#include "..\include\itiglob.h"
#include "..\include\itiwin.h"
#include "..\include\winid.h"
#include "..\include\itiutil.h"
#include "..\include\cesutil.h"
#include "init.h"
#include <stdio.h>
#include "..\include\dialog.h"
#include "dialog.h"
#include "catalog.h"
#include "job.h"


/*
 * returns TRUE if the current user is a super user 
 */

BOOL EXPORT ItiPermIsSuperUser (void)
   {
   return pglobals->usUserType >= USER_SUPER;
   }


/*
 * returns TRUE if the current user can edit the current window
 */

BOOL EXPORT ItiPermQueryEdit (HWND hwnd)
   {
   USHORT usWindowID;

   if (hwnd == NULL)
      return FALSE;

   usWindowID = (UM) QW (hwnd, ITIWND_ID, 0, 0, 0);

   if (ItiCesIsCatalogWindow (hwnd, 0))
      return CanEditCatalog (usWindowID);

   if (ItiCesIsJobWindow (hwnd, 0))
      return CanEditJob (hwnd, TRUE);

   if (ItiCesIsProgramWindow (hwnd, 0))
      return CanEditJob (hwnd, FALSE);

   /* a bad idea */
   return TRUE;
   }



