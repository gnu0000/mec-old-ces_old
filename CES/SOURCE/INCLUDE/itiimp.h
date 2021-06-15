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
 * ItiImp.h
 * Mark Hampton
 * This file is part of DS/Shell.
 *
 * This file contains function and data type definition for import.
 */

#if !defined(INDV_SELECT)
#define INDV_SELECT      200
#define INDV_INSERT      300
#endif

/* standard query ids */
#define ITIIMP_BASE                    1000
#define ITIIMP_GETDISTINCTFKS          (ITIIMP_BASE + 0)
#define ITIIMP_UPDATECODEVALUE         (ITIIMP_BASE + 1)
#define ITIIMP_UPDATEAREATYPEKEY       (ITIIMP_BASE + 2)
#define ITIIMP_UPDATEAREAKEY           (ITIIMP_BASE + 3)
#define ITIIMP_UPDATESTANDARDITEMKEY   (ITIIMP_BASE + 4)
#define ITIIMP_UPDATE1                 (ITIIMP_BASE + 5)
#define ITIIMP_UPDATENAS               (ITIIMP_BASE + 6)
#define ITIIMP_ZERONULLSQRY            (ITIIMP_BASE + 7)
#define ITIIMP_GETDISTINCTFKSNONULL    (ITIIMP_BASE + 8)
#define ITIIMP_UPDATE2                 (ITIIMP_BASE + 9)
#define ITIIMP_UPDATECODEVALUE_CUR      (ITIIMP_BASE + 10)


#define ITIIMP_MSY_BASE                (ITIIMP_BASE + 100)
#define ITIIMP_MSY_GETDISTINCTFKS      (ITIIMP_MSY_BASE + 0)
#define ITIIMP_MSY_UPDATE              (ITIIMP_MSY_BASE + 5)
#define ITIIMP_KEYTEXT_UPDATE          (ITIIMP_MSY_BASE + 6)
#define ITIIMP_MI_KEYTEXT_UPDATE       (ITIIMP_MSY_BASE + 7)

#define ITIIMP_PARA_BASE               (ITIIMP_BASE + 110)
#define ITIIMP_PARA_GETDISTINCTFKS     (ITIIMP_PARA_BASE + 0)
#define ITIIMP_PARA_UPDATE             (ITIIMP_PARA_BASE + 5)
#define ITIIMP_PARA_QUERY              (ITIIMP_PARA_BASE + 6)
#define ITIIMP_PARA_UPDATE_IMP         (ITIIMP_PARA_BASE + 7)
#define ITIIMP_PARA_MIQTY_SEL_FK       (ITIIMP_PARA_BASE + 8)
#define ITIIMP_PARA_MIQTY_UPD_FK       (ITIIMP_PARA_BASE + 9)

#define ITIIMP_PARA_SIGMI_SEL_FK       (ITIIMP_PARA_BASE + 10)
#define ITIIMP_PARA_SIGMI_UPD_FK       (ITIIMP_PARA_BASE + 11)

#define ITIIMP_PARA_MIQ_SEL_FK         (ITIIMP_PARA_BASE + 12)
#define ITIIMP_PARA_MIQ_UPD_FK         (ITIIMP_PARA_BASE + 13)


#define ITIIMP_DOIMPORTDBFIRST   0x0001U  /* search import db first */
#define ITIIMP_DOIMPORTDBLAST    0x0002U  /* search import db last */
#define ITIIMP_NONAS             0x0004U  /* don't fill in NAs */
#define ITIIMP_ZERONULLS         0x0008U  /* zero out nulls */
#define ITIIMP_MULTISPECYRS      0x0010U  /* Use SpecYear modifications */
#define ITIIMP_PARAMETRIC        0x0020U  /* Parmetric module data */
#define ITIIMP_KEYTEXT           0x0040U  /* Indictator to extract     */
                                          /* SpecYear from XxxxKeyText */




#if !defined (RC_INVOKED)

#define ITIIMP_USE_MODIFICATIONS  1


/* maximum number of columns per table */
#define MAX_COLUMNS        64
#define ITIIMP_STR1_LEN    256

typedef LONG Key;


typedef struct _TABLEINFO *PTABLEINFO;


typedef struct _COLINFO *PCOLINFO;

typedef struct _IMPORTINFO *PIMPORTINFO;


typedef USHORT (EXPORT *PFNIMPFK) (HHEAP       hheap, 
                                   PIMPORTINFO pii,
                                   PCOLINFO    pci,
                                   PSZ         pszProdDatabase);

typedef USHORT (EXPORT *PFNCOPY) (HHEAP       hheap, 
                                  PIMPORTINFO pii,
                                  PSZ         pszProdDatabase);

typedef USHORT (EXPORT *PFNMERGE) (HHEAP       hheap, 
                                   PIMPORTINFO pii,
                                   PSZ         pszProdDatabase);

typedef USHORT (EXPORT *PFNCALC) (HHEAP       hheap, 
                                  PIMPORTINFO pii,
                                  PSZ         pszProdDatabase);



typedef struct _TABLEINFO
   {
   PSZ      pszTableName;           /* name of table */
   USHORT   usTableID;              /* table id */
   HMODULE  hmod;                   /* module of loaded dll */
   PFNCOPY  pfnCopy;                /* pointer to the copy function */
   PFNMERGE pfnMerge;               /* pointer to the merge function */
   PFNCALC  pfnCalc;                /* pointer to the calc function */
   } TABLEINFO;


typedef struct _COLINFO
   {
   PSZ      pszImportName; /* import file column name */
   PSZ      pszColumnName; /* name of column in database */
   USHORT   usColumnOrder; /* bcp order of column in database */
   PSZ      pszColumnData; /* pointer to column data */
   USHORT   usColumnMax;   /* maximum number of chars to store */
   PSZ      pszFormat;     /* formatting info for the column */
   PSZ      pszDefault;    /* default value for the column */
   BOOL     bNullable;     /* TRUE if the column is nullable */
   BOOL     bSysGenKey;    /* TRUE if the column is a sys gen key */
   Key      kForeignKey;   /* TableKey if the column is a foreign key */
   PFNIMPFK pfnFillInForeignKeys; /* foreign key fill in function */
   PSZ      pszFileColumnName; /* name of column in import file mar95 */
   } COLINFO;




typedef struct _IMPORTINFO
   {
   PTABLEINFO  pti;                 /* table info */
   PCOLINFO    pci;                 /* column info */
   USHORT      usFileColumns;       /* number of columns in import file */
   USHORT      usTotalColumns;      /* total number of columns to insert */
   ULONG       ulRows;              /* total number of rows loaded. */
   USHORT      usError;             /* Error code */
   } IMPORTINFO;


#define MAX_TABLE_NAME_SIZE   33
#define MAX_COLUMN_NAME_SIZE  33


#define ERROR_NO_MEMORY          1     /* no memory left! */
#define ERROR_BAD_TABLE          2     /* Bad table name */
#define ERROR_BAD_COLUMN         3     /* bad column name */
#define END_OF_FILE              4     /* End of file */
#define ERROR_TOO_MANY_COLUMNS   5     /* too many columns were imported */
#define ERROR_BAD_QUERY          6     /* bad query sent to server */
#define ERROR_METABASE           7     /* metabase data error */
#define ERROR_BCP_SET_TABLE      8     /* BcpSetTable failed */
#define ERROR_BCP_BIND           9     /* BcpBind failed */
#define ERROR_BCP_DONE           10    /* BcpDone failed -- duplicate rows? */
#define ERROR_NO_KEYS            11    /* could not get keys */
#define ERROR_CANT_FILL_IN_KEYS  12    /* could not fill in keys */
#define ERROR_NO_KEY             13    /* table does not have a key! */
#define ERROR_BAD_KEY            14    /* bad key value */
#define ERROR_DLL_LOAD_ERROR     15    /* dll could not load */
#define ERROR_DLL_FUNCTION_NOT_FOUND 16 /* function could not be found in dll */
#define ERROR_MUST_HAVE_DLL      17    /* must have foreign key dll */
#define ERROR_CANT_OPEN_TEMP_FILE 18   /* could not open temp file for highest conversion */
#define ERROR_HIGHEST_CSV_ERROR  19    /* error in HighEst CSV file */


extern USHORT EXPORT ItiImpIndividualExec (HHEAP   hheap,
                                           HMODULE hmod,
                                           PSZ     apszTokens [],
                                           PSZ     apszValues [],
                                           USHORT  usNumValues,
                                           USHORT  usInsert,
                                           USHORT  usSelect);


extern USHORT EXPORT ItiImpExec (HHEAP       hheap,
                                 PIMPORTINFO pii,
                                 PSZ         pszProdDatabase,
                                 HMODULE     hmod,
                                 USHORT      usQueryID);


extern USHORT EXPORT ItiImpFillInFK (HHEAP       hheap,
                                     HMODULE     hmod,
                                     PIMPORTINFO pii,
                                     PCOLINFO    pci,
                                     PSZ         pszProdDatabase,
                                     PSZ         pszTargetColumn,
                                     PSZ         pszCodeTableName,
                                     PSZ         pszProdTableName,
                                     PSZ         pszProdKeyName,
                                     PSZ         pszProdIDName,
                                     USHORT      usQueryID,
                                     USHORT      usUpdateID,
                                     USHORT      usFlags);


extern USHORT EXPORT ItiImpFillInNA (HHEAP       hheap,
                                     PIMPORTINFO pii,
                                     PCOLINFO    pci,
                                     PSZ         pszTargetColumn);
       
              
              
extern PSZ EXPORT ItiImpQueryImportDatabase (void);

#if defined (_FILE_DEFINED)
#define ItiCsvWriteField   ItiCSVWriteField
#define ItiCsvWriteLine    ItiCSVWriteLine
extern void EXPORT ItiCSVWriteField (FILE *pf, PSZ psz, int cTrail);
extern void CEXPORT ItiCSVWriteLine (FILE *pf, ...);
typedef BOOL (EXPORT *PFNEXP) (HHEAP       hheap, 
                               FILE        *pf,
                               PSZ         pszWhere);
extern BOOL EXPORT ItiExpExportTable (HHEAP    hheap, 
                                      HMODULE  hmod,
                                      FILE     *pf, 
                                      USHORT   usID,
                                      PSZ      pszWhere);
#else
extern void EXPORT ItiCSVWriteField (void *pf, PSZ psz, int cTrail);
extern void CEXPORT ItiCSVWriteLine (void *pf, ...);
typedef BOOL (EXPORT *PFNEXP) (HHEAP       hheap, 
                               void        *pf,
                               PSZ         pszWhere);

extern BOOL EXPORT ItiExpExportTable (HHEAP    hheap, 
                                      HMODULE  hmod,
                                      VOID     *pf, 
                                      USHORT   usID,
                                      PSZ      pszWhere);
#endif 


#endif 


