‚‚‚‚‚‚ |       Programs, a proprietary product of AASHTO, no part of which may be  |
|       reproduced or transmitted in any form or by any means, electronic,  |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from AASHTO.                  |
|                                                                           |
+--------------------------------------------------------------------------*/


#include "..\include\iti.h"
#include "..\include\itifmt.h"
#include "..\include\itiutil.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include <stdio.h>
#include "default.h"

int _cdecl main (int argc, char *argv [])
   {
   HHEAP    hheap;            
   PSZ      pszJobKey         ;
   PSZ      pszBaseDate       ;
   PSZ      pszJobBreakdownKey;
   PSZ      pszStandardItemKey;
   PSZ      pszQuantity       ;
   PSZ      pszPrice;

   if (!ItiFmtInitialize ())
      {
      printf ("Could not initialize format module.\n");
      return 1;
      }

   if (!ItiDbInit (0, 1))
      {
      printf ("Could not initialize data base module.\n");
      return 2;
      }

   hheap = ItiMemCreateHeap (0);

   printf ("%-10s %-12s %-10s %-10s %-14s %-14s\n%-10s %-12s %-10s %-10s %-14s %-14s\n",
           "Job", "Base Date", "Job Break", "StdItem", "Quantity", "Price",
           "----------", "------------", "----------", "----------", "--------------", "--------------");

   pszJobKey          = NULL;
   pszBaseDate        = "1/25/1990";
   pszJobBreakdownKey = NULL;
   pszStandardItemKey = "1036404";
   pszQuantity        = NULL;
   pszPrice = ItiEstItem (hheap, pszBaseDate, pszJobKey, pszJobBreakdownKey,
                          pszStandardItemKey, pszQuantity);
   printf ("%-10s %-12s %-10s %-10s %-14s %-14s\n",
           pszJobKey, pszBaseDate, pszJobBreakdownKey, pszStandardItemKey, 
           pszQuantity, pszPrice);

   pszJobKey          = "1000000";
   pszBaseDate        = NULL;
   pszJobBreakdownKey = "1000000";
   pszStandardItemKey = "1036404";
   pszQuantity        = "12";
   pszPrice = ItiEstItem (hheap, pszBaseDate, pszJobKey, pszJobBreakdownKey,
                          pszStandardItemKey, pszQuantity);
   printf ("%-10s %-12s %-10s %-10s %-14s %-14s\n",
           pszJobKey, pszBaseDate, pszJobBreakdownKey, pszStandardItemKey, 
           pszQuantity, pszPrice);

   pszJobKey          = "1000000";
   pszBaseDate        = "1/25/1990";
   pszJobBreakdownKey = "1000000";
   pszStandardItemKey = "1036404";
   pszQuantity        = "12";
   pszPrice = ItiEstItem (hheap, pszBaseDate, pszJobKey, pszJobBreakdownKey,
                          pszStandardItemKey, pszQuantity);
   printf ("%-10s %-12s %-10s %-10s %-14s %-14s\n",
           pszJobKey, pszBaseDate, pszJobBreakdownKey, pszStandardItemKey, 
           pszQuantity, pszPrice);

   pszJobKey          = NULL;
   pszBaseDate        = NULL;
   pszJobBreakdownKey = NULL;
   pszStandardItemKey = "1036404";
   pszQuantity        = "12";
   pszPrice = ItiEstItem (hheap, pszBaseDate, pszJobKey, pszJobBreakdownKey,
                          pszStandardItemKey, pszQuantity);
   printf ("%-10s %-12s %-10s %-10s %-14s %-14s\n",
           pszJobKey, pszBaseDate, pszJobBreakdownKey, pszStandardItemKey, 
           pszQuantity, pszPrice);

   pszJobKey          = NULL;
   pszBaseDate        = NULL;
   pszJobBreakdownKey = NULL;
   pszStandardItemKey = NULL;
   pszQuantity        = NULL;
   pszPrice = ItiEstItem (hheap, pszBaseDate, pszJobKey, pszJobBreakdownKey,
                          pszStandardItemKey, pszQuantity);
   printf ("%-10s %-12s %-10s %-10s %-14s %-14s\n",
           pszJobKey, pszBaseDate, pszJobBreakdownKey, pszStandardItemKey, 
           pszQuantity, pszPrice);

   pszJobKey          = "1000000";
   pszBaseDate        = NULL;
   pszJobBreakdownKey = "1000000";
   pszStandardItemKey = NULL;
   pszQuantity        = "12";
   pszPrice = ItiEstItem (hheap, pszBaseDate, pszJobKey, pszJobBreakdownKey,
                          pszStandardItemKey, pszQuantity);
   printf ("%-10s %-12s %-10s %-10s %-14s %-14s\n",
           pszJobKey, pszBaseDate, pszJobBreakdownKey, pszStandardItemKey, 
           pszQuantity, pszPrice);

   pszJobKey          = "1000000";
   pszBaseDate        = "gibberish";
   pszJobBreakdownKey = "1000000";
   pszStandardItemKey = "1036404";
   pszQuantity        = "12";
   pszPrice = ItiEstItem (hheap, pszBaseDate, pszJobKey, pszJobBreakdownKey,
                          pszStandardItemKey, pszQuantity);
   printf ("%-10s %-12s %-10s %-10s %-14s %-14s\n",
           pszJobKey, pszBaseDate, pszJobBreakdownKey, pszStandardItemKey, 
           pszQuantity, pszPrice);

   pszJobKey          = NULL;
   pszBaseDate        = "1/25/1991";
   pszJobBreakdownKey = "1000000";
   pszStandardItemKey = "-1";
   pszQuantity        = "12";
   pszPrice = ItiEstItem (hheap, pszBaseDate, pszJobKey, pszJobBreakdownKey,
                          pszStandardItemKey, pszQuantity);
   printf ("%-10s %-12s %-10s %-10s %-14s %-14s\n",
           pszJobKey, pszBaseDate, pszJobBreakdownKey, pszStandardItemKey, 
           pszQuantity, pszPrice);

   pszJobKey          = "-1";
   pszBaseDate        = NULL;
   pszJobBreakdownKey = "1000000";
   pszStandardItemKey = "1036404";
   pszQuantity        = "12";
   pszPrice = ItiEstItem (hheap, pszBaseDate, pszJobKey, pszJobBreakdownKey,
                          pszStandardItemKey, pszQuantity);
   printf ("%-10s %-12s %-10s %-10s %-14s %-14s\n",
           pszJobKey, pszBaseDate, pszJobBreakdownKey, pszStandardItemKey, 
           pszQuantity, pszPrice);

   return 0;
   }


