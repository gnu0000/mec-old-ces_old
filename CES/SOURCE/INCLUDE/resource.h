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
 * resource.h
 *
 */


/*
 * The following structure types are used
 * in the resource files
 */
typedef struct
   {
   SHORT    xLeft;
   SHORT    yBottom;
   SHORT    xRight;
   SHORT    yTop;
   } RECTS;


typedef struct
   {
   USHORT   uId;
   USHORT   uChildId;
   ULONG    ulKind;
   RECTS    rcs;
   USHORT   uCoordType;
   USHORT   uAddId;
   USHORT   uChangeId;
   } CD;     /* ChildData */


typedef struct
   {
   USHORT   uId;
   USHORT   uNextId;
   RECTS    rcs;
   USHORT   uCoordType;
   LONG     lFColor;
   ULONG    lFlags;
   USHORT   uOffset;        /* internal use */
   } LD;     /* Label Data */

typedef struct
   {
   USHORT   uId;
   USHORT   uNextId;
   RECTS    rcs;
   USHORT   uCoordType;
   LONG     lFColor;
   ULONG    lFlags;
   USHORT   uColumn;
   USHORT   uOffset;        /* internal use */
   } DD;     /* Data Field Data */


/*
 * DLGI - Dialog Info
 * Dialog Windows
 */
typedef struct         // Used by buildres an read by mbase
   {                   //
   USHORT   uId;       // Parents window id
   USHORT   uAddQuery;    // Add Query id
   USHORT   uChangeQuery; // Change Query id
   USHORT   uColId;    // Colid in query
   USHORT   uCtlId;    // Ctl Id of dialog box
   USHORT   uOffset;   // Internal use (offset to formats)
   } DLGI;
typedef DLGI FAR *PDLGI;

// use DG in itiwin.h
//
//typedef struct
//   {
//   USHORT   uId;
//   USHORT   uQuery;
//   USHORT   uColumn;
//   USHORT   uCtlId;
//   USHORT   uOffset;        /* internal use */
//   } DG;     /* Dialog Field Data */
//


typedef struct
   {
   USHORT   uId;
   RECTS    rcs;
   USHORT   uCoordType;
   USHORT   uOptions;
   LONG     lBColor;
   USHORT   uAddId;
   USHORT   uChangeId;
   } SM;     /* Static Metrics */



typedef struct
   {
   USHORT   uId;
   USHORT   uOptions;
   LONG     lBColor;
   LONG     lLBColor;
   USHORT   uLabelYSize;
   USHORT   uDataYSize;
   USHORT   uButtonId;
   } LM;     /* List Metrics */


typedef struct
   {
   USHORT   uId;
   USHORT   uOffset;
   } IDX;    /* index to strings */

typedef struct
   {
   USHORT   uId;
   USHORT   uOffset;
   USHORT   uCount;
   } IDX2;    /* index to strings */

typedef struct 
   {
   USHORT   uTableId;
   USHORT   uColumnId;
   USHORT   uOffset;
   } TABCOL;
