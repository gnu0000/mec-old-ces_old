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
 * HardErr.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 */


#define INCL_VIO
#define INCL_KBD
#define INCL_DOSMODULEMGR

#include "..\include\iti.h"
#include "..\include\itierror.h"
#include "..\include\itibase.h"
#include "..\include\itiutil.h"
#include "..\include\itiglob.h"
#include "initdll.h"
#include <stdio.h>

#define NUM_BANNER_LINES (sizeof amsgBanner / sizeof (MESSAGE))

#define MESSAGE_LINE       5
#define PRINT_LINE         23
#define ESCAPE_LINE        24

#define APPLICATION_LINE   2
#define APPLICATION_COL    42

typedef struct 
   {
   PSZ   pszMessage;
   BYTE  bAttribute;
   } MESSAGE;


static MESSAGE amsgBanner [] = 
   {
         /* 00000000001111111111222222222233333333334444444444555555555566666666667777777777 */
         /* 01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
/* 00 */ { "      ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป       ", 0x1e },
/* 01 */ { "      บ            Info Tech DS/Shell Application Error               บ       ", 0x1e },
/* 02 */ { "      บ                      Application:                               บ       ", 0x1e },
/* 03 */ { "      ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ       ", 0x1e },
/* 04 */ { "                                                                                ", 0x1c },
/* 05 */ { "                                                                                ", 0x1c },
/* 06 */ { "                                                                                ", 0x1c },
/* 07 */ { "                                                                                ", 0x1c },
/* 08 */ { "                                                                                ", 0x1c },
/* 09 */ { "                                                                                ", 0x1c },
/* 10 */ { "                                                                                ", 0x1c },
/* 11 */ { "                                                                                ", 0x1c },
/* 12 */ { "                                                                                ", 0x1c },
/* 13 */ { "                                                                                ", 0x1c },
/* 14 */ { "                                                                                ", 0x1c },
/* 15 */ { "                                                                                ", 0x1c },
/* 16 */ { "                                                                                ", 0x1c },
/* 17 */ { "                                                                                ", 0x1c },
/* 18 */ { "                                                                                ", 0x1c },
/* 19 */ { "                                                                                ", 0x1c },
/* 20 */ { "                                                                                ", 0x1c },
/* 21 */ { "                                                                                ", 0x1c },
/* 22 */ { "        This error has been copied to the log file                              ", 0x1a },
/* 23 */ { "        Press the Print Screen key if you want to print this error.             ", 0x1a },
/* 24 */ { "         Press the ESCape key to continue, or press Alt-X to exit.              ", 0x1a },
   };

#define FILE_LINE    22
#define FILE_COLUMN  51

void EXPORT ItiErrDisplayHardError (PSZ pszMessage)
   {
   USHORT      i;
   KBDKEYINFO  kbci;
   USHORT      usFlag;
   PSZ         pszTemp;
   USHORT      usLen;
   PGLOBALS    pglobals;
   BYTE        b;

   pglobals = ItiGlobQueryGlobalsPointer ();

   usFlag = VP_WAIT | VP_OPAQUE;
   DosBeep (1000, 250);
   if (0 != VioPopUp (&usFlag, 0))
      {
      DosBeep (1500, 250);
      ItiErrWriteDebugMessage ("Could not create VIO popup! Message follows:");
      ItiErrWriteDebugMessage (pszMessage);
      return;
      }

#if defined (DEBUG)
   /* go ahead and write the message to the debug pipe */
   ItiErrWriteDebugMessage (pszMessage);
#endif 

   for (i = 0; i < NUM_BANNER_LINES; i++)
      VioWrtCharStrAtt (amsgBanner [i].pszMessage, 
                        ItiStrLen (amsgBanner [i].pszMessage), i, 0,
                        &amsgBanner [i].bAttribute, 0);

   /* output the debug output file name */
   VioWrtCharStrAtt (szFileName, ItiStrLen (szFileName),
                     FILE_LINE, FILE_COLUMN,
                     &amsgBanner [FILE_LINE].bAttribute, 0);
      
   /* output the application name */
   if (pglobals && pglobals->pszAppName)
      {
      b = 0x1a;
      VioWrtCharStrAtt (pglobals->pszAppName, ItiStrLen (pglobals->pszAppName),
                        APPLICATION_LINE, APPLICATION_COL,
                        &b, 0);
      }
   else if (pglobals)
      {
      b = 0x9c;
      VioWrtCharStrAtt ("Unregistered!", 8,
                        APPLICATION_LINE, APPLICATION_COL,
                        &b, 0);
      }
   else
      {
      b = 0x9c;
      VioWrtCharStrAtt ("Unknown!", 8,
                        APPLICATION_LINE, APPLICATION_COL,
                        &b, 0);
      }

   i = MESSAGE_LINE;
   while (*pszMessage)
      {
      pszTemp = ItiStrChr (pszMessage, '\n');
      if (pszTemp == NULL)
         {
         usLen = ItiStrLen (pszMessage);
         VioWrtCharStr (pszMessage, ItiStrLen (pszMessage), i, 1, 0);
         pszMessage += usLen;
         }
      else
         {
         VioWrtCharStr (pszMessage, pszTemp - pszMessage, i, 1, 0);
         pszMessage = pszTemp+1;
         }
      i++;
      }

   while (0 == KbdCharIn (&kbci, IO_WAIT, 0) &&
          (kbci.fbStatus == 0 || kbci.chChar != (UCHAR) 0x1b))
      {
//      char szTemp [256];
//
//      sprintf (szTemp, "chChar = %x, chScan = %x, fbStatus = %x, fsState = %x", 
//               (int) kbci.chChar, 
//               (int) kbci.chScan, 
//               (int) kbci.fbStatus, 
//               (int) kbci.fsState);
//      ItiErrWriteDebugMessage (szTemp);

      if (kbci.chChar == (UCHAR) 0x00 &&
          kbci.chScan == (UCHAR) 0x2d &&
          kbci.fsState & KBDSTF_ALT)
         {
         VioEndPopUp (0);
         DosExit (EXIT_PROCESS, 0);
         }
      }


   VioEndPopUp (0);
   }
