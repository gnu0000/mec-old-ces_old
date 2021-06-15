/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by Info Tech, Inc.  All Rights Reserved.         |
|                                                                           |
|       This program module is part of DS/Shell (PC), Info Tech's           |
|       database interfaces for OS/2, a proprietary product of              |
|       Info Tech, Inc., no part of which may be reproduced or              |
|       transmitted in any form or by any means, electronic,                |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from Info Tech, Inc.          |
|                                                                           |
+--------------------------------------------------------------------------*/


/***************************************************************************\
* DATA.C -- This file contains per process global variables
* Created by Microsoft Corporation, 1989
\***************************************************************************/

#define NO_DOS
#define NO_GPI
#include "tool.h"

/*
   This library uses a NON SHARED DATA selector.  This means each
   process using the library gets its own selector, and also that
   values cannot be shared and must be recreated for each process.
*/

HMODULE vhModule;            /* Library module handle */
HHEAP  vhheap;               /* Library heap */

PSTR   vrgsz[CSTRINGS];      /* Array of pointer to our strings (indexed
                                by IDS_... */
