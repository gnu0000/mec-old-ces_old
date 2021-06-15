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


/*
 * install.c
 */

#include "..\include\iti.h"
#include "initcat.h"
#include "itiinst.h"



/*
 * ItiEstInstall Creates all the necessary tables, indicies, etc for both
 * the production database and import database.
 *
 * Returns TRUE on success, FALSE on error.
 */

BOOL EXPORT ItiInstall (HWND hwnd)
   {
   WinMessageBox (HWND_DESKTOP, hwnd,
                  "This new release requires that no users are running "
                  "the old release during installation.  Make sure that "
                  "no one else is running BAMS/CES.  Then choose the "
                  "Deny logon command from the utilities menu.  Then exit "
                  "this application, and from a full screen OS/2 command "
                  "prompt, type the command: \"A:CESINST\" to install "
                  "the CES Executables, or \"A:SERVINST\" to install the "
                  "database updates.  For more information, refer to the "
                  "installation instructions accompanying this shipment.",
                  "Info Tech Install",
                  0, MB_ICONHAND | MB_OK | MB_MOVEABLE);

   return TRUE;
   }


USHORT EXPORT ItiDllQueryVersion (void)
   {
   return 1;
   }

