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


/* notes:
 *   ReadFile should check magic number      ok!
 *   
 */



/*
 * EBS.h
 *
 * (C) 1992 Info Tech Inc.
 * This file is the main header for the EBS module
 */


extern char sz        [512];


//#define EBSVER 100
//
//#define TIME  __TIME__
//#define DATED __DATE__
//
//#define IDMARK     "~@BIDIDMARK"
//#define IDSIZE     14
//#define NAMESIZE   40
//
//typedef long   double BIG;
//typedef BIG    *PBIG;
//typedef PSZ    *PPSZ;
//
//typedef USHORT  FLAG;
//typedef PUSHORT PFLAG;
//
//typedef ULONG DATE;
//typedef DATE  *PDATE;
//#define MARK  0x12345678L
//#define MAGIC 1357911L
//
//#define CONJUNCTION   "\n" 
//#define MAXITEMS      10000
//
//extern char szInFile  [256];
//extern char szOutFile [256];
//
//
////#define malloc  MyDosAllocSeg
////#define realloc MyDosReAllocSeg
////#define free    MyDosFreeSeg
//#define malloc(u)  MyDosAlloc2 (u,__FILE__,__LINE__)
//#define realloc(p,u) MyDosReAlloc2 (p,u,__FILE__,__LINE__)
//#define strdup(p) MyStrDup2 (p,__FILE__,__LINE__)
//
////  used in LAS input file
////  also used by make struct
////
//
//#define PR1    1
//#define PR2    2
//#define PRD    3
//#define PRJ    4
//#define AMD    5
//#define SEC    6
//#define SCD    7 
//#define IT1    8
//#define IT2    9
//#define ITS   10
//#define ITL   11
//#define END   12
//
//
//
//
//
//
//
///********************************************************************/
///*                                                                  */
///*  Proposal data structures used by most modules                   */
///*                                                                  */
///********************************************************************/
//                          //
//typedef struct            //
//   {                      //
//   USHORT uNum;           //
//   ULONG  dDate;          //
//   PSZ    pszDesc;        //
//   } NOTE;                //
//typedef NOTE *PNOTE;      //
//                          //
//typedef struct            //
//   {                      //
//   PSZ    pszLineNum;     //4
//   PSZ    pszAlt;         //4  
//   PSZ    pszItemNum;     //4
//   BIG    bgQuan;         //10 
//   PSZ    pszUnit;        //4  
//   BIG    bgUnitPrice;    //10 
//   BIG    bgExt;          //10 
//   FLAG   fFlags;         //2 See Below
//   PSZ    pszShortDesc;   //4  
//   PSZ    pszLongDesc;    //4 : 56
//   } ITEM;                //
//typedef ITEM *PITEM;      //
//typedef PITEM *PPITEM;    //
//                          //
//                          //
//typedef struct            //This structure is 
//   {                      //a subset of ITEM
//   BIG    bgQuan;         //that is copied to/from 
//   BIG    bgUnitPrice;    //the EBS file 
//   FLAG   fFlags;         //
//   } ITMIN;               //
//typedef ITMIN *PITMIN;    //
//                          //
//typedef struct            //
//   {                      //
//   ULONG  ulMagicNumber;  //4
//   DATE   dLettingDate;   //4
//   PSZ    pszLettingID;   //4  
//   USHORT uCallOrder;     //2
//   PSZ    pszContractID;  //4  
//   PSZ    pszDesc;        //4 MultiLine
//   DATE   dDateGenerated; //4
//   DATE   dDateRevised;   //4
//   USHORT uEBSVer;        //2
//   USHORT uRevisionCount; //2
//   USHORT uAmendCount;    //2
//   PSZ    pszBidderID;    //4
//   PSZ    pszBidderName;  //4  
//   FLAG   fFlags;         //2 See Below
//   ULONG  ulCRC;          //4 
//   PSZ    pszSerialNum;   //4
//   PSZ    pszStateName;   //4  
//   PSZ    pszAgencyName;  //4  
//   PSZ    pszSigBlk;      //4 Multiline
//   PSZ    pszProjects;    //4 Multiline
//   USHORT uNoteCount;     //2
//   PNOTE  pNote;          //4
//   USHORT uItemCount;     //2
//   PPITEM ppItem;         //4
//   BIG    bgBidTotal;     //10 : 112
//   } PROP;                //
//typedef PROP *PPROP;      //
//
//
//
//
///********************************************************************/
///*                                                                  */
///*  Flag Values                                                     */
///*                                                                  */
///********************************************************************/
//                          //
//                          //
////ITEM Flag Bit Values    //
////======================= //
//#define SECTION         0 // Tells that this struct is a section vs item
//#define FIXEDPRICE      1 // Tells price is fixed
//#define LUMPSUM         2 // Tells lump sum
//#define DELETE          3 // Used For Amendments to delete
//#define ISBID           4 // Used By check function
//#define ISALT           5 // Tells if bidding this item is an alternative
//#define CHANGED         6 // Tells if item has changed at add edendum
//                          //   The change flag is also used as a placeholder
//                          //   when generating amendment files
//
//                          //
////PROP Flag Bit Values    //
////========================//
//#define DIRTY           0 // Tells if proposal has been changed
//#define JOINTBID        1 // Tells if bid is joint
//#define INFO            2 // Tells if proposal is informational
//#define UNDERBID        3 // Tells that not enough bids entered?
//#define OVERBID         4 // Tells that too many bids entered  ?
//
//
