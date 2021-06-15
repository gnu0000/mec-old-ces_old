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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <share.h>
#include <sqlfront.h>
#include <sqldb.h>
#include "dbqry.h"
#include "harderr.h"
#include "servinst.h"

#define FOREVER SEM_INDEFINITE_WAIT   

static HDB  hdb;
static HSEM hsemHDBMutex;       // access to the queue
static FILE *pfLog = stderr;

static PGINFOSEG pgis;

static BOOL DbResults (HDB hdb);
static BOOL DbNextRow (HDB hdb, USHORT *uState);

int API _loadds err_handler (DBPROCESS *dbproc, 
                             int       iSeverity, 
                             int       iDbError, 
                             int       iOsError, 
                             char      *pszDbErrorString, 
                             char      *pszOsErrorString)
   
   {
   if ((dbproc == NULL) || (DBDEAD(dbproc)))
      return INT_CANCEL;
   else
      {
      fprintf (pfLog, "ERROR: %s\n", pszDbErrorString);
      if (iOsError != DBNOERR)
         fprintf (pfLog, "OS/2 ERROR: %s\n", pszOsErrorString);
      if (iSeverity != 0)
         fprintf (pfLog, "ERROR Severity = %d, dberr = %d, oserr = %d, dbproc = %p\n", 
                  iSeverity, iDbError, iOsError, dbproc);
      return INT_CANCEL;
      }
   }

int API _loadds msg_handler (DBPROCESS    *dbproc, 
                             DBINT        lMessageNumber, 
                             DBSMALLINT   sMessageState, 
                             DBSMALLINT   sSeverity, 
                             char         *pszMessage)
   {
   fprintf (pfLog, "MESSAGE: %s\n", pszMessage);
   if (sSeverity != 0)
      fprintf (pfLog, "MESSAGE = %ld, state %d, severity %d, dbproc = %p\n",
               lMessageNumber, sMessageState, sSeverity, dbproc);
   return 0;
   }






void DbDeInit (void)
   {
   fprintf (pfLog, "End time: %2d/%.2d/%.4d %2d:%.2d:%.2d%cm\n",
               (USHORT) pgis->month,
               (USHORT) pgis->day,
               (USHORT) pgis->year,
               (USHORT) pgis->hour > 12 ? pgis->hour - 12 : pgis->hour,
               (USHORT) pgis->minutes,
               (USHORT) pgis->seconds,
               (USHORT) pgis->hour >= 12 ? 'p' : 'a');
   fclose (pfLog);
   pfLog = stderr;
   }


FILE *DbInit (PSZ pszLogFile, PSZ pszServer, PSZ pszDatabase, PSZ pszSA, PSZ pszPassword)
   {
   LOGINREC *plogin;
   SEL selGlobal, selLocal;

   DosGetInfoSeg (&selGlobal, &selLocal);
   pgis = MAKEP (selGlobal, 0);

   pfLog = _fsopen (pszLogFile, "wt", SH_DENYWR);
   if (pfLog == NULL)
      {
      fprintf (stderr,"\n\n\n\n\nCould not open log file %s\n", pszLogFile);
      return NULL;
      }

   fprintf (pfLog, "ServInst release %s\n%s\n", VERSION_STRING, dbinit ());
   fprintf (pfLog, "Start time: %2d/%.2d/%.4d %2d:%.2d:%.2d%cm\n",
               (USHORT) pgis->month,
               (USHORT) pgis->day,
               (USHORT) pgis->year,
               (USHORT) pgis->hour > 12 ? pgis->hour - 12 : pgis->hour,
               (USHORT) pgis->minutes,
               (USHORT) pgis->seconds,
               (USHORT) pgis->hour >= 12 ? 'p' : 'a');

//   ItiSemCreateMutex (NULL, 0, &hsemHDBMutex);

   if (pszServer == NULL)
      pszServer = "";
   if (pszSA == NULL)
      pszSA = "sa";
   if (pszPassword == NULL)
      pszPassword = "";
   if (pszDatabase == NULL)
      pszDatabase = "master";

   /* set time to wait for login to 5 seconds. -- mdh */
   dbsetlogintime (5);

   plogin = dblogin();
   DBSETLAPP  (plogin, "ServInst"); 
   DBSETLHOST (plogin, "ServInst");
   DBSETLUSER (plogin, pszSA);
   DBSETLPWD  (plogin, pszPassword);

   /* set up error handlers -- mdh */
   dberrhandle (err_handler);
   dbmsghandle (msg_handler);

   if (pszServer == NULL)
      pszServer = "";
   if ((hdb = dbopen (plogin, pszServer)) == NULL)
      {
      char szTemp [1024];

      sprintf (szTemp, 
               "Unable to connect to SQL Server.\n"
               "This may mean that the SQL server is not running, or that\n"
               "you have supplied the wrong server machine name.\n");

      DisplayHardError (szTemp, pszLogFile);
      fputs ("Could not connect to SQL Server.\n", pfLog);
      dbfreelogin (plogin);
      return NULL;
      }

   if ((dbuse (hdb, "master")) == FAIL)
      {
      DisplayHardError ("Unable to use the master database.  The database may be corrupted.", pszLogFile);
      fprintf (pfLog, "Could not use database master\n");
      dbclose (hdb);
      dbfreelogin (plogin);
      return NULL;
      }

   dbfreelogin (plogin);

   return pfLog;
   }



static HDB GetHDB (void)
   {
//   ItiSemRequestMutex (hsemHDBMutex, FOREVER);
   return hdb;
   }


/*
 * param is for later
 */

static void FreeHDB (HDB hdb)
   {
//   ItiSemReleaseMutex (hsemHDBMutex);
   }



static void FreeHQRY (HQRY hqry)
   {
   free (hqry);
   FreeHDB (hdb);
   }


#define MAX_QUERY_SIZE     2000
BOOL DbSendQueryPart (PSZ pszPart, USHORT *pusState)
   {
   if (pszPart == NULL || *pszPart == '\0')
      return FALSE;

   while (strlen (pszPart) > MAX_QUERY_SIZE)
      {
      char szTemp [MAX_QUERY_SIZE + 1];

      strncpy (szTemp, pszPart, sizeof szTemp - 1);
      szTemp [sizeof szTemp - 1] = '\0';
      if ((*pusState = dbcmd (hdb, szTemp)) == FAIL)
         {
         fprintf (pfLog,"unable to send dbcmd, uState = %u\n", *pusState);
         return FALSE;
         }

      pszPart += MAX_QUERY_SIZE;
      }

   /*--- passing the remaining sql text to the dblib ---*/
   if ((*pusState = dbcmd(hdb, pszPart)) == FAIL)
      {
      FreeHDB (hdb);
      fprintf (pfLog,"unable to process dbcmd, uState = %u\n", *pusState);
      return FALSE;
      }

   return TRUE;
   }



/*
 * If an error occurs, this fn returns NULL
 * and you can check uState for the error code. 
 * If tou get NULL but error code = SUCCEED, there was insuficient memory
 * currently hmod,uResId, and uId are not used. They are for the 
 * formatting which has yet to be implemented
 */
HQRY DbExecQuery (PSZ     pszQuery,
                  USHORT  *pusNumCols,
                  USHORT  *pusState)
   {
   HDB   hdb;
   HQRY  hqry;

   hdb = GetHDB ();

   if (!DbSendQueryPart (pszQuery, pusState))
      return NULL;

   if ((*pusState = dbsqlexec(hdb)) == FAIL)
      {
      FreeHDB (hdb);
      fprintf (pfLog,"unable to process dbsqlexec, uState = %u\n", *pusState);
      fprintf (pfLog, "Offending Query: %s\n", pszQuery);
      return NULL;
      }

   if ((*pusState = dbresults(hdb)) != SUCCEED)
      {
      FreeHDB (hdb);
      fprintf (pfLog,"Query did not succeed, usState=%u, query = \"%s\"\n", 
               *pusState, pszQuery);
      fprintf (pfLog, "Offending Query: %s\n", pszQuery);
      return NULL;
      }
   *pusNumCols = dbnumcols (hdb);

   if ((hqry = (HQRY) malloc (sizeof (QRY))) == NULL)
      {
      fprintf (pfLog, 
               "Could not allocate memory for an hqry!\n"
               "Offending Query: %s", pszQuery);
      return NULL;
      }
   hqry->hdb   = hdb;
   hqry->uNumCols = *pusNumCols;
   return hqry;
   }


/*
 * If an error occurs, this fn returns NULL
 * and you can check uState for the error code. 
 * If tou get NULL but error code = SUCCEED, there was insuficient memory
 * currently hmod,uResId, and uId are not used. They are for the 
 * formatting which has yet to be implemented
 */
BOOL DbExecSQLStatement (PSZ pszStatement)
   {
   HDB   hdb;
   USHORT usState;

   hdb = GetHDB ();

   /*--- passing the sql text to the dblib ---*/
   if (!DbSendQueryPart (pszStatement, &usState))
      {
      FreeHDB (hdb);
      fprintf (pfLog, "ERROR: unable to process dbcmd\n");
      fprintf (pfLog, "Offending Query: %s\n", pszStatement);
      return FALSE;
      }

   if ((usState = dbsqlexec(hdb)) == FAIL)
      {
      FreeHDB (hdb);
      fprintf (pfLog, "ERROR: unable to process dbsqlexec\n");
      fprintf (pfLog, "Offending Query: %s\n", pszStatement);
      return FALSE;
      }

   if ((usState = dbresults(hdb)) != SUCCEED)
      {
      FreeHDB (hdb);
      fprintf (pfLog, "Offending Query: %s\n", pszStatement);
      return FALSE;
      }

   do 
      {
      while (DbNextRow (hdb, &usState))
         ;
      } while (DbResults (hdb));

   FreeHDB (hdb);
   return TRUE;
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

static BOOL DbNextRow (HDB hdb, USHORT *uState)
   {
   if ((*uState = dbnextrow(hdb)) != MORE_ROWS)
      return FALSE;
   return TRUE;
   }

/* returns TRUE if there are more results */
static BOOL DbResults (HDB hdb)
   {
   return SUCCEED == dbresults (hdb);
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
                           USHORT *pusDestLen)
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
   if (pusDestLen)
      *pusDestLen = iLen;
   return 0;
   }

BOOL DbGetQueryRow (HQRY   hqry,
                    PSZ    **pppsz,
                    USHORT *pusState)
   {
   CHAR     szCol[1024];
   USHORT   i;

   *pppsz = NULL;
   if ((*pusState = dbnextrow(hdb)) != MORE_ROWS)
      {
      /* gobble up secondary query results */
      while (DbResults (hdb))
         {
         while (DbNextRow (hdb, pusState))
            ;
         }

      FreeHQRY (hqry);
      return FALSE;
      }
   if ((*pppsz = malloc (sizeof (PSZ) * hqry->uNumCols))== NULL)
      return FALSE;

   for (i = 0; i < hqry->uNumCols; i++)
      {
      DbGetFmtCol (hqry->hdb, i, szCol, sizeof szCol, NULL);
      (*pppsz)[i] = strdup (szCol);
      }
   return TRUE;
   }






void DbTerminateQuery (HQRY hqry)
   {
   DbCancel (hqry->hdb);
   FreeHQRY (hqry);
   return;
   }

