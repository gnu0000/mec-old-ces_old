/*
 * ad-hoc.c
 */

#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimbase.h"
#include "..\include\winid.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itiutil.h"
#include "..\include\dialog.h"
#include "..\include\itimenu.h"
#include "..\include\itifmt.h"
#include "..\include\itiest.h"
#include "..\include\colid.h"
#include <stdio.h>
#include "initcat.h"
#include "ad-hoc.h"
#include "dialog.h"



/*
 * ItiEstInstall Creates all the necessary tables, indicies, etc for both
 * the production database and import database.
 *
 * Returns TRUE on success, FALSE on error.
 */

BOOL EXPORT ItiEstInstall (PSZ szDatabaseName,
                           PSZ pszImportDatabase)
   {
   return TRUE;
   }


/*
 * ItiEstDeinstall deletes all the necessary tables, indicies, etc for both
 * the production database and import database.
 *
 * Returns TRUE on success, FALSE on error.
 */

BOOL EXPORT ItiEstDeinstall (PSZ szDatabaseName,
                             PSZ pszImportDatabase)
   {
   return TRUE;
   }



/*
 * ItiDllQueryVersion
 *
 */

USHORT EXPORT ItiDllQueryVersion (void)
   {
   return 1;
   }


VOID EXPORT ItiDllQueryMenuName (PSZ      pszBuffer,
                                 USHORT   usMaxSize,
                                 PUSHORT  pusWindowID)
   {
   *pszBuffer = 0;
   *pusWindowID = 0;
   }







/*
 * ItiEstTableChanged is used to tell an installable estimation method
 * that a table has changed.
 *
 * Parameters: usTable        The ID of the table that has changed.
 *
 *             ausColumns     An array from 0..usNumColumns, that
 *                            contain the IDs of the columns that
 *                            have changed.  If this parameter is
 *                            NULL, then all columns have changed.
 *
 *             usNumColumns   The number of columns in ausColumns.
 *
 *             pszKey         A pointer to a string that contains
 *                            the unique key.  The string is in the
 *                            form "ColumnName = ColumnValue
 *                            [AND ColumnName = ColumnValue]".
 */

void EXPORT ItiEstTableChanged (USHORT   usTable,
                                USHORT   ausColumns [],
                                USHORT   usNumColumns,
                                PSZ      pszKey)
   {
   }



/*
 * ItiEstItem returns a string to be used as the unit price for
 * a given standard item.  This function is usefull when the item has
 * not yet been added to the data base.
 *
 * Parameters: hheap                A heap to allocate from.
 *
 *             pszBaseDate          The base date for the price.  This
 *                                  parameter may be null iff pszJobKey
 *                                  is not NULL.  If this parameter and
 *                                  pszJobKey are supplied, pszBaseDate
 *                                  overrides the Job's base date.
 *
 *             pszJobKey            The job that the item is in.
 *                                  If the item is not in a job,
 *                                  set this parameter to NULL.
 *
 *             pszJobBreakdownKey   The job breakdown that the
 *                                  item is in.  If the item is not
 *                                  in a job, set this parameter to NULL.
 *
 *             pszStandardItemKey   The standard item to price.  This
 *                                  parameter may not be NULL.
 *
 *             pszQuantity          The quantity for this item.  If the
 *                                  quantity is unknown, use NULL.
 *
 * Return value:  A pointer to a null terminated string containing the
 * unit price.  The string is allocated from hheap.  Special return
 * values:
 *
 *    NULL:          An error occured estimating the item.
 *
 *    empty string:  Not enough information was supplied to estimate
 *                   the given item.
 *
 * Notes:  The caller is responsible for freeing the unit price from the
 * heap.
 *
 * The unit price is rounded according to the RoundingPrecision field
 * in the standard item catalog.
 */

PSZ EXPORT ItiEstItem (HHEAP hheap,
                       PSZ   pszBaseDate,
                       PSZ   pszJobKey,
                       PSZ   pszJobBreakdownKey,
                       PSZ   pszStandardItemKey,
                       PSZ   pszQuantity)
   {
   return NULL;
   }



BOOL EXPORT ItiDllInitDll (void)
   {
   return TRUE;
   }


BOOL EXPORT ItiEstQueryAvail (HHEAP hheap,
                              PSZ   pszStandardItemKey,
                              PSZ   pszBaseDate)
   {
   return TRUE;
   }

BOOL EXPORT ItiEstQueryJobAvail (HHEAP hheap,
                                 PSZ   pszJobKey,
                                 PSZ   pszJobItemKey)
   {
   return TRUE;
   }

VOID EXPORT ItiEstQueryMenuName (PSZ      pszBuffer,
                                 USHORT   usMaxSize,
                                 PUSHORT  pusWindowID)
   {
   *pszBuffer = 0;
   *pusWindowID = 0;
   }

