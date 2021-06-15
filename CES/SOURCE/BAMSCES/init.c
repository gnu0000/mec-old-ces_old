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
 * Init.c
 * Mark Hampton
 */


#define INCL_WIN
#define INCL_DOS
#include <stdio.h>
#include "..\include\iti.h"
#include "bamsces.h"
#include "init.h"
#include "dialog.h"
#include "..\include\dialog.h"
#include "..\include\message.h"
#include "..\include\itiwin.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itibase.h"
#include "help.h"
#include "menu.h"
#include "..\include\winid.h"
#include "open.h"
#include "..\include\itiperm.h"
#include "..\include\itiutil.h"

/* include references to job window procedures */
#include "job.h"
#include "jobbreak.h"
#include "jobitem.h"
#include "itemschd.h"
#include "jobuser.h"
#include "jobfund.h"
#include "jobalt.h"
#include "jobcom.h"
#include "defuser.h"
#include "basedate.h"

/* include references to program window procedures */
#include "program.h"
#include "progjob.h"
#include "proguser.h"
#include "progfund.h"

#include "copy.h"

/* include references to DLLs */
#include "..\include\itierror.h"
#include "..\include\itiglob.h"
#include "..\include\itimenu.h"
#include "..\include\itifmt.h"
#include "..\include\itiest.h"

#include "specyear.h"


/* define global variables */
PGLOBALS  pglobals  = NULL;

CHAR szCurSpecYear[8];

CHAR szState[60] = "";
PSZ  pszState, pszChkState;
BOOL bDoCBEFirst;

// HBUF     hbufSIC  = NULL; /* StandardItem Catalog */
// HBUF     hbufSIAC = NULL; /* StandardItem Avg. Catalog */
// HBUF     hbufSIRC = NULL; /* StandardItem Reg. Catalog */
// HBUF     hbufSIDC = NULL; /* StandardItem Def. Catalog */
// HBUF     hbufSICC = NULL; /* StandardItem CBE  Catalog */

/*
 * Initialize work day calendars for the next seven years.
*/
#define NUM_OF_CALS  8

WORKDAYS  WorkDays [ NUM_OF_CALS ] = {
            {1995, FALSE, "Default"
    /*Jan*/ ,0,0,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1
    /*Feb*/ ,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,0
    /*Mar*/ ,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1
    /*Apr*/ ,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0
    /*May*/ ,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,1,1
    /*Jun*/ ,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1
    /*Jul*/ ,0,0,1,0,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1
    /*Aug*/ ,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1
    /*Sep*/ ,1,0,0,0,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0
    /*Oct*/ ,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1
    /*Nov*/ ,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0,0,0,0,1,1,1,1
    /*Dec*/ ,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,1,1,1,1,0,0}

            ,{1996, TRUE, "Default"
    /*Jan*/ ,0,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1
    /*Feb*/ ,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1
    /*Mar*/ ,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0
    /*Apr*/ ,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1
    /*May*/ ,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,1,1,1,1
    /*Jun*/ ,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0
    /*Jul*/ ,1,1,1,0,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1
    /*Aug*/ ,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0
    /*Sep*/ ,0,0,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1
    /*Oct*/ ,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1
    /*Nov*/ ,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0,0,0
    /*Dec*/ ,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,0,1,1,0,0,1,1}

            ,{1997, FALSE, "Default"
    /*Jan*/ ,0,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1
    /*Feb*/ ,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0
    /*Mar*/ ,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1
    /*Apr*/ ,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1
    /*May*/ ,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,1,1,1,1,0
    /*Jun*/ ,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1
    /*Jul*/ ,1,1,1,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1
    /*Aug*/ ,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0
    /*Sep*/ ,0,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1
    /*Oct*/ ,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1
    /*Nov*/ ,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0,0,0,0
    /*Dec*/ ,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0,1,0,0,1,1,1}

            ,{1998, FALSE, "Default"
    /*Jan*/ ,0,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0
    /*Feb*/ ,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0
    /*Mar*/ ,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1
    /*Apr*/ ,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1
    /*May*/ ,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,1,1,1,1,0,0
    /*Jun*/ ,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1
    /*Jul*/ ,1,1,1,0,0,0,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1
    /*Aug*/ ,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1
    /*Sep*/ ,1,1,1,1,0,0,0,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1
    /*Oct*/ ,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0
    /*Nov*/ ,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0,0,0,0,1
    /*Dec*/ ,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,0,0,0,1,1,1,1}

            ,{1999, FALSE, "Default"
    /*Jan*/ ,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0
    /*Feb*/ ,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0
    /*Mar*/ ,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1
    /*Apr*/ ,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1
    /*May*/ ,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0
    /*Jun*/ ,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1
    /*Jul*/ ,1,1,0,0,0,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0   
    /*Aug*/ ,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1
    /*Sep*/ ,1,1,1,0,0,0,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1
    /*Oct*/ ,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0
    /*Nov*/ ,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0,0,0,0,1,1
    /*Dec*/ ,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,1,1,1,1}

            ,{2000, TRUE, "Default"
    /*Jan*/ ,0,0,0,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1
    /*Feb*/ ,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1
    /*Mar*/ ,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1
    /*Apr*/ ,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0
    /*May*/ ,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,1,1
    /*Jun*/ ,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1
    /*Jul*/ ,0,0,1,0,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1
    /*Aug*/ ,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1
    /*Sep*/ ,1,0,0,0,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0
    /*Oct*/ ,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1
    /*Nov*/ ,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0
    /*Dec*/ ,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,1,1,1,1,0,0}

            ,{2001, FALSE, "Default"
    /*Jan*/ ,0,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1
    /*Feb*/ ,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0
    /*Mar*/ ,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0
    /*Apr*/ ,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1
    /*May*/ ,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,1,1,1
    /*Jun*/ ,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0
    /*Jul*/ ,0,1,1,0,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1
    /*Aug*/ ,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1
    /*Sep*/ ,0,0,0,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0
    /*Oct*/ ,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1
    /*Nov*/ ,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0,0
    /*Dec*/ ,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,0,1,1,1,0,0,1}

//            ,{2002, FALSE, "Default"
//    /*Jan*/ ,0,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1
//    /*Feb*/ ,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,0
//    /*Mar*/ ,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0
//    /*Apr*/ ,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1
//    /*May*/ ,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,1,1,1,1
//    /*Jun*/ ,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0
//    /*Jul*/ ,1,1,1,0,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1
//    /*Aug*/ ,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0
//    /*Sep*/ ,0,0,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1
//    /*Oct*/ ,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1
//    /*Nov*/ ,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0,0,0
//    /*Dec*/ ,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,0,1,1,0,0,1,1}
//
//            ,{2003, FALSE, "Default"
//    /*Jan*/ ,0,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1
//    /*Feb*/ ,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0
//    /*Mar*/ ,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1
//    /*Apr*/ ,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1
//    /*May*/ ,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,1,1,1,1,0
//    /*Jun*/ ,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1
//    /*Jul*/ ,1,1,1,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1
//    /*Aug*/ ,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0
//    /*Sep*/ ,0,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1
//    /*Oct*/ ,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1
//    /*Nov*/ ,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0,0,0,0
//    /*Dec*/ ,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0,1,0,0,1,1,1}
//
//            ,{2004, TRUE, "Default"
//    /*Jan*/ ,0,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0
//    /*Feb*/ ,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0
//    /*Mar*/ ,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1
//    /*Apr*/ ,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1
//    /*May*/ ,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0
//    /*Jun*/ ,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1
//    /*Jul*/ ,1,1,0,0,0,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0
//    /*Aug*/ ,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1
//    /*Sep*/ ,1,1,1,0,0,0,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1
//    /*Oct*/ ,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0
//    /*Nov*/ ,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0,0,0,0,1,1
//    /*Dec*/ ,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,1,1,1,1}
            
            };

PWORKDAYS pWorkDays = WorkDays;



/* define module specific globals */
//#ifdef DEBUG
//char szAppName [] = "BAMS/CES  [Development Release]";
//#else
char szAppName [] = "BAMS/CES";
char szAppNameMetric [] = "BAMS/CES                          (Metric SpecBook Active)";
//#endif

#define NUM_DATA_BYTES  (1 * sizeof (void far *))

MRESULT EXPENTRY LoadingProc (HWND     hwnd,
                              USHORT   usMessage,
                              MPARAM   mp1,
                              MPARAM   mp2)
   {
   switch (usMessage)
      {
      case WM_INITDLG:
         {
         SWP   swpMe;
         SWP   swpApplication;

#ifdef PRERELEASE
         ItiStrPrintfDlgItem (hwnd, DID_VERSION, VERSION_STRING, PRERELEASE);
#else
         ItiStrPrintfDlgItem (hwnd, DID_VERSION, VERSION_STRING, "");
#endif
         ItiStrPrintfDlgItem (hwnd, DID_COPY1, COPYRIGHT);
         ItiStrPrintfDlgItem (hwnd, DID_COPY2, COPYRIGHT);

         /* position the dialog box */
         WinQueryWindowPos (hwnd, &swpMe);
         WinQueryWindowPos (pglobals->hwndAppFrame, &swpApplication);

         swpMe.x = swpApplication.x + swpApplication.cx / 2 - swpMe.cx / 2;
         swpMe.y = swpApplication.y + swpApplication.cy -
                   (swpMe.cy + (SHORT) pglobals->alSysValues [SV_CYTITLEBAR] +
                    (SHORT) pglobals->alSysValues [SV_CYMENU] * 2);

         if (swpMe.x < 0)
            swpMe.x = 0;

         if (swpMe.y < 0)
            swpMe.y = 0;
         
         if (swpMe.x + swpMe.cx > (SHORT) pglobals->alSysValues [SV_CXSCREEN])
            swpMe.x -= (swpMe.x + swpMe.cx) -
                       (SHORT) pglobals->alSysValues [SV_CXSCREEN];

         if (swpMe.y + swpMe.cy > (SHORT) pglobals->alSysValues [SV_CYSCREEN])
            swpMe.y -= (swpMe.y + swpMe.cy) -
                       (SHORT) pglobals->alSysValues [SV_CYSCREEN];

         swpMe.fs = SWP_MOVE;
         WinSetMultWindowPos (hwnd, &swpMe, 1);
         }
         return 0;

      case WM_CLOSE:
         WinDismissDlg (hwnd, TRUE);
         break;
      }
   return WinDefDlgProc (hwnd, usMessage, mp1, mp2);
   }







HWND PreloadBuffer (USHORT uWinId)
   {
   USHORT   uJunk;
   HMODULE  hmod;
   CHAR     szQuery [800];

   ItiMbQueryHMOD (uWinId, &hmod);
   ItiMbQueryQueryText (hmod, ITIRID_WND, uWinId, szQuery, sizeof szQuery);
   return ItiDbCreateBuffer (NULL, uWinId, ITI_LIST, szQuery, &uJunk);
   }




void MBExit (PSZ pszStr)
   {
   WinMessageBox (pglobals->hwndDesktop, pglobals->hwndAppFrame, 
                              pszStr, "BAMS/CES Init", 0, MB_OK);
   DosExit (EXIT_PROCESS, 0);
   }


                                              

void CheckZoneDefaults (void)
   {
   HHEAP  hhp, hhp2;
   HQRY   hqry;
   CHAR   szQry[500] = "";
   CHAR   szZoneTypeKey[16] = "";
   USHORT uCols, uErr;
   USHORT usCount = 0;
   PSZ    pszCount = NULL;
   PSZ    pszRes = NULL;
   PSZ    *ppsz = NULL;

   hhp = ItiMemCreateHeap (MAX_HEAP_SIZE);
   hhp2 = ItiMemCreateHeap (1000);

   sprintf (szQry,"/* CheckZoneDefaults */ "
                  "IF NOT EXISTS (select ZoneTypeKey from ZoneType"   
                                " where ZoneTypeKey = 1) "
                  "INSERT INTO ZoneType "
                  "(ZoneTypeKey, ZoneTypeID, Description, Deleted) "
                  "VALUES "
                  "(1, ' DEFAULT', ' Statewide default zone type.', NULL)" );
   ItiDbExecSQLStatement (hhp2, szQry);

   sprintf (szQry,"/* CheckZoneDefaults */ "
                  "IF EXISTS (select ZoneTypeKey from ZoneType "   
                            " where ZoneTypeKey = 1 "
                              " and Deleted >= 1) "
                  "UPDATE ZoneType "
                  " SET Deleted = NULL "
                  " WHERE ZoneTypeKey = 1 AND Deleted >= 1");
   ItiDbExecSQLStatement (hhp2, szQry);


   sprintf (szQry,"/* CheckZoneDefaults */ "
                  "IF NOT EXISTS (select ZoneTypeKey from Zone "   
                            " where ZoneKey = 1 and ZoneTypeKey = 1 ) "
                  "INSERT INTO Zone "
                  "(ZoneTypeKey, ZoneKey, ZoneID, Description, Deleted) "
                  "VALUES "
                  "(1, 1, ' Default', ' Default zone.', NULL)" );
   ItiDbExecSQLStatement (hhp2, szQry);

   sprintf (szQry,"/* CheckZoneDefaults */ "
                  "IF EXISTS (select ZoneTypeKey from Zone "   
                            " where ZoneKey = 1 and ZoneTypeKey = 1 "
                              " and Deleted >= 1 ) "
                  "UPDATE Zone "
                  " SET Deleted = NULL "
                  " WHERE ZoneKey = 1 AND ZoneTypeKey = 1 "
                   " AND Deleted >= 1 " );
   ItiDbExecSQLStatement (hhp2, szQry);



   sprintf(szQry,
      "/* CheckZoneDefaults */ "
      "select DISTINCT ZoneTypeKey"
      " FROM Zone WHERE ZoneKey != 1"
               " AND ZoneTypeKey != 0 "
               " AND ZoneTypeKey NOT IN (select DISTINCT ZoneTypeKey"
                                       " from Zone where ZoneKey = 1) ");

   if (!(hqry = ItiDbExecQuery(szQry, hhp, 0, 0, 0, &uCols, &uErr)))
      {
		ItiErrWriteDebugMessage("Failed in CheckZoneDefaults.");
      ItiMemDestroyHeap (hhp);
      ItiMemDestroyHeap (hhp2);
      return;
      }

   while (ItiDbGetQueryRow(hqry, &ppsz, &uErr)) 
      {
      ItiStrCpy(szZoneTypeKey, ppsz[0], sizeof szZoneTypeKey);
      
      sprintf (szQry,"/* CheckZoneDefaults */ "
                     "IF NOT EXISTS (select ZoneTypeKey from Zone"
                               " where ZoneKey = 1 and ZoneTypeKey = %s ) "
                     "INSERT INTO Zone "
                     "(ZoneTypeKey, ZoneKey, ZoneID, Description, Deleted) "
                     "VALUES "
                     "(%s,1,' Default',' Default zone.', NULL)",
                     szZoneTypeKey, szZoneTypeKey);
      ItiDbExecSQLStatement (hhp2, szQry);
      
      sprintf (szQry,"/* CheckZoneDefaults */ "
                     "IF EXISTS (select ZoneTypeKey from Zone "
                               " where ZoneKey = 1 and ZoneTypeKey = %s "
                               " and Deleted >= 1) "
                     "UPDATE Zone "
                     " SET Deleted = NULL "
                     " WHERE ZoneTypeKey = %s AND ZoneKey = 1 "
                       " AND Deleted >= 1 "
                   , szZoneTypeKey, szZoneTypeKey);
      ItiDbExecSQLStatement (hhp2, szQry);


      sprintf (szQry,"/* CheckZoneDefaults */ "
                     "IF NOT EXISTS (select ZoneTypeKey from ZoneMap "
                               " where ZoneKey = 1 and MapKey = 1 "
                                  "and ZoneTypeKey = %s ) "
                      "INSERT INTO ZoneMap "
                      "(ZoneTypeKey, ZoneKey, MapKey, BaseDate) "
                      "VALUES "
                      "(%s,1,1,'%s')",
                      szZoneTypeKey, szZoneTypeKey, UNDECLARED_BASE_DATE);
      ItiDbExecSQLStatement (hhp2, szQry);

      ItiFreeStrArray (hhp, ppsz, uCols);
      }/* End of while (ItiDbGetQueryRow... */


   ItiStrCpy (szQry,"/* CheckZoneDefaults */ "
                    "IF EXISTS (select ZoneTypeKey from Zone "
                              " where ZoneKey = 1 and Deleted >= 1) "
                  "UPDATE Zone "
                  " SET Deleted = NULL "
                  " WHERE ZoneKey = 1 AND Deleted >= 1 "
                  , sizeof szQry);
   ItiDbExecSQLStatement (hhp2, szQry);


   ItiMemDestroyHeap (hhp);
   }/* End of Function CheckZoneDefaults */




void CheckPriceDefaults (void)
   {
   HHEAP  hhp, hhp2;
   HQRY   hqry, hqryE, hqryL;
   CHAR   szQry[500] = "";
   CHAR   szKey[16] = "";
   CHAR   szMinBaseDate[64] = "";
   USHORT uCols, uErr;
   USHORT usCount = 0;
   PSZ    pszCount = NULL;
   PSZ    pszRes = NULL;
   PSZ    pszMinBaseDate = NULL;
   PSZ    *ppsz = NULL;

   hhp = ItiMemCreateHeap (1000);
   hhp2 = ItiMemCreateHeap (1000);

   /* -- Get the undeclared base date. */
   sprintf(szQry, " select MIN(BaseDate) from BaseDate ");
   pszMinBaseDate = ItiDbGetRow1Col1(szQry, hhp2, 0, 0, 0);
   if (pszMinBaseDate == NULL)
      {
		ItiErrWriteDebugMessage
         ("Failed in CheckPriceDefaults undecBaseDate.");
      ItiMemDestroyHeap (hhp);
      ItiMemDestroyHeap (hhp2);
      return;
      }
   ItiStrCpy(szMinBaseDate, pszMinBaseDate, sizeof szMinBaseDate);
   ItiMemFree(hhp2, pszMinBaseDate);


   /* === Material default zone price check. ========================== */
   sprintf(szQry,
      "SELECT DISTINCT MaterialKey FROM MaterialPrice WHERE"
       " ZoneKey != 1 AND BaseDate IN ( '%s' , '%s' ) "
//      " MaterialKey"
//            " NOT IN (select MaterialKey"
//            " from MaterialPrice "
//            " WHERE ZoneKey = 1"
//            " AND UnitType = 0"
//            " AND BaseDate = '%s' )"
//      " OR MaterialKey"
//            " NOT IN (select MaterialKey"
//            " from MaterialPrice "
//            " WHERE ZoneKey = 1"
//            " AND UnitType = 1"
//            " AND BaseDate = '%s' )"
      ,szMinBaseDate, szMinBaseDate );

   if (!(hqry = ItiDbExecQuery(szQry, hhp, 0, 0, 0, &uCols, &uErr)))
      {
		ItiErrWriteDebugMessage
         ("Failed in CheckPriceDefaults MaterialPrice.");
      ItiMemDestroyHeap (hhp);
      ItiMemDestroyHeap (hhp2);
      return;
      }

   while (ItiDbGetQueryRow(hqry, &ppsz, &uErr)) 
      {
      ItiStrCpy(szKey, ppsz[0], sizeof szKey);

      /* -- English system */
      sprintf (szQry,"/* CheckPriceDefaults */ "
         "IF NOT EXISTS (select MaterialKey from MaterialPrice "
              " WHERE ZoneKey = 1 "
              " AND MaterialKey = %s "
              " AND UnitType = 0"
              " AND BaseDate = '%s' )"
         " INSERT INTO MaterialPrice "
         "(MaterialKey, ZoneKey, UnitPrice, Deleted, BaseDate, UnitType) "
         "VALUES ( %s ,1, 0, NULL, '%s', 0) ",
         szKey, szMinBaseDate, szKey, szMinBaseDate);
      ItiDbExecSQLStatement (hhp2, szQry);

      /* -- Metric system */
      sprintf (szQry,"/* CheckPriceDefaults */ "
         "IF NOT EXISTS (select MaterialKey from MaterialPrice "
              " WHERE ZoneKey = 1 "
              " AND MaterialKey = %s "
              " AND UnitType = 1"
              " AND BaseDate = '%s' )"
         " INSERT INTO MaterialPrice "
         "(MaterialKey, ZoneKey, UnitPrice, Deleted, BaseDate, UnitType) "
         "VALUES ( %s ,1, 0, NULL, '%s', 1) ",
         szKey, szMinBaseDate, szKey, szMinBaseDate);
      ItiDbExecSQLStatement (hhp2, szQry);

      //ItiFreeStrArray (hhp, ppsz, uCols);
      }/* end of while */

 
   /* === Equipment default zone price check. ========================== */
   sprintf(szQry,
      "UPDATE EquipmentRate SET Deleted = NULL "
      " WHERE BaseDate = (select MIN(BaseDate) from BaseDate)"
        " AND ZoneKey = 1 AND Deleted != NULL ");
   ItiDbExecSQLStatement (hhp2, szQry);

   /* -- Determine Equipment keys not found in rate table. */
   sprintf(szQry,
      "SELECT DISTINCT EquipmentKey FROM Equipment "
      " WHERE EquipmentKey"
            " NOT IN (select EquipmentKey"
            " FROM EquipmentRate WHERE ZoneKey = 1"
            " AND BaseDate = (select MIN(BaseDate) from BaseDate))");

   if (!(hqryE = ItiDbExecQuery(szQry, hhp, 0, 0, 0, &uCols, &uErr)))
      {
		ItiErrWriteDebugMessage
         ("Failed in CheckPriceDefaults Equipment.");
      ItiMemDestroyHeap (hhp);
      ItiMemDestroyHeap (hhp2);
      return;
      }

   while (ItiDbGetQueryRow(hqryE, &ppsz, &uErr)) 
      {
      ItiStrCpy(szKey, ppsz[0], sizeof szKey);
      
      sprintf (szQry,"/* CheckPriceDefaults */ "
         "IF NOT EXISTS (select EquipmentKey from EquipmentRate "
              " WHERE ZoneKey = 1 AND EquipmentKey = %s AND "
              " BaseDate = (select MIN(BaseDate) from BaseDate))"
         " INSERT INTO EquipmentRate "
         "(EquipmentKey, ZoneKey, Rate, OvertimeRate, Deleted, BaseDate) "
         "VALUES ( %s ,1, 0, 0, NULL, '%s' )",
         szKey, szKey, szMinBaseDate);

      ItiDbExecSQLStatement (hhp2, szQry);
      }/* end of while */



   /* -- Now do the rest. */
   sprintf(szQry,
      "SELECT DISTINCT EquipmentKey FROM EquipmentRate WHERE"
      " BaseDate = (select MIN(BaseDate) from BaseDate)"
      " AND  ZoneKey != 1"
      " AND EquipmentKey"
            " NOT IN (select EquipmentKey"
            " FROM EquipmentRate WHERE ZoneKey = 1"
            " AND BaseDate = (select MIN(BaseDate) from BaseDate))");

   if (!(hqryE = ItiDbExecQuery(szQry, hhp, 0, 0, 0, &uCols, &uErr)))
      {
		ItiErrWriteDebugMessage
         ("Failed in CheckPriceDefaults Equipment.");
      ItiMemDestroyHeap (hhp);
      ItiMemDestroyHeap (hhp2);
      return;
      }

   while (ItiDbGetQueryRow(hqryE, &ppsz, &uErr)) 
      {
      ItiStrCpy(szKey, ppsz[0], sizeof szKey);
      
      sprintf (szQry,"/* CheckPriceDefaults */ "
         "IF NOT EXISTS (select EquipmentKey from EquipmentRate "
              " WHERE ZoneKey = 1 AND EquipmentKey = %s AND "
              " BaseDate = (select MIN(BaseDate) from BaseDate))"
         " INSERT INTO EquipmentRate "
         "(EquipmentKey, ZoneKey, Rate, OvertimeRate, Deleted, BaseDate) "
         "VALUES ( %s ,1, 0, 0, NULL, '%s' )",
         szKey, szKey, szMinBaseDate);

      ItiDbExecSQLStatement (hhp2, szQry);
      }/* end of while */


   /* === Laborer default zone price check. ========================== */
   sprintf(szQry,
      "UPDATE LaborerWageAndBenefits SET Deleted = NULL "
      " WHERE BaseDate = (select MIN(BaseDate) from BaseDate)"
        " AND ZoneKey = 1 AND Deleted != NULL ");
   ItiDbExecSQLStatement (hhp2, szQry);


   /* -- Determine Laborer keys not found in wage table. */
   sprintf(szQry,
      "SELECT DISTINCT LaborerKey FROM Labor "
      " WHERE LaborerKey"
            " NOT IN (select LaborerKey"
            " FROM LaborerWageAndBenefits WHERE ZoneKey = 1"
            " AND BaseDate = (select MIN(BaseDate) from BaseDate))");

   if (!(hqryL = ItiDbExecQuery(szQry, hhp, 0, 0, 0, &uCols, &uErr)))
      {
		ItiErrWriteDebugMessage
         ("Failed in CheckPriceDefaults Laborer.");
      ItiMemDestroyHeap (hhp);
      ItiMemDestroyHeap (hhp2);
      return;
      }

   while (ItiDbGetQueryRow(hqryL, &ppsz, &uErr)) 
      {
      ItiStrCpy(szKey, ppsz[0], sizeof szKey);
      
      sprintf (szQry,
         "/* CheckPriceDefaults */ "
         "IF NOT EXISTS (select LaborerKey from LaborerWageAndBenefits "
         " WHERE ZoneKey = 1 AND LaborerKey = %s "
         " AND BaseDate = (select MIN(BaseDate) from BaseDate))"
         " INSERT INTO LaborerWageAndBenefits "
         "(LaborerKey, ZoneKey, "
         "DavisBaconRate, DavisBaconOvertimeRate, "
         "NonDavisBaconRate, NonDavisBaconOvertimeRate, "
         "Deleted, BaseDate) "
         "VALUES ( %s ,1, 0, 0, 0, 0, NULL, '%s' )",
         szKey, szKey, szMinBaseDate);

      ItiDbExecSQLStatement (hhp2, szQry);
      }/* end of while */


   /* -- Now do the rest. */
   sprintf(szQry,
      "SELECT DISTINCT LaborerKey FROM LaborerWageAndBenefits WHERE"
      " BaseDate = (select MIN(BaseDate) from BaseDate)"
      " AND  ZoneKey != 1"
      " AND LaborerKey"
            " NOT IN (select LaborerKey"
            " FROM LaborerWageAndBenefits WHERE ZoneKey = 1"
            " AND BaseDate = (select MIN(BaseDate) from BaseDate))");

   if (!(hqryL = ItiDbExecQuery(szQry, hhp, 0, 0, 0, &uCols, &uErr)))
      {
		ItiErrWriteDebugMessage
         ("Failed in CheckPriceDefaults Laborer.");
      ItiMemDestroyHeap (hhp);
      ItiMemDestroyHeap (hhp2);
      return;
      }

   while (ItiDbGetQueryRow(hqryL, &ppsz, &uErr)) 
      {
      ItiStrCpy(szKey, ppsz[0], sizeof szKey);
      
      sprintf (szQry,
         "/* CheckPriceDefaults */ "
         "IF NOT EXISTS (select LaborerKey from LaborerWageAndBenefits "
         " WHERE ZoneKey = 1 AND LaborerKey = %s "
         " AND BaseDate = (select MIN(BaseDate) from BaseDate))"
         " INSERT INTO LaborerWageAndBenefits "
         "(LaborerKey, ZoneKey, "
         "DavisBaconRate, DavisBaconOvertimeRate, "
         "NonDavisBaconRate, NonDavisBaconOvertimeRate, "
         "Deleted, BaseDate) "
         "VALUES ( %s ,1, 0, 0, 0, 0, NULL, '%s' )",
         szKey, szKey, szMinBaseDate);

      ItiDbExecSQLStatement (hhp2, szQry);
      }/* end of while */

   ItiMemDestroyHeap (hhp);
   ItiMemDestroyHeap (hhp2);
   }/* End of Function CheckPriceDefaults */


void SetWorkingDayCalendar (void)
   {

   HHEAP  hheap;
   CHAR   szQry[510] = "";
   PSZ    pszRes = NULL;
   USHORT usWorkDays, i, j, k, usTmp, usCount,
          usMonthIdx[12] = { 31,  60,  91, 121, 152, 182,
                            213, 244, 274, 305, 335, 366};


   hheap = ItiMemCreateHeap (4000);

   if (!hheap)
      {
		ItiErrWriteDebugMessage("Failed in SetWorkingDayCalendar.");
      return;
      }

   pWorkDays  = WorkDays;


               //   "/* Check WorkingDayCalendar */ "
               //   "IF EXISTS (select WorkingDayCalendarKey "
               //   "FROM WorkingDayCalendar "   
               //   "WHERE WorkingDayCalendarKey = 1) "
               //   " SELECT 1 "
               //   "ELSE SELECT 0 "

   sprintf (szQry,
                  "/* Check WorkingDayCalendar */ "
                  "select WorkingDayCalendarKey "
                  "FROM WorkingDayCalendar "   
                  "WHERE WorkingDayCalendarKey = 1 "
                  ) ;
   pszRes = ItiDbGetRow1Col1(szQry, hheap, 0, 0, 0);
	ItiErrWriteDebugMessage("=-=-= Check of WorkingDayCalendar yielded:");
	ItiErrWriteDebugMessage(pszRes);
   if (pszRes != NULL)
      {
      ItiStrToUSHORT (pszRes, &usCount);
      if (usCount == 1)
         {
         if (hheap)
            ItiMemDestroyHeap (hheap);
         return;
         }
      }


   sprintf (szQry,"/* SetWorkingDayCalendar */ "
                  "IF NOT EXISTS (select WorkingDayCalendarKey "
                  "FROM WorkingDayCalendar "   
                  "WHERE WorkingDayCalendarKey = 1) "
                  "INSERT INTO WorkingDayCalendar "
                  "VALUES "
                  "(1, 'Default', 'Default Working Day Calendar.', NULL)" );
   ItiDbExecSQLStatement (hheap, szQry);

   for (i = 0 ; i < NUM_OF_CALS ; i++)
      {
      usWorkDays = usTmp = 0;

      for (j = 0 ; j < 12 ; j++)
         {
         usWorkDays = 0;
         for (k = usTmp ; k < usMonthIdx[j] ; k++)
            usWorkDays += (USHORT) pWorkDays->bWorkDay[k];
         
         usTmp = usMonthIdx[j];

         sprintf (szQry,"/* SetWorkingDayCalendar */ "
                        "IF NOT EXISTS (select WorkingDayCalendarKey "
                        "FROM  WorkingDayPrediction "   
                        "WHERE WorkingDayCalendarKey = 1 "
                        "AND   Year  = %d "
                        "AND   Month = %d) "
                        "INSERT INTO WorkingDayPrediction "
                        "VALUES "
                        "(1, %d, %d, '%s', %d)",
                        pWorkDays->usYear, (j + 1),
                        pWorkDays->usYear, (j + 1), UNDECLARED_BASE_DATE,
                        usWorkDays);
         ItiDbExecSQLStatement (hheap, szQry);
         }
      pWorkDays++;
      }

   if (hheap)
      ItiMemDestroyHeap (hheap);


   return;
   }




void CheckJobBreakdownCodes (void)
   {
   HHEAP  hhp3;
   CHAR   szQry[120] = "";

   hhp3 = ItiMemCreateHeap (1000);

   sprintf (szQry,
            "UPDATE JobBreakdown "
            " SET RoadType = %d "
            " WHERE RoadType <= 0 "
            , CODE_DEF_ROAD);
   ItiDbExecSQLStatement (hhp3, szQry);

   sprintf (szQry,
      "UPDATE JobBreakdown SET StructureType = %d WHERE StructureType <= 0 "
            , CODE_DEF_STRU);
   ItiDbExecSQLStatement (hhp3, szQry);


   ItiMemDestroyHeap (hhp3);
   }/* End of Function CheckJobBreakdownCodes */




BOOL RegisterWindows (void);

void Initialize (void)
   {
   HWND     hwndLoading;
   HAB      habMainThread;
   HMQ      hmqMainThread;
   HWND     hwndAppFrame;
   HWND     hwndAppClient;
   HWND     hwndMainMenu;
   HELPINIT hinit;
   ULONG    flStyle;
   USHORT   y, cx, cy;
   HPOINTER hptr;
   HHEAP    hhpInit = NULL;
   PSZ      pszUS = NULL;
   CHAR     szQ[250] = "";
   CHAR     szQry[250] = "";
   CHAR     szIQry[250] = "";
   PSZ   pszMetricYear = NULL;
   PSZ   pszEnglishYear = NULL;

   extern CHAR  szMetricYear[];
   extern CHAR  szEnglishYear[];
   extern BOOL bEnglishYearGiven, bMetricYearGiven;


   flStyle = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER |
             FCF_MINMAX | FCF_SHELLPOSITION | FCF_TASKLIST |
             FCF_MENU | FCF_ACCELTABLE | FCF_ICON;

   /*
    * Initialize the thread for making Presentation Manager calls and
    * create the message queue.
    */

   habMainThread = WinInitialize (0);
   if (habMainThread == NULL)
      {
      ItiErrDisplayWindowError (habMainThread, hwndAppFrame, 0);
      DosExit (EXIT_PROCESS, 0);
      }

   hmqMainThread = WinCreateMsgQueue (habMainThread, 1000);

   if (hmqMainThread == NULL)
      {
      ItiErrDisplayWindowError (habMainThread, hwndAppFrame, 0);
      DosExit (EXIT_PROCESS, 0);
      }

   /* Register the main window class, terminate on failure. */
   if (!WinRegisterClass (habMainThread, szAppName,
                          MainWindowProc, 0, NUM_DATA_BYTES))
      {
      ItiErrDisplayWindowError (habMainThread, hwndAppFrame, 0);
      DosExit (EXIT_PROCESS, 0);
      }

   /* Create the main window, terminate on failure. */
   hwndAppFrame = WinCreateStdWindow (HWND_DESKTOP, 
                                      FS_NOBYTEALIGN,
                                      &flStyle, szAppName, NULL, 
                                      0L, 0, ID_RESOURCE, &hwndAppClient);

   if (hwndAppFrame == NULL)
      {
      ItiErrDisplayWindowError (habMainThread, hwndAppFrame, 0);
      DosExit (EXIT_PROCESS, 0);
      }
   y  = (USHORT) WinQuerySysValue (HWND_DESKTOP, SV_CYICON) * 2;
   cx = (USHORT) WinQuerySysValue (HWND_DESKTOP, SV_CXFULLSCREEN);
   cy = (USHORT) (WinQuerySysValue (HWND_DESKTOP, SV_CYFULLSCREEN) - y +
                  WinQuerySysValue (HWND_DESKTOP, SV_CYTITLEBAR));
   WinSetWindowPos (hwndAppFrame, HWND_TOP, 0, y, cx, cy, 
                    SWP_MOVE | SWP_SIZE | SWP_SHOW);

   /* Initialize the Globals module. DO NOT MOVE THIS CODE TO BE
      ANY LATER THAN THIS LINE!!! OR ALL HELL WILL BREAK LOOSE!!! */
   pglobals = ItiGlobInitGlobals (habMainThread, hmqMainThread, hwndAppFrame,
                                  hwndAppClient, szAppName, "BAMS/CES",
                                  VERSION_STRING);

   if (pglobals == NULL)
      {
      DosExit (EXIT_PROCESS, 0);
      }
   pglobals->usVersionMajor = VERSION_MAJOR;
   pglobals->usVersionMinor = VERSION_MINOR;
   pglobals->pszVersionString = VERSION_STRING;
   pglobals->ucVersionChar = VERSION_CHAR;

   if (!ItiWndSetMDIApp (hwndAppFrame))
      {
      ItiErrDisplayWindowError (habMainThread, hwndAppFrame, 0);
      DosExit (EXIT_PROCESS, 0);
      }

   /* set the multilingual character set */
   if (!WinSetCp (hmqMainThread, 850))
      {
      ItiErrWriteDebugMessage ("Could not set code page 850.");
      WinMessageBox (HWND_DESKTOP, hwndAppFrame, 
                     "Could not set code page 850.  See your OS/2 "
                     "documentation regarding the CODEPAGE and DEVINFO "
                     "lines in the CONFIG.SYS file.  "
                     "This error will only affect the appearance of "
                     "certain characters on the screen.", 
                     szAppName, 0, 
                     MB_OK | MB_ICONHAND | MB_MOVEABLE);
      }

   /* create and display the initial loading dialog box */
   hwndLoading = WinLoadDlg (pglobals->hwndDesktop, hwndAppFrame,
                             LoadingProc, 0, IDD_INITIAL, NULL);

   if (hwndLoading == NULL)
      {
      ItiErrDisplayWindowError (habMainThread, hwndAppFrame, 0);
//      DosExit (EXIT_PROCESS, 0);
      }


   hptr = WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, FALSE);
   WinSetPointer (HWND_DESKTOP, hptr);
   hptr = WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, FALSE);

   WinUpdateWindow (hwndAppClient);
   
   /*
    * Do time consuming initiailzation here
    */

   if (!ItiFmtInitialize ())
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("The format module could not be initialized.");
      }

   if (!ItiDbInit (2, 3))
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("The data base module could not be initialized.");
      }

// Programmer's Note: Metric changes moved from here to after
//                    the login call below.

   if (ItiWndRegister (habMainThread))
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("The list window module could not be initialized.");
      }
      
   hwndMainMenu = WinWindowFromID (hwndAppFrame, FID_MENU);

#ifdef DEBUG
   if (hwndMainMenu == NULL)
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("The Main Menu could not be found!");
      }
#endif 

   if (!ItiMenuInitialize (hwndMainMenu))
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("The ITIMENU module could not be initialized! "
               "Make sure that the tables Application, "
               "ApplicationCatalog, ApplicationReport, and "
               "Report have not been corrupted.");
      }

   if (!ItiEstInitialize ())
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("The ITIEST module could not be initialized! ");
      }

   if (!RegisterWindows ())
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      MBExit ("Could not register windows!");
      }

//    /* --- PRELOAD  SOME BUFFERS --- */
//    hbufSIC  = PreloadBuffer (StandardItemCatL);
//    hbufSIAC = PreloadBuffer (StdItemPemethAvgL);
//    hbufSIRC = PreloadBuffer (StdItemPemethRegL);
//    hbufSIDC = PreloadBuffer (StdItemDefUPL);
//    hbufSICC = PreloadBuffer (StdItemCBEstL);

   /* initialize help system */
   hinit.cb = sizeof hinit;
   hinit.ulReturnCode             = 0L;
   hinit.pszTutorialName          = NULL;
   hinit.phtHelpTable             = MAKEP (0xFFFF, HT_HELPTABLE);
   hinit.hmodHelpTableModule      = 0;
   hinit.hmodAccelActionBarModule = 0;
   hinit.idAccelTable             = BAMSCES_ACCELERATORS;
   hinit.idActionBar              = BAMSCES_MAIN_MENU;
   hinit.pszHelpWindowTitle       = "BAMS/CES Help",
   hinit.usShowPanelId            = CMIC_HIDE_PANEL_ID;
   hinit.pszHelpLibraryName       = "BAMSCES.hlp";

   pglobals->hwndHelp = WinCreateHelpInstance (habMainThread, &hinit);

   if (pglobals->hwndHelp == NULL || hinit.ulReturnCode != 0)
      {
      WinSetPointer (HWND_DESKTOP, hptr);
      ItiErrDisplayWindowError (habMainThread, hwndAppFrame, 0);
      }
   else
      {
      WinAssociateHelpInstance (pglobals->hwndHelp, hwndAppClient);
      }

   /* --- Set any missing default zones.         */
   /*     MUST come after the DB initialization! */
   CheckZoneDefaults();
   CheckPriceDefaults();
   bBaseDateListMade = FALSE;

   SetWorkingDayCalendar ();
   /* -- Check breakdown defaults */
   CheckJobBreakdownCodes();

   /*--- Log user in ---*/
   if (!ItiPermLogin (hwndLoading))
      DosExit (EXIT_PROCESS, 0);

   /* -- Identify the state we are in. */
   DosScanEnv("STATE", &pszState);
   if (pszState != NULL)
      {
      ItiStrCpy(&szState[0], pszState, sizeof szState);
      //// For Minnesota, test for CBEst first.
      pszChkState = ItiStrChr (szState, 'M');
      if ((pszChkState != NULL)
           && (0 == (ItiStrNICmp(pszChkState, "Minnesota", 9))))
         {
         bDoCBEFirst = TRUE;
         ItiErrWriteDebugMessage (szState);
         }
      else
         {
         bDoCBEFirst = FALSE;
         ItiErrWriteDebugMessage
            (" NO State environment variable specified.");
         }
      } /* end if (pszState... */

   /* -- SpecYear settings for Minnesota. */
   DosScanEnv("ENGLISHSPECYR", &pszEnglishYear);
   if (pszEnglishYear != NULL)
      {
      ItiStrCpy(&szEnglishYear[0], pszEnglishYear, YEARLEN);
      if ( ItiCharIsDigit(szEnglishYear[0]) 
          && ItiCharIsDigit(szEnglishYear[1])
          && ItiCharIsDigit(szEnglishYear[2])
          && ItiCharIsDigit(szEnglishYear[3])
         )
         {
         /* -- Do further validation here. */
         bEnglishYearGiven = TRUE;
         ItiErrWriteDebugMessage (szEnglishYear);
         }
      else
         {
         bEnglishYearGiven = FALSE;
         ItiErrWriteDebugMessage (" NO EnglishSpecYr environment variable specified.");
         }
      } /* end if (pszEnglishYear... */
   else
      bEnglishYearGiven = FALSE;

   DosScanEnv("METRICSPECYR", &pszMetricYear);
   if (pszMetricYear != NULL)
      {
      ItiStrCpy(&szMetricYear[0], pszMetricYear, YEARLEN);
      if ( ItiCharIsDigit(szMetricYear[0]) 
          && ItiCharIsDigit(szMetricYear[1])
          && ItiCharIsDigit(szMetricYear[2])
          && ItiCharIsDigit(szMetricYear[3])
         )
         {
         /* -- Do further validation here. */
         bMetricYearGiven = TRUE;
         ItiErrWriteDebugMessage (szMetricYear);
         }
      else
         {
         bMetricYearGiven = FALSE;
         ItiErrWriteDebugMessage (" NO MetricSpecYr environment variable specified.");
         }
      } /* end if (pszMetricYear... */
   else
      bMetricYearGiven = FALSE;




// --------------------------------------------- Metric System Changes
                    /* Note this section comes AFTER the ItiDbInit! */
   hhpInit = ItiMemCreateHeap (1000);

   sprintf (szQry,
       " /* Initialize */ "
       "IF EXISTS (select UserKey from UnitTypeSystem where UserKey = %s ) "
           " SELECT 'User already initialized.' "
       " ELSE INSERT INTO UnitTypeSystem (UnitType, UserKey) VALUES (0, %s) ",
       pglobals->pszUserKey,
       pglobals->pszUserKey);
   ItiDbExecSQLStatement (hhpInit, szQry);

   /* -- Set the metric flag. */
   sprintf (szQ,
           "SELECT UnitType FROM UnitTypeSystem WHERE UserKey = %s "
           ,pglobals->pszUserKey); 
   pszUS = ItiDbGetRow1Col1(szQ, hhpInit, 0, 0, 0);
   if ( (pszUS != NULL) && ((*pszUS) == '1') )
      pglobals->bUseMetric = TRUE;
   else
      pglobals->bUseMetric = FALSE;

   /* -- Set spec year stuff. */
   ItiDbExecSQLStatement (hhpInit, szIQry);

   sprintf (szIQry,
       " /* Initialize */ "
       " IF NOT EXISTS (SELECT SpecYear FROM CurrentSpecYear WHERE UserKey = %s ) "
       " INSERT INTO CurrentSpecYear (SpecYear, UserKey) "
       " SELECT MAX(SpecYear), %s FROM StandardItem "
       , pglobals->pszUserKey,
       pglobals->pszUserKey);
   ItiDbExecSQLStatement (hhpInit, szIQry);

   sprintf (szQ,
           "SELECT SpecYear FROM CurrentSpecYear WHERE UserKey = %s "
           ,pglobals->pszUserKey); 
   pszUS = ItiDbGetRow1Col1(szQ, hhpInit, 0, 0, 0);
   if ( (pszUS != NULL) && ((*pszUS) != '\0') && ((*pszUS) != '0') )
      {
      ItiStrCpy(&szCurSpecYear[0], pszUS, sizeof szCurSpecYear);
      }
   else
      {
      ItiStrCpy(&szCurSpecYear[0], "1900", sizeof szCurSpecYear);
      }

   pglobals->pszCurrentSpecYear = &szCurSpecYear[0];
   if (hhpInit != NULL)
      ItiMemDestroyHeap (hhpInit);

   /* -- Set the correct window title. */
   SetSpecYrTitle();
   //if (pglobals->bUseMetric)
   //   WinSetWindowText (hwndAppFrame, APP_TITLE_METRIC);
   //else
   //   WinSetWindowText (hwndAppFrame, APP_TITLE_ENGLISH);
// -------------------------------------------------------------------




   WinSetPointer (HWND_DESKTOP, hptr);
   WinDestroyWindow (hwndLoading);
   }


void Deinitialize (void)
   {
   WinSetWindowText (pglobals->hwndAppFrame, "  Exiting Program ");
//    ItiDbDeleteBuffer (hbufSIC, NULL);
//    ItiDbDeleteBuffer (hbufSIAC, NULL);
//    ItiDbDeleteBuffer (hbufSIRC, NULL);
//    ItiDbDeleteBuffer (hbufSIDC, NULL);
//    ItiDbDeleteBuffer (hbufSICC, NULL);
   WinDestroyWindow (pglobals->hwndAppFrame); 
   WinDestroyMsgQueue (pglobals->hmqMainMessageQueue); 
   ItiWndTerminate ();
   ItiDbTerminate ();
   WinTerminate (pglobals->habMainThread);
   }


BOOL RegisterWindows (void)
   {
   /* register static windows */
   if (ItiMbRegisterStaticWnd (ProgramUserS, ProgramUserSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (ProgramJobS, ProgramJobSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (ProgramFundS, ProgramFundSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (ProgramS, ProgramSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (JobS, JobSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (JobUserS, JobUserSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (JobItemsS, JobItemsSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (JobItemScheduleS, JobItemScheduleSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (JobItemS, JobItemSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (JobFundS, JobFundSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (JobCommentS, JobCommentSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (JobBreakdownFundS, JobBreakdownFundSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (JobBreakdownS, JobBreakdownSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (JobAlternateGroupS, JobAlternateGroupSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (DefaultUserS, DefaultUserSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (JobBreakdownListS, JobBreakdownListSProc, 0))
      return FALSE;
   if (ItiMbRegisterStaticWnd (JobAlternateGroupListS, JobAlternateGroupListSProc, 0))
      return FALSE;



   /* register list windows */
   if (ItiMbRegisterListWnd (DefaultUserL1, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (DefaultUserL2, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (JobAlternateL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (JobBreakdownFundL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (JobCommentL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (JobFundL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (JobItemL1, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (JobItemL2, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (JobItemScheduleL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (JobItemsL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (JobUserL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (ProgramFundL, ItiWndDefListWndTxtProc, 0))
      return FALSE;

   if (ItiMbRegisterListWnd (ProgramJobL, ItiWndDefListWndTxtProc, 0))

//   if (ItiMbRegisterListWnd (ProgramJobL, ProgramJobLProc, 0))
//      return FALSE;

   if (ItiMbRegisterListWnd (ProgramJobPredL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (ProgramJobSucL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (ProgramUserL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (JobBreakdownL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (JobAlternateGroupL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (DefJobUserCatL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (ChProgJobL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (AddProgJobL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (JobBreakFundL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (ItemSchPredL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (ItemSchSucL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (DefltJobUsersL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (DefltProgUsersL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (PredSchedChangeL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (SuccSchedChangeL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (PredSchedAddL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (SuccSchedAddL, ItiWndDefListWndTxtProc, 0))
      return FALSE;

   if (ItiMbRegisterListWnd (ProgramJobSchPredL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (ProgramJobSchSuccL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (ProgramJobPredSchChgL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (ProgramJobSuccSchChgL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (ProgramJobPredSchAddL, ItiWndDefListWndTxtProc, 0))
      return FALSE;
   if (ItiMbRegisterListWnd (ProgramJobSuccSchAddL, ItiWndDefListWndTxtProc, 0))
      return FALSE;


   /* register dialog boxes */
   if (ItiMbRegisterDialogWnd (OpenJobD, OpenJobD, OpenJobDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd (OpenProgramD, OpenProgramD, OpenProgramDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd (JobItemD, JobItemD, JobItemDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd (JobD, JobD, JobDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd (JobAltGroupD, JobAltGroupD, JobAltGroupDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd (JobAltD, JobAltD, JobAltDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd (JobCommentD, JobCommentD, JobCommentDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd (JobBreakdownD, JobBreakdownD, JobBreakdownDProc, 0))
      return FALSE;

//   if (ItiMbRegisterDialogWnd (ChDefJobUsersEditDlg, ChDefJobUsersEditDlg, ChDefJobUsersEditDlgProc, 0))
//      return FALSE;
   if (ItiMbRegisterDialogWnd (DefJobUsersD, DefJobUsersD, DefJobUsersDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd (DefProgUsersD, DefProgUsersD, DefProgUsersDProc, 0))
      return FALSE;

   if (ItiMbRegisterDialogWnd  (ChJobUserD, ChJobUserD, ChJobUserDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd  (AddJobUserD, AddJobUserD, AddJobUserDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd  (JobNewD, JobNewD, JobNewDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd  (ProgramD, ProgramD, ProgramDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd  (ChProgJobD, ChProgJobD, ChProgJobDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd  (AddProgJobD, AddProgJobD, AddProgJobDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd  (AddProgUserD, AddProgUserD, AddProgUserDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd  (ChProgUserD, ChProgUserD, ChProgUserDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd  (ChJobBreakFundD, ChJobBreakFundD, ChJobBreakFundDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd  (AddJobBreakFundD, AddJobBreakFundD, AddJobBreakFundDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd  (ItemSchedD, ItemSchedD, ItemSchedDProc, 0))
      return FALSE;

   if (ItiMbRegisterDialogWnd  (PredecessorsChangeD, PredecessorsChangeD, PredecessorsChangeDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd  (SuccessorsChangeD, SuccessorsChangeD, SuccessorsChangeDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd  (PredecessorsAddD, PredecessorsAddD, PredecessorsAddDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd  (SuccessorsAddD, SuccessorsAddD, SuccessorsAddDProc, 0))
      return FALSE;

   if (ItiMbRegisterDialogWnd  (ProgramJobSchedD, ProgramJobSchedD, ProgramJobSchedDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd  (ProgramJobPredChangeD, ProgramJobPredChangeD, ProgramJobPredChangeDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd  (ProgramJobSuccChangeD, ProgramJobSuccChangeD, ProgramJobSuccChangeDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd  (ProgramJobPredAddD, ProgramJobPredAddD, ProgramJobPredAddDProc, 0))
      return FALSE;
   if (ItiMbRegisterDialogWnd  (ProgramJobSuccAddD, ProgramJobSuccAddD, ProgramJobSuccAddDProc, 0))
      return FALSE;

   if (ItiMbRegisterDialogWnd  (DefltJobProgUsersD, DefltJobProgUsersD, DefltJobProgUsersDProc, 0))
      return FALSE;

   if (ItiMbRegisterDialogWnd  (DeclareBaseDate, DeclareBaseDate, DeclareBaseDateProc, 0))
      return FALSE;

   if (ItiMbRegisterDialogWnd  (UpdateBaseDate, UpdateBaseDate, UpdateBaseDateProc, 0))
      return FALSE;

//   if (ItiMbRegisterDialogWnd  (JobCBESTAdd, JobCBESTAdd, JobCBESTAddProc, 0))
//      return FALSE;
//

   if (ItiMbRegisterDialogWnd  (CopyJobD, CopyJobD, CopyJobDProc, 0))
      return FALSE;

   return TRUE;
   }
