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
    TOOL.C -- Contains commonly used routines and globals
    Created by Microsoft Corporation, 1989
*/

#define NO_DOS
#define NO_GPI
#include "tool.h"


/****************************************************************************\
* This function returns the character following the current one.
\****************************************************************************/

PSZ  FAR PASCAL NextChar (PSZ lpsz)
   {
   if (*lpsz) 
      return ++lpsz; 
   else 
      return lpsz;
   }


/****************************************************************************\
* This function returns the character previous to the current one.
\****************************************************************************/

PSZ  FAR PASCAL PrevChar (PSZ lpszStart, PSZ lpszCurrent)
   {
   if (lpszCurrent > lpszStart) 
      return --lpszCurrent; 
   else 
      return lpszStart;
   }


/****************************************************************************\
* This function transforms a string to upper case.
\****************************************************************************/

PSZ  FAR PASCAL Upper (PSZ lpsz) 
   {
   PSZ  lpszPtr = lpsz;

   while (*lpszPtr) 
      {
      if (*lpszPtr >= 'a' && *lpszPtr <= 'z') 
         *lpszPtr &= ~0x20;
      lpszPtr++;
      }
   return lpsz;
   }
