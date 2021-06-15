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
 * WildCard.c
 * Mark Hampton
 * Charles Engelke
 *
 * This module supplies a wildcard matching routine.
 */


#include "..\include\iti.h"
#include "..\include\itiutil.h"


/* returns TRUE if the string pszString contains a wildcard character */
BOOL EXPORT ItiStrIsWildCard (PSZ pszString)
   {
   while (*pszString)
      {
      if (*pszString == '?' || *pszString == '*')
         return TRUE;
      pszString += 1;
      }
   return FALSE;
   }


BOOL EXPORT ItiStrMatchWildCard (PSZ pszWildCard,       /* Wildcard to match */
                                 PSZ pszMatch)          /* string to match */
   {
   if (*pszWildCard == '\0')
      {
      /* if we're at the end of the wild card, make sure we're at the
         end of the match string */
      return *pszMatch == '\0';
      }
   switch (*pszWildCard)
      {
      case '*':
         pszWildCard += 1;
         while (TRUE)
            {
            if (ItiStrMatchWildCard (pszWildCard, pszMatch))
               /* we found a match -- return TRUE */
               return TRUE;
            else if (*pszMatch)
               /* skip on to the next match character */
               pszMatch += 1;
            else
               /* we've reached the end of the string -- no match */
               return FALSE;
            }
         break;

      case '?':
         if (*pszMatch == '\0')
            /* a ? does not match an end of string */
            return FALSE;
         else
            /* a match -- return check the rest of the string */
            return ItiStrMatchWildCard (pszWildCard + 1, pszMatch + 1);
         break;

      default:
         if (*pszMatch == '\0')
            /* no match -- we're at the end of the match string */
            return FALSE;
         else if (*pszWildCard != *pszMatch)
            /* the two characters do not match -- return FALSE */
            return FALSE;
         else
            /* the two match. continue processing */
            return ItiStrMatchWildCard (pszWildCard + 1, pszMatch + 1);
         break;
      }
   }

