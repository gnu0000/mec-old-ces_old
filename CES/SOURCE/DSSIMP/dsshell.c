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
 * dssimp.c
 */

#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "..\include\itifmt.h"
#include "..\include\itiwin.h"
#include "..\include\itiest.h"
#include "..\include\winid.h"
#include "..\include\itiglob.h"
#include "..\include\itiimp.h"
#include "..\include\itierror.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dssimp.h"
#include "initdll.h"

static PSZ   apszDTokens[10],  apszDValues[10];


/*******************************************************************
 * Copies
 *******************************************************************/

USHORT EXPORT ItiImpCopyCodeTable (HHEAP       hheap,
                                   PIMPORTINFO pii,
                                   PSZ         pszProdDatabase)
   {
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYCODETABLE);
   }


USHORT EXPORT ItiImpCopyCodeValue (HHEAP       hheap,
                                   PIMPORTINFO pii,
                                   PSZ         pszProdDatabase)
   {
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYCODEVALUE);
   }


USHORT EXPORT ItiImpCopyCounty (HHEAP       hheap,
                                PIMPORTINFO pii,
                                PSZ         pszProdDatabase)
   {
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYCOUNTY);
   }


USHORT EXPORT ItiImpCopyCountyMap (HHEAP       hheap,
                                   PIMPORTINFO pii,
                                   PSZ         pszProdDatabase)
   {
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYCOUNTYMAP);
   }


USHORT EXPORT ItiImpCopyFund (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PSZ         pszProdDatabase)
   {
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYFUND);
   }


USHORT EXPORT ItiImpCopyMajorItem (HHEAP       hheap,
                                   PIMPORTINFO pii,
                                   PSZ         pszProdDatabase)
   {
   USHORT usRet = 0;

   /* -- Major Items */
   apszDTokens [0] = "ProductionDatabase"; apszDValues [0] = pszProdDatabase;
   apszDTokens [1] = "MajorItemKey";      apszDValues [1] = "Number";
   apszDTokens [2] = "MajorItemID";       apszDValues [2] = "String";
   apszDTokens [3] = "Description";       apszDValues [3] = "String";
   apszDTokens [4] = "CommonUnit";        apszDValues [4] = "Number";
   apszDTokens [5] = "SpecYear";          apszDValues [5] = "Number";
   apszDTokens [6] = "UnitType";          apszDValues [6] = "Number";
   apszDTokens [7] = NULL;                apszDValues [7] = NULL;

   usRet = ItiImpIndividualExec (hheap, hmod, apszDTokens, apszDValues, 7,
                       COPYMAJORITEM+INDV_INSERT, COPYMAJORITEM+INDV_SELECT);
   return usRet;
   }


USHORT EXPORT ItiImpCopyMap (HHEAP       hheap,
                             PIMPORTINFO pii,
                             PSZ         pszProdDatabase)
   {
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYMAP);
   }



USHORT EXPORT ItiImpCopyMapVertex (HHEAP       hheap,
                                   PIMPORTINFO pii,
                                   PSZ         pszProdDatabase)
   {
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYMAPVERTEX);
   }



USHORT EXPORT ItiImpCopyMaterial (HHEAP       hheap,
                                  PIMPORTINFO pii,
                                  PSZ         pszProdDatabase)
   {
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYMATERIAL);
   }



USHORT EXPORT ItiImpCopySIDependency (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PSZ         pszProdDatabase)
   {
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYSIDEPENDENCY);
   }



USHORT EXPORT ItiImpCopySIMajorItem (HHEAP       hheap,
                                     PIMPORTINFO pii,
                                     PSZ         pszProdDatabase)
   {
   /* -- Individually copy SpecYears by UnitType. */
#define   NUMBER_OF_SPEC_YRS  16
#define   YEAR_LENGTH          7
   static PSZ    pszImpDatabase = "";
   static CHAR   szTmp[512] = "";
   static CHAR   szTemplate[512] = "";
   // BOOL   bOK;
   USHORT i, usRet, usCnt, uCols, uErr;
   PSZ    *ppszTmp;
   CHAR   aszSpecYear[NUMBER_OF_SPEC_YRS + 1][YEAR_LENGTH + 1];
   HHEAP  hhpLocal;
   HQRY   hqry;


   hhpLocal = ItiMemCreateHeap (3000);
   if (hhpLocal == NULL)
      {
      ItiErrWriteDebugMessage
         ("IMPORT ERROR: NO heap could be created in dssimp.dsshell.c.ItiImpCopySIMajorItem" );
      return 13;
      }

   pszImpDatabase = ItiImpQueryImportDatabase();
   if (pszImpDatabase == NULL)
      pszImpDatabase = "DSShellImport";

   sprintf(szTmp,"/* dssimp.dsshell.c.ItiImpCopySIMajorItem */ "
                 " USE %s ", pszImpDatabase);
   usRet = ItiDbExecSQLStatement (hhpLocal, szTmp);

   /* -- ================= Make a list of SpecYears. */
   sprintf(szTmp,"SELECT DISTINCT SpecYear FROM %s..MajorItem  "
           , pszImpDatabase);
   if (!(hqry = ItiDbExecQuery(szTmp, hhpLocal, 0, 0, 0, &uCols, &uErr)))
      {
      ItiErrWriteDebugMessage
         ("dssimp.dsshell.c.ItiImpCopySIMajorItem, failed ExecQuery.");
      ItiMemDestroyHeap (hhpLocal);
      return 13;
      }

   aszSpecYear[0][0] = '\0';
   usCnt = 0;
   while (ItiDbGetQueryRow(hqry, &ppszTmp, &uErr)) 
      {
      if (usCnt <= NUMBER_OF_SPEC_YRS)
         {
         if ( *ppszTmp[0] == '\0')
            ItiStrCpy(aszSpecYear[usCnt], " NULL ", YEAR_LENGTH); 
         else
            ItiStrCpy(aszSpecYear[usCnt], ppszTmp[0], YEAR_LENGTH);

         aszSpecYear[usCnt+1][0] = '\0';
         }
      else
         ItiErrWriteDebugMessage
            ("ERR: dssimp.dsshell.c.ItiImpCopySIMajorItem exceeded array space.");

      ItiFreeStrArray(hhpLocal, ppszTmp, uCols); 
      usCnt++;
      }/* end of while */


   ItiMbQueryQueryText (hmod, ITIRID_IMPORT, 
                        COPYSIMAJORITEM, szTemplate, sizeof szTemplate);


   /* -- ================= Loop thru the SpecYear list. */
   for (i = 0; i < usCnt; i++)
      {
      /* -- Update where  UnitType is English. */
      sprintf(szTmp, szTemplate, pszProdDatabase, aszSpecYear[i], " 0 ");
      usRet = ItiDbExecSQLStatement (hhpLocal, szTmp);

      /* -- Update where  UnitType is Metric.  */
      sprintf(szTmp, szTemplate, pszProdDatabase, aszSpecYear[i], " 1 ");
      usRet = ItiDbExecSQLStatement (hhpLocal, szTmp);

      /* -- Update where  UnitType is Both.    */
      sprintf(szTmp, szTemplate, pszProdDatabase, aszSpecYear[i], " NULL ");
      usRet = ItiDbExecSQLStatement (hhpLocal, szTmp);

      }/* end of for loop */

   if (hhpLocal != NULL)
      ItiMemDestroyHeap (hhpLocal);

   return usRet;

   // return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYSIMAJORITEM);
   } /* End of Function */



USHORT EXPORT ItiImpCopyZone (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PSZ         pszProdDatabase)
   {
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYZONE);
   }



USHORT EXPORT ItiImpCopyZoneType (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PSZ         pszProdDatabase)
   {
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYZONETYPE);
   }



USHORT EXPORT ItiImpCopyZoneMap (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PSZ         pszProdDatabase)
   {
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYZONEMAP);
   }



USHORT EXPORT ItiImpCopyVendorClass (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PSZ         pszProdDatabase)
   {
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYVENDORCLASS);
   }



USHORT EXPORT ItiImpCopyVendorAddress (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PSZ         pszProdDatabase)
   {
   return ItiImpExec (hheap, pii, pszProdDatabase, hmod, COPYVENDORADDRESS);
   }




/*******************************************************************
 * Foreign keys
 *******************************************************************/

USHORT EXPORT ItiImpCodeTableKey (HHEAP       hheap,
                                  PIMPORTINFO pii,
                                  PCOLINFO    pci,
                                  PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, 
                          "CodeTableKey",          /* target column */
                          NULL,                    /* code table name */
                          "CodeTable",             /* prod table name */
                          "CodeTableKey",          /* prod key name */
                          "CodeTableID",           /* prod id name */
                          ITIIMP_GETDISTINCTFKS,   /* query ID */
                          ITIIMP_UPDATE1,          /* update ID */
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   }


USHORT EXPORT ItiImpCountyKey (HHEAP       hheap,
                               PIMPORTINFO pii,
                               PCOLINFO    pci,
                               PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, 
                          "CountyKey",             /* target column */
                          NULL,                    /* code table name */
                          "County",                /* prod table name */
                          "CountyKey",             /* prod key name */
                          "CountyID",              /* prod id name */
                          ITIIMP_GETDISTINCTFKS,   /* query ID */
                          ITIIMP_UPDATE1,          /* update ID */
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   }

USHORT EXPORT ItiImpMapKey (HHEAP       hheap,
                            PIMPORTINFO pii,
                            PCOLINFO    pci,
                            PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, 
                          "MapKey",                /* target column */
                          NULL,                    /* code table name */
                          "Map",                   /* prod table name */
                          "MapKey",                /* prod key name */
                          "MapID",                 /* prod id name */
                          ITIIMP_GETDISTINCTFKS,   /* query ID */
                          ITIIMP_UPDATE1,          /* update ID */
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   }

USHORT EXPORT ItiImpCommonUnit (HHEAP       hheap,
                                PIMPORTINFO pii,
                                PCOLINFO    pci,
                                PSZ         pszProdDatabase)
   {
   USHORT usReturn = 0;
   CHAR szProdTable [62] = "";
   CHAR szProdKey [62]   = "";
   CHAR szProdID [62]    = "";

   if ( (0 == ItiStrCmp(pii->pti->pszTableName, "MajorItem"))
      && (0 == ItiStrCmp(pci->pszColumnName, "CommonUnitText")) )
      {
      ItiStrCpy (szProdTable,"CodeValue",    sizeof szProdTable);
      ItiStrCpy (szProdKey,  "CodeValueKey", sizeof szProdKey);
      ItiStrCpy (szProdID,   "CodeValueID",  sizeof szProdID);

      usReturn = ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, 
                          "CommonUnit",            /* target column   */
                          "UNITS",                 /* code table name */
                          szProdTable,             /* prod table name */
                          szProdKey,               /* prod key name   */
                          szProdID,                /* prod id name    */
                          ITIIMP_GETDISTINCTFKS,   /* query ID        */
                          ITIIMP_UPDATECODEVALUE,  /* update ID       */
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
      }
   else
      usReturn = ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, 
                          "CommonUnit",            /* target column */
                          "UNITS",                 /* code table name */
                          szProdTable,             /* prod table name */
                          szProdKey,               /* prod key name */
                          szProdID,                /* prod id name */
                          ITIIMP_GETDISTINCTFKS,   /* query ID */
                          ITIIMP_UPDATE1,          /* update ID */
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   return usReturn;
   }

USHORT EXPORT ItiImpUnit (HHEAP       hheap,
                       PIMPORTINFO pii,
                       PCOLINFO    pci,
                       PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, 
                          "Unit",                  /* target column */
                          "UNITS",                 /* code table name */
                          NULL,                    /* prod table name */
                          NULL,                    /* prod key name */
                          NULL,                    /* prod id name */
                          ITIIMP_GETDISTINCTFKS,   /* query ID */
                          ITIIMP_UPDATE1,          /* update ID */
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   }


USHORT EXPORT ItiImpZoneTypeKey (HHEAP       hheap,
                                 PIMPORTINFO pii,
                                 PCOLINFO    pci,
                                 PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, 
                          "ZoneTypeKey",           /* target column */
                          NULL,                    /* code table name */
                          "ZoneType",              /* prod table name */
                          "ZoneTypeKey",           /* prod key name */
                          "ZoneTypeID",            /* prod id name */
                          ITIIMP_GETDISTINCTFKS,   /* query ID */
                          ITIIMP_UPDATE1,          /* update ID */
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   }


USHORT EXPORT ItiImpStandardItemKey (HHEAP       hheap,
                                     PIMPORTINFO pii,
                                     PCOLINFO    pci,
                                     PSZ         pszProdDatabase)
   {
   char szDestColumn [255];

   ItiStrCpy (szDestColumn, pci->pszColumnName, sizeof szDestColumn);
   szDestColumn [ItiStrLen (szDestColumn) - 4] = '\0';

   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, 
                          szDestColumn,         /* target column */
                          NULL,                 /* code table name */
                          "StandardItem",       /* prod table name */
                          "StandardItemKey",    /* prod key name */
                          "StandardItemNumber", /* prod id name */
                          ITIIMP_GETDISTINCTFKS,/* query ID */
                          STANDARDITEMKEY,      /* update ID */
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   }

USHORT EXPORT ItiImpMajorItemKey (HHEAP       hheap,
                                  PIMPORTINFO pii,
                                  PCOLINFO    pci,
                                  PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, 
                          "MajorItemKey",          /* target column */
                          NULL,                    /* code table name */
                          "MajorItem",             /* prod table name */
                          "MajorItemKey",          /* prod key name */
                          "MajorItemID",           /* prod id name */
                          ITIIMP_MSY_GETDISTINCTFKS,   /* query ID  */
                          ITIIMP_MI_KEYTEXT_UPDATE,    /* update ID */
                          ITIIMP_KEYTEXT );     /* SpecYr from key text */
                        //  ITIIMP_MULTISPECYRS );       /* MSY       */
                        //  ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   }


USHORT EXPORT ItiImpMIStandardItemKey (HHEAP       hheap,
                                             PIMPORTINFO pii,
                                             PCOLINFO    pci,
                                             PSZ         pszProdDatabase)
   {
   USHORT usRet = 0;
   CHAR szDestColumn [255] = "";

   ItiStrCpy (szDestColumn, pci->pszColumnName, sizeof szDestColumn);
   szDestColumn [ItiStrLen (szDestColumn) - 4] = '\0';

   usRet = ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, 
                          szDestColumn,         /* target column        */
                          NULL,                 /* code table name      */
                          "StandardItem",       /* prod table name      */
                          "StandardItemKey",    /* prod key name        */
                          "StandardItemNumber", /* prod id name         */
                          ITIIMP_MSY_GETDISTINCTFKS,   /* query ID      */
                          ITIIMP_MI_KEYTEXT_UPDATE,    /* update ID     */
                                       /* ignore unittype consideration */
                          ITIIMP_KEYTEXT );     /* SpecYr from key text */
   return usRet;
   }/* End of Function */

   
USHORT EXPORT ItiImpZoneKey (HHEAP       hheap,
                         PIMPORTINFO pii,
                         PCOLINFO    pci,
                         PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, 
                          "ZoneKey",               /* target column */
                          NULL,                    /* code table name */
                          "Zone",                  /* prod table name */
                          "ZoneKey",               /* prod key name */
                          "ZoneID",                /* prod id name */
                          ITIIMP_GETDISTINCTFKS,   /* query ID */
                          ITIIMP_UPDATE1,          /* update ID */
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   }


USHORT EXPORT ItiImpVendorClass (HHEAP       hheap,
                               PIMPORTINFO pii,
                               PCOLINFO    pci,
                               PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, 
                          "VendorClassification",  /* target column */
                          "VENCLAS",               /* code table name */
                          NULL,                    /* prod table name */
                          NULL,                    /* prod key name */
                          NULL,                    /* prod id name */
                          ITIIMP_GETDISTINCTFKS,   /* query ID */
                          ITIIMP_UPDATE1,          /* update ID */
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   }



USHORT EXPORT ItiImpKey (HHEAP       hheap,
                         PIMPORTINFO pii,
                         PCOLINFO    pci,
                         PSZ         pszProdDatabase)
   {
   char szDestColumn [255] = "";
   char szID [250] = "";
   char szTable [250] = "";

   ItiStrCpy (szDestColumn, pci->pszColumnName, sizeof szDestColumn);
   szDestColumn [ItiStrLen (szDestColumn) - 4] = '\0';
   ItiStrCpy (szID, szDestColumn, sizeof szTable);

   ItiStrCpy (szTable, pii->pti->pszTableName, sizeof szTable);
   //szTable [ItiStrLen (szTable) - 3] = '\0';
   ItiStrCpy (szID, szTable, sizeof szID);
   ItiStrCat (szID, "ID", sizeof szID);

   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, 
                          szDestColumn,            /* target column */
                          NULL,                    /* code table name */
                          szTable,                 /* prod table name */
                          szDestColumn,            /* prod key name */
                          szID,                    /* prod id name */
                          ITIIMP_GETDISTINCTFKS,   /* query ID */
                          ITIIMP_UPDATE1,          /* update ID */
                          ITIIMP_DOIMPORTDBFIRST | ITIIMP_ZERONULLS);
   } /* End of Function ItiImpKey  */



USHORT EXPORT ItiImpSignificantMajorItemKey (HHEAP       hheap,
                                  PIMPORTINFO pii,
                                  PCOLINFO    pci,
                                  PSZ         pszProdDatabase)
   {
   USHORT usRet = 0;

   usRet = ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, 
                          "MajorItemKey",          /* target column */
                          NULL,                    /* code table name */
                          "MajorItem",             /* prod table name */
                          "MajorItemKey",          /* prod key name   */
                          "MajorItemKeyText",       /* prod id name    */
                          ITIIMP_PARA_SIGMI_SEL_FK,     /* query ID   */
                          ITIIMP_PARA_SIGMI_UPD_FK,     /* update ID  */
                          ITIIMP_PARAMETRIC );     /* parametric flag */
                    //      ITIIMP_PARA_GETDISTINCTFKS,   /* query ID   */
                    //      ITIIMP_PARA_UPDATE,           /* update ID  */
                    //      ITIIMP_PARAMETRIC );     /* parametric flag */
                    //    //  ITIIMP_MULTISPECYRS );       /* MSY       */

   return usRet;                     
   }

 


USHORT EXPORT  ItiImpParaProKey (HHEAP       hheap,
                                PIMPORTINFO pii,
                                PCOLINFO    pci,
                                PSZ         pszProdDatabase)
   {
   /* -- This function is used to fill in the ParametricProfile key       */
   /* -- in the SignificantMajorItem table of the DSShellImport database. */

   char szDestColumn [60] = "";
   char szID [60] = "";
   char szTable [60] = "";

   /* -- remove the 'Text' suffix. */
   ItiStrCpy (szDestColumn, pci->pszColumnName, sizeof szDestColumn);
   szDestColumn [ItiStrLen (szDestColumn) - 4] = '\0';
   ItiStrCpy (szID, szDestColumn, sizeof szTable);

   ItiStrCpy (szTable, pii->pti->pszTableName, sizeof szTable);
   ItiStrCpy (szID, szTable, sizeof szID);
   ItiStrCat (szID, "ID", sizeof szID);

   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, 
                          szDestColumn,            /* target column */ 
                          NULL,                    /* code table name */
                          szTable,                 /* prod table name */
                          szDestColumn,            /* prod key name */
                          szID,                    /* prod id name */
                          ITIIMP_PARA_GETDISTINCTFKS,   /* query ID   */
                          ITIIMP_PARA_UPDATE_IMP,           /* update ID  */
                          ITIIMP_PARAMETRIC );     /* parametric flag */
   } /* End of Function ItiImpParaProKey  */





USHORT EXPORT ItiImpMIQtyRegKey (HHEAP       hheap,
                                  PIMPORTINFO pii,
                                  PCOLINFO    pci,
                                  PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, 
                          "MajorItemKey",          /* target column */
                          NULL,                    /* code table name */
                          "MajorItem",             /* prod table name */
                          "MajorItemKey",          /* prod key name   */
                          "MajorItemKeyText",       /* prod id name    */
                          ITIIMP_PARA_MIQTY_SEL_FK, /* query ID   */
                          ITIIMP_PARA_MIQTY_UPD_FK, /* update ID  */
                          ITIIMP_PARAMETRIC );      /* parametric flag, */
                                   /* use only import DB to find keys. */
   } /* End od Function ItiImpMIQtyRegKey */



USHORT EXPORT ItiImpMIQtyKey (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PCOLINFO    pci,
                              PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, 
                          "MajorItemKey",          /* target column */
                          NULL,                    /* code table name */
                          "MajorItem",             /* prod table name */
                          "MajorItemKey",          /* prod key name   */
                          "MajorItemKeyText",       /* prod id name    */
                          ITIIMP_PARA_MIQ_SEL_FK, /* query ID   */
                          ITIIMP_PARA_MIQ_UPD_FK, /* update ID  */
                          ITIIMP_PARAMETRIC );      /* parametric flag, */
                                   /* use only import DB to find keys. */
   } /* End od Function ItiImpMIQtyKey */

                           

/////////////////////////

USHORT EXPORT ItiImpMIPriceRegKey (HHEAP       hheap,
                                  PIMPORTINFO pii,
                                  PCOLINFO    pci,
                                  PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, 
                          "MajorItemKey",          /* target column */
                          NULL,                    /* code table name */
                          "MajorItem",             /* prod table name */
                          "MajorItemKey",          /* prod key name   */
                          "MajorItemKeyText",       /* prod id name    */
                          ITIIMP_PARA_MIQ_SEL_FK, /* query ID   */
                          ITIIMP_PARA_MIQ_UPD_FK, /* update ID  */
                          ITIIMP_PARAMETRIC );      /* parametric flag, */
                                   /* use only import DB to find keys. */
   } /* End od Function ItiImpMIPriceRegKey */



USHORT EXPORT ItiImpMIPriceKey (HHEAP       hheap,
                              PIMPORTINFO pii,
                              PCOLINFO    pci,
                              PSZ         pszProdDatabase)
   {
   return ItiImpFillInFK (hheap, hmod, pii, pci, pszProdDatabase, 
                          "MajorItemKey",          /* target column */
                          NULL,                    /* code table name */
                          "MajorItem",             /* prod table name */
                          "MajorItemKey",          /* prod key name   */
                          "MajorItemKeyText",       /* prod id name    */
                          ITIIMP_PARA_MIQ_SEL_FK, /* query ID   */
                          ITIIMP_PARA_MIQ_UPD_FK, /* update ID  */
                          ITIIMP_PARAMETRIC );      /* parametric flag, */
                                   /* use only import DB to find keys. */
   } /* End od Function ItiImpMIPriceKey */

                           

