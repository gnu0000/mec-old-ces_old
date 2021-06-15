
/*
 * ItiEstInstall Creates all the necessary tables, indicies, etc for both
 * the production database and import database.
 *
 * Returns TRUE on success, FALSE on error.
 */

extern BOOL EXPORT ItiEstInstall (PSZ szDatabaseName, 
                                  PSZ pszImportDatabase);


/*
 * ItiEstDeinstall deletes all the necessary tables, indicies, etc for both
 * the production database and import database.
 *
 * Returns TRUE on success, FALSE on error.
 */

extern BOOL EXPORT ItiEstDeinstall (PSZ szDatabaseName, 
                                    PSZ pszImportDatabase);


/*
 * ItiDllInitialize is automatically called by ItiDllLoad.
 */





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

extern void EXPORT ItiEstTableChanged (USHORT   usTable, 
                                       USHORT   ausColumns [],
                                       USHORT   usNumColumns,
                                       PSZ      pszKey);



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

extern PSZ EXPORT ItiEstItem (HHEAP hheap,
                              PSZ   pszBaseDate,
                              PSZ   pszJobKey,
                              PSZ   pszJobBreakdownKey,
                              PSZ   pszStandardItemKey,
                              PSZ   pszQuantity);
