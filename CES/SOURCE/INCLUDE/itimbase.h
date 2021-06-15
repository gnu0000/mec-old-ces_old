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
 * metabase.h
 */

#define ITIMB_RELCOL      32768U


/* ---- RESOURCE TYPES --- */
#define ITIRT_CHILD       1000
#define ITIRT_LABEL       1001
#define ITIRT_LABELTXT    1002
#define ITIRT_DATA        1003
#define ITIRT_FORMAT      1004
#define ITIRT_SWMET       1005
#define ITIRT_LWMET       1006
#define ITIRT_QUERY       1007
#define ITIRT_TITLE       1008
#define ITIRT_DIALOG      1009
#define ITIRT_DLGFMAT     1010
#define ITIRT_COLUMNS     1011
#define ITIRT_TABLES      1012
#define ITIRT_TABLECOLUMN 1013


/* ---- RESOURCE ID'S --- */
#define ITIRID_NONE         0
#define ITIRID_WND          1
#define ITIRID_RPT          2
#define ITIRID_LOCK         3
#define ITIRID_UPDATE       4
#define ITIRID_INSERT       5
#define ITIRID_DELETE       6
#define ITIRID_IMPORT       7
#define ITIRID_EXPORT       8
#define ITIRID_CALC         9
#define ITIRID_EXPORTHDR   10
#define ITIRID_COPY        11

#if !defined (RC_INVOKED)

#if !defined (METABASE_INCLUDED)
#define METABASE_INCLUDED





#if !defined (EDT_DEFINED)
#define EDT_DEFINED
/*********************************************************************/
/* Information below defines the structures that are used by the     */
/* metabase module to pass required information to the windows       */
/*********************************************************************/

/*
 * EDT - Element Description Table
 * 2 arrays of these structures are used when initializing
 * list or static windows. 1 for the data elements and one for
 * the label elements
 */
typedef struct
   { USHORT uIndex;         // col # for data, str index for labels
     USHORT uNextId;        // window linked to this column (for dbl-clk)
     RECTL  rcl;            // bounding rectangle for data
     USHORT uCoordType;     // type of coordinates rcl is in
     LONG   lFColor;        // foreground color
     USHORT uFlags;         // text drawing flags
   } EDT;
typedef EDT FAR *PEDT;

///*
// * DLGI - Dialog Info
// * Dialog Windows
// */
//typedef struct         // Used by buildres an read by mbase
//   {                   //
//   USHORT   uId;       // Parents window id
//   USHORT   uQuery;    // Query id
//   USHORT   uColId;    // Colid in query
//   USHORT   uCtlId;    // Ctl Id of dialog box
//   USHORT   uOffset;   // Internal use (offset to formats)
//   } DLGI;
//typedef DLGI FAR *PDLGI;


/*
 * DGI - Dialog Info
 * Dialog Windows
 */
typedef struct         // Used by metabase in ItiMbGetDlgInfo
   {                   //
   USHORT   uAddQuery;    // Add Query id
   USHORT   uChangeQuery; // Change Query id
   USHORT   uColId;    // Colid in query
   USHORT   uCtlId;    // Ctl Id of dialog box
   USHORT   uOffset;   // Internal use (offset to formats)    /*--- internal use (offset to buffdat)---*/
   PSZ      pszInputFmt; // Input validation format
   } DGI;
typedef DGI FAR *PDGI;

#endif






#if defined (WINDOW_INCLUDED)

/* WNDMET - Static Window Metrics */
typedef struct
   { RECTL  rcl;
     USHORT uCoordType;
     USHORT uOptions;
     LONG   lBColor;
     USHORT uAddId;
     USHORT uChangeId;
   } SWMET;
typedef SWMET FAR *PSWMET;



/* WNDMET - List Window Metrics */
typedef struct
   { USHORT uOptions;
     LONG   lBColor;
     LONG   lLBColor;
     USHORT uLabelYSize;
     USHORT uDataYSize;
     USHORT uButtonId;
   } LWMET;
typedef LWMET FAR *PLWMET;




/*
 * Metabase Error Returns
 * 0 = no error
 * 1 = unable to find resource
 * 2 = no match found
 * 3 = Other Error
 */

extern USHORT EXPORT ItiMbGetChildInfo (  HMODULE hmod,
                                          USHORT  uResId,
                                          USHORT  uId,
                                          HHEAP   hheap,
                                          PCHD    *ppCHD,
                                          USHORT  *uNum);


extern USHORT EXPORT ItiMbGetLabelInfo (  HMODULE hmod,
                                          USHORT  uResId,
                                          USHORT  uId,
                                          HHEAP   hheap,
                                          PEDT    *ppEDT,
                                          PSZ     **pppsz,
                                          USHORT  *uNum);


extern USHORT EXPORT ItiMbGetDialogInfo  ( HMODULE hmod,
                                           USHORT  uResId,
                                           USHORT  uId,
                                           HHEAP   hheap,
                                           PDGI    *ppDGI,
                                           PSZ     **pppszOutput,
                                           USHORT  *uNum);


extern USHORT EXPORT ItiMbQueryStaticMetrics (HMODULE hmod,
                                              USHORT  uResId,
                                              USHORT  uId,
                                              PSWMET  pStaticMet);


extern USHORT EXPORT ItiMbQueryListMetrics ( HMODULE hmod,
                                             USHORT  uResId,
                                             USHORT  uId,
                                             PLWMET  pListMet);

extern void EXPORT ItiMbFreeCHD (HHEAP hheap, PCHD pchd);

extern void EXPORT ItiMbFreeEDT (HHEAP hheap, PEDT pedt);


#endif /* #if defined (WINDOW_INCLUDED) */


extern USHORT EXPORT ItiMbGetDataInfo  (HMODULE hmod,
                                        USHORT  uResId,
                                        USHORT  uId,
                                        HHEAP   hheap,
                                        PEDT    *ppEDT,
                                        PSZ     **pppsz,
                                        USHORT  *uNum);




/* returns 0 if OK, otherwise a meaningless number is returned on error */

extern USHORT EXPORT ItiMbQueryQueryText (HMODULE hmod,
                                          USHORT  uResId,
                                          USHORT  uId,
                                          PSZ     psz,
                                          USHORT  uSize);


extern USHORT EXPORT ItiMbQueryTitleText (HMODULE hmod,
                                          USHORT  uResId,
                                          USHORT  uId,
                                          PSZ     psz,
                                          USHORT  uSize);

extern USHORT EXPORT ItiMbGetFormatText (HMODULE hmod,
                                         USHORT  uResId,
                                         USHORT  uId,
                                         HHEAP   hheap,
                                         PSZ     **pppsz,
                                         USHORT  *puNum);


/* This fn is used to register a static window
 * This must be done before you can use the ItiWndRegisterStaticProc fn.
 * This fn stores the windows proc and the location of its metadata
 * As a minimum this proc must be called at least once with 
 * ITI_DEFAULTWND as the uId and the default fn for pfn and the default
 * hmod for hmod.
 */
extern USHORT EXPORT ItiMbRegisterStaticWnd (USHORT  uId,
                                             PFNWP   pfn,
                                             HMODULE hmod);



/* This fn is used to register a list window
 * This must be done before you can use the ItiWndRegisterListProc fn.
 * This fn stores the windows text proc and the location of its metadata
 * As a minimum this proc must be called at least once with 
 * ITI_DEFAULTWND as the uId and the default textfn for pfn and the default
 * hmod for hmod.
 */
#if !defined PFNTXT_DEFINED
#define PFNTXT_DEFINED
typedef int (EXPORT *PFNTXT) (HWND hwnd, USHORT uRow, USHORT uCol, PSZ *ppszText);
#endif

extern USHORT EXPORT ItiMbRegisterListWnd (USHORT      uId,
                                            PFNTXT      pfn,
                                            HMODULE     hmod);






extern USHORT EXPORT ItiMbRegisterDialogWnd (USHORT  uId,
                                              USHORT  uDlgId,
                                              PFNWP   pfn,
                                              HMODULE hmod);




/*
 * This procedure returns the hmod of the module which contains the 
 * metadata for the given window Id.
 */
extern USHORT EXPORT ItiMbQueryHMOD (USHORT uId, HMODULE *phmod);


/* This procedure returns a pfn for the 
 * static window uId.  If uId does not have a matching proc,
 * the default is returned.
 */
extern PFNWP EXPORT ItiMbStaticProc (USHORT uId, HMODULE *phmod);


/* This procedure returns a pfn for the 
 * list window edit proc.  If uId does not have a matching proc,
 * the default is returned.
 */
extern PFNTXT EXPORT ItiMbListEditProc (USHORT uId, HMODULE *phmod);



/*
 * This procedure returns a pfn for the 
 * static window uId.  If uId does not have a matching proc,
 * the default is returned.
 */
PFNWP EXPORT ItiMbDialogProc (USHORT uId, USHORT *uDlgId, HMODULE *phmod);



/* return values:  0 - window id not found
 *                 1 - static window
 *                 2 - list window
 *                 3 - dialog window
 */
USHORT EXPORT ItiMbQueryWindowType (USHORT uId);




#endif  /* if !defined (METABASE_INCLUDED) */

#endif  /* if !defined (RC_INVOKED) */








