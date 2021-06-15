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
 * DbQry.c
 *
 */

#define DATABASE_INTERNAL
#define INCL_WIN
#define INCL_DOSMISC
#define DBMSOS2
#include "..\INCLUDE\Iti.h"
#include "..\INCLUDE\itibase.h"
#include "..\INCLUDE\itiutil.h"
#include <sqlfront.h>
#include <sqldb.h>
#include "dbqry.h"
#include <stdio.h>


#define FOREVER SEM_INDEFINITE_WAIT   

static HDB  hdb;
static HSEM hsemHDBMutex;       // access to the queue


BOOL DbInit (void)
   {
   LOGINREC *plogin;
   PSZ pszServerName, pszDatabase, pszUserName, pszPassword;

   ItiSemCreateMutex (NULL, 0, &hsemHDBMutex);

   if (DosScanEnv ("SERVER", &pszServerName))
      pszServerName = "";
   if (DosScanEnv ("DATABASE", &pszDatabase))
      pszDatabase = "DSShell";
   if (DosScanEnv ("USERNAME", &pszUserName))
      pszUserName = "sa";
   if (DosScanEnv ("PASSWORD", &pszPassword))
      pszPassword = "";

   /* set time to wait for login to 5 seconds. -- mdh */
   dbsetlogintime (5);

   plogin = dblogin();
   DBSETLAPP  (plogin, "app"); 
   DBSETLHOST (plogin, "host");
   DBSETLUSER (plogin, pszUserName);
   DBSETLPWD  (plogin, pszPassword);

   if ((hdb = dbopen (plogin, pszServerName)) == NULL)
      {
      printf ("unable to open db\n");
      dbfreelogin (plogin);
      return FALSE;
      }

   if ((dbuse (hdb, pszDatabase)) == FAIL)
      {
      printf ("unable to use db\n");
      dbclose (hdb);
      dbfreelogin (plogin);
      return FALSE;
      }

   dbfreelogin (plogin);

   return TRUE;
   }



static HDB GetHDB (void)
   {
   ItiSemRequestMutex (hsemHDBMutex, FOREVER);
   return hdb;
   }


/*
 * param is for later
 */

static void FreeHDB (HDB hdb)
   {
   ItiSemReleaseMutex (hsemHDBMutex);
   }



static void FreeHQRY (HQRY hqry)
   {
   ItiMemFree (hqry->hheap, hqry);
   FreeHDB (hdb);
   }




/*
 * If an error occurs, this fn returns NULL
 * and you can check uState for the error code. 
 * If tou get NULL but error code = SUCCEED, there was insuficient memory
 * currently hmod,uResId, and uId are not used. They are for the 
 * formatting which has yet to be implemented
 */
HQRY DbExecQuery (PSZ     pszQuery,
                  HHEAP   hheap,  
                  USHORT  *puNumCols,
                  USHORT  *puState)
   {
   HDB   hdb;
   HQRY  hqry;

   hdb = GetHDB ();

   /*--- passing the sql text to the dblib ---*/
   if ((*puState = dbcmd(hdb, pszQuery)) == FAIL)
      {
      FreeHDB (hdb);
      printf ("unable to process dbcmd, uState = %u\n", *puState);
      return FALSE;
      }

   if ((*puState = dbsqlexec(hdb)) == FAIL)
      {
      FreeHDB (hdb);
      printf ("unable to process dbsqlexec, uState = %u\n", *puState);
      return FALSE;
      }

   if ((*puState = dbresults(hdb)) != SUCCEED)
      {
      FreeHDB (hdb);
      printf ("Query did not succeed, usState=%u, query = \"%s\"\n", 
               *puState, pszQuery);
      return FALSE;
      }
   *puNumCols = dbnumcols (hdb);

   if ((hqry = (HQRY) ItiMemAlloc (hheap, sizeof (QRY), 0)) == NULL)
      return NULL;
   hqry->hdb   = hdb;
   hqry->hheap = hheap;
   hqry->uNumCols = *puNumCols;
   return hqry;
   }


static void DbCancel (HDB hdb)
   {
   dbcancel (hdb);
   }


static USHORT DbNumCols (HDB hdb)
   {
   return dbnumcols (hdb);
   }

static PSZ DbColName (HDB hdb, USHORT uCol)
   {
   return (PSZ) dbcolname (hdb, uCol + 1);
   }

static USHORT DbColLen  (HDB hdb, USHORT uCol)
   {
   return (USHORT)dbcollen (hdb, uCol + 1);
   }

static USHORT DbDatLen  (HDB hdb, USHORT uCol)
   {
   return (USHORT)dbdatlen (hdb, uCol + 1);
   }

static USHORT DbColType (HDB hdb, USHORT uCol)
   {
   return (USHORT)dbcoltype (hdb, uCol + 1);
   }



static BOOL DbGetCol (HDB hdb, USHORT uCol, PSZ *ppszCol, USHORT *uType, USHORT *uLen)
   {
   *ppszCol = (PSZ) dbdata  (hdb, uCol + 1);
   *uLen  = (USHORT) DbDatLen  (hdb, uCol);
   *uType = (USHORT) DbColType (hdb, uCol);
   return *uLen != 0;
   }



/* like above proc except that the col is converted to a string
 *
 * returns 0 if successful
 *
 */
static USHORT DbGetFmtCol (HDB    hdb,
                           USHORT uCol,
                           PSZ    pszCol,
                           USHORT uStrLen,
                           USHORT *puDestLen)
   {
   BYTE     *pData;
   SHORT    iLen;
   USHORT   uLen, uType;

   *pszCol = '\0';
   if (DbGetCol (hdb, uCol, &pData, &uType, &uLen))
      iLen = dbconvert (hdb, uType, pData, uLen, SQLCHAR, pszCol, uStrLen);
   else
      iLen = 0;

   if (iLen == -1)
      return 2;
   pszCol [iLen] = '\0';
   if (puDestLen)
      *puDestLen = iLen;
   return 0;
   }

BOOL DbGetQueryRow (HQRY   hqry,
                    PSZ    **pppsz,
                    USHORT *puState)
   {
   CHAR     szCol[1024];
   USHORT   i;

   *pppsz = NULL;
   if ((*puState = dbnextrow(hdb)) != MORE_ROWS)
      {
      FreeHQRY (hqry);
      return FALSE;
      }
   if ((*pppsz = ItiMemAlloc (hqry->hheap, sizeof (PSZ)* hqry->uNumCols, 0))== NULL)
      return FALSE;

   for (i = 0; i < hqry->uNumCols; i++)
      {
      DbGetFmtCol (hqry->hdb, i, szCol, sizeof szCol, NULL);
      (*pppsz)[i] = ItiStrDup (hqry->hheap, szCol);
      }
   return TRUE;
   }




void DbTerminateQuery (HQRY hqry)
   {
   DbCancel (hqry->hdb);
   FreeHQRY (hqry);
   return;
   }

