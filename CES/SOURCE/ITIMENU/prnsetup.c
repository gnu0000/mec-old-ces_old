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
 * prnsetup.c
 * Copyright (C) 1992 Info Tech, Inc.
 *
 */


#define INCL_DEV

#define INCL_WIN
#define INCL_DOSMISC
#include <stdlib.h> 
#include <string.h>
#include <stdio.h>
#include "..\include\iti.h"
#include "..\include\itiglob.h"
#include "window.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itiutil.h"
#include "..\include\colid.h"
#include "..\include\itierror.h"
#include "..\include\dialog.h"
#include "..\include\itirpt.h"
#include "dialog.h"
#include "init.h"
#include "print.h"


/* Notice: The bulk of this file's code was adapted from MS OS/2 v1.2 PMTools
 *         sample code.  The following quote concerning usage is from the
 *         their print.doc file (located in g:\source\pmtools\stock).
 *
 * "NOTE: Developers may copy the sample code in PRINT.C and PRINT.H and use it
 *        in their applications. Developers will need to change the memory-
 *        management strategy employed by the sample code, but otherwise
 *        the code is usable without modifications. "
 *
 *
 *
 *
 */




#define DLL_VERSION   1


//  index values for PrintQueryInfo

#define PQPI_NAME		      1
#define PQPI_LOG_ADDRESS	2
#define PQPI_DRIVER		   3
#define PQPI_MODEL		   4
#define PQPI_TYPE		      5
#define PQPI_PRINTER		   6
#define PQPI_DESCRIPTION	7



#define IDL_PRINTER		501
#define IDB_SETUP		502
#define IDB_RESET		503



typedef VOID *HPRINTERLIST; /* hprtlist */
typedef VOID *HPRINTER;     /* hprt */



typedef struct
   {    /* glb */
   char	 achAppTitle[CCHMAXPATH]; // Application title
   BOOL	 bNewJobProp;	// TRUE => job properties changed
   HAB 	 hab;
   HWND	 hwndFrame;
   HWND	 hwndClient;
   HPRINTER	 hprt;		// Current printer handle
   HPRINTERLIST hprtlist;	// Handle of printer list
   } GLOBAL;




// SEARCHP - Search structure for PrinterBegin/Next/EndSearch
//
typedef struct
   { /* srchp */
   char *  pchName;			// Driver name list
   char *  pchNext;			// Next name in achNames to consume
   } SEARCHP;

typedef SEARCHP *PSEARCHP; /* psrchp */


// PRINTER - Structure for printer destination (used for DevOpenDC)
//
typedef struct _printer
   {  /* prt */
   LONG    lType;			// Printer type (OD_DIRECT/OD_QUEUED)
   char *  pszName;			// Name (either queue or printer name)
				//  NOTE: This pointer always points
				//	  to either pszLogAddr or
				//	  pszPrinter.  This pointer
				//	  should never be freed!
   char *  pszLogAddr; 		// Logical address for DevOpenDC
   char *  pszPrinter; 		// Printer Name
   char *  pszDriver;			// Driver name
   char *  pszModel;			// Driver model name
   char *  pszDescription;		// Description
   BOOL    bMultipleDrivers;		// TRUE => pszDriver/pszModel contain
				//	the first driver in the printer
				//	definition.  If this is a queue
				//	(lType == OD_QUEUED), the
				//	driver/model must be gotten
				//	from PM_SPOOLER_QUEUE_DD.
   USHORT  cbDriverData;		// Length of driver data
   BYTE *  pbDriverData;		// Driver data
   struct _printer *pprtNext;		// Link to next printer
   struct _printer *pprtPrev;		// Link to previous printer
   struct _printerlist *pprtlist;	// Owning list
   } PRINTER;

typedef PRINTER *PPRINTER;  /* pprt */


//  PRINTERLIST - Structure for list of printers
//
typedef struct _printerlist
   { /* prtlist */
   HAB 	hab;	      		// Anchor block for list
   PPRINTER	pprtHead;		// Head of printer list
   PPRINTER	pprtTail;		// Tail of printer list
   PPRINTER	pprtDefault;  	// System default printer
   } PRINTERLIST;

typedef PRINTERLIST *PPRINTERLIST; /* pprtlist */


//  JOBPROP - Structure for exporting/importing job properties
//
typedef struct
   {    /* jp */
   USHORT  cb; 			// Total buffer size
   USHORT  cbName;			// Size of printer name (counting NUL)
   USHORT  cbDriver;			// Size of driver name (counting NUL)
   USHORT  cbModel;			// Size of model name (counting NUL)
   USHORT  cbData;			// Size of driver data
 //
 // The "fields" below show the order in which the strings/data follow the
 // structure "header":
 //
 // CHAR    achName[];			// Printer name (asciiz)
 // CHAR    achDriver[];		// Driver name (asciiz)
 // CHAR    achModel[]; 		// Model name (asciiz)
 // BYTE    abData[];			// Driver data (binary)
 //
   } JOBPROP;

typedef JOBPROP *PJOBPROP;  /* pjp */



static	GLOBAL	 glb;
static	HPOINTER  hptrArrow;
static	HPOINTER  hptrWait;
static   BOOL      bNewJobProp;
static	char      achPath[CCHMAXPATH];
static	char	    achMisc[CCHMAXPATH];











/***	TrimTrailingSemicolon - truncate string at first semicolon
 *
 *	Entry
 *	    psz - string to truncate
 *	Exit
 *	    String truncated at first semicolon (if any)
 */
void TrimTrailingSemicolon(PSZ psz)
   {
   while ((*psz != '\0') && (*psz != ';'))
	   psz++;
   *psz = '\0';
   }



/*	MatchPrinter - Look for printer in list
 *
 *	Entry
 *	    pprtlist   - Printer list
 *	    pszQueue   - Queue name to match (may be NULL)
 *	    pszPrinter - Printer name to match (may be NULL)
 *
 *	Exit-Success
 *	    Returns TRUE
 *
 *	Exit-Failure
 *	    Returns FALSE
 */
PPRINTER MatchPrinter(PPRINTERLIST pprtlist, PSZ pszQueue, PSZ pszPrinter)
   {
   PPRINTER	pprt;

   for (pprt=pprtlist->pprtHead; pprt!=NULL; pprt=pprt->pprtNext)
      {
   	if ((pprt->lType == OD_QUEUED) && (pszQueue != NULL))
         { // A queue
         if (ItiStrCmp(pprt->pszLogAddr,pszQueue) == 0)
	      	return pprt;
      	}
	   else if (pszPrinter != NULL)
         {	// A direct printer
   	   if (ItiStrCmp(pprt->pszPrinter,pszPrinter) == 0)
	   	   return pprt;
   	   }
      }
   return NULL;
   }





/***	DestroyPrinter - Destroy a PRINTER
 *
 *	Entry
 *	    pprt - printer to destroy
 *
 *	Exit
 *	    NONE
 */
VOID DestroyPrinter(PPRINTER pprt)
   {
   if (pprt->pszLogAddr != NULL)
	   ItiMemFree(hheapGlobal, pprt->pszLogAddr);

   if (pprt->pszDriver != NULL)
	   ItiMemFree(hheapGlobal, pprt->pszDriver);

   if (pprt->pszModel != NULL)
	   ItiMemFree(hheapGlobal, pprt->pszModel);

   if (pprt->pszPrinter != NULL)
	   ItiMemFree(hheapGlobal, pprt->pszPrinter);

   if (pprt->pszDescription != NULL)
   	ItiMemFree(hheapGlobal, pprt->pszDescription);

   if (pprt->pbDriverData != NULL)
	   ItiMemFree(hheapGlobal, pprt->pbDriverData);

   ItiMemFree(hheapGlobal, pprt);
   }





/***	AddPrinter - Add printer to printer list (if unique)
 *
 *	NOTE: If the printer is not added, it is freed!
 *
 *	Entry
 *	    pprtlist - printer list
 *	    pprt     - printer to be added
 *
 *	Exit-Success
 *	    Returns TRUE
 *
 *	Exit-Failure
 *	    Returns FALSE
 */
BOOL AddPrinter(PPRINTERLIST pprtlist,PPRINTER pprtNew)
   {
   PPRINTER	pprt;

   // Avoid duplicate entry

   if (pprtNew->lType == OD_QUEUED)
      {	// Queue, check for duplicate
   	pprt = MatchPrinter(pprtlist,pprtNew->pszLogAddr,NULL);
   	if (pprt != NULL)
         {		// Found a match
   	   pprt->bMultipleDrivers = TRUE; // Force query of driver/model
	      DestroyPrinter(pprtNew);	// No need for new printer
	      return TRUE;		// Do not add printer
      	}
      }
    else
      {				// Direct printer
   	// Do nothing, since the Print Manager/Spooler never produces
	   // duplicate printers.
   	;				// Do nothing
      }

   // Link onto list

   pprtNew->pprtPrev = pprtlist->pprtTail; // Set previous printer
   if (pprtlist->pprtHead == NULL)	// List is empty
      pprtlist->pprtHead = pprtNew;	// Put at head of list
   else
	   pprtlist->pprtTail->pprtNext = pprtNew; // Add at end of list

   pprtlist->pprtTail = pprtNew;	// New tail of list
   pprtNew->pprtNext = NULL;		// Set next printer

   pprtNew->pprtlist = pprtlist;	// Point at owning list
   return TRUE;
   }




/***	GetDriverModel - Get Driver and Model for a printer
 *
 *	Entry
 *	    pprt - printer
 *
 *	Exit
 *	    If printer was a queue, and the existing printer/model may
 *	    not be the one the user setup, get it from the HINI_SYSTEM.
 *
 *	Uses
 *	    achPath
 *
 *	NOTE 1:
 *	    We *need* to query the PM_SPOOLER_QUEUE_DD section *only* if the
 *	    queue might have more than one driver associated with it.  This
 *	    could happen in several cases:
 *
 *		1)  If the queue is associated with a single printer, but
 *			that printer has more than one driver/model pair.
 *
 *		2)  If the queue is associated with more than one printer,
 *			and the set of driver/model pairs supported by
 *			these printers has more than one member.
 *
 *	    (1) Is easy to compute (we keep a flag when we build the PRINTER
 *		object).
 *
 *	    (2) Is more complicated, so we compute a cheaper version:
 *
 *		2a) If the queue is associated with more than one printer.
 *
 *		We compute this in AddPrinter.	If AddPrinter tries to add
 *		a PRINTER that has a pszLogAddr that matches one already
 *		added, then it knows we have a queue pointing to more than
 *		one printer.  So, it sets the bMultipleDrivers flag, and then
 *		the code below can use that single flag to determine if it
 *		has to get the driver/model from HINI_SYSTEM).
 *
 *	NOTE 2:
 *	     For an OD_DIRECT printer (no queue) there is no way to figure
 *	     out which driver/model pair to use, so we take the first one
 *	     in the list!  Since that is what we did when we built the
 *	     printer object initialy, we have nothing to do.
 */
VOID GetDriverModel(PPRINTER pprt)
   {
   LONG cb;
   PSZ  psz;
   PSZ  pszDriver;
   PSZ  pszModel;


   if (pprt->lType != OD_QUEUED)	// Not a queue
	   return; 			            // Driver.model is correct

   if (!pprt->bMultipleDrivers)	// Only one driver
	   return; 			            // No change needed

   // We have a queue with multiple driver.model choices

   pprt->bMultipleDrivers = FALSE;	// Only do this computation once

   // Get driver.model for this queue

   cb = PrfQueryProfileString(HINI_SYSTEM,		// system ini
                  	   "PM_SPOOLER_QUEUE_DD",	// app section
                  	   pprt->pszName,		      // printer name
                  	   "", 			            // Default value
                  	   achPath,			         // Buffer to receive value
                  	   (ULONG)sizeof(achPath));// Buffer size
   if (cb <= 1)  // No driver.model
	   return; 	  // Use existing one

   // Parse driver.model string

   psz = achPath;
   // A: "IBM4201;"
   //	   ^
   // B: "LASERJET.HP LaserJet IID;"
   //	   ^

   // Parse the driver and model names

   pszDriver = psz;			// Set driver name
   psz = strpbrk(psz,".;");// Find separator
   if (psz == NULL)			// Bad format
	   return; 			      //  Exit without changing

   if (*psz == '.')
      {			                     // Found model name separator
   	*psz = '\0';			         // Null terminate driver name
	   pszModel = psz + 1;	         // Set model name
   	psz = strpbrk(pszModel,";");	// Find end of model name
	   if (psz == NULL)		         // Bad format
	      return;			            // Exit without changing
      }
   else				            // No model name
	   pszModel = pszDriver;	// Make model same as driver name

    *psz = '\0';			      // Null terminate driver name

   // Create string copies

   pszDriver = ItiStrDup(hheapGlobal, pszDriver);
   if (pszDriver == NULL)
      return;

   pszModel = ItiStrDup(hheapGlobal, pszModel);
   if (pszModel == NULL)
      {
   	ItiMemFree(hheapGlobal, pszDriver);
	   return;
      }

   // Update printer structure

   if (pprt->pszDriver != NULL)
	   ItiMemFree(hheapGlobal, pprt->pszDriver);
   if (pprt->pszModel != NULL)
   	ItiMemFree(hheapGlobal, pprt->pszModel);

   pprt->pszDriver = pszDriver;
   pprt->pszModel = pszModel;
   }




/***	GetDescription - Get description from HINI_SYSTEM
 *
 *	Get printer description or queue description, as appropriate.
 *
 *	Entry
 *	    pprt - printer
 *
 *	Exit
 *	    Set description from HINI_SYSTEM.
 *
 *	Uses
 *	    achPath
 */
VOID GetDescription(PPRINTER pprt)
   {
   LONG    cb;
   char *  psz;
   char *  pszApp;

   if (pprt->lType == OD_QUEUED)    // Queue
	   pszApp = "PM_SPOOLER_QUEUE_DESCR";
   else
	   pszApp = "PM_SPOOLER_PRINTER_DESCR";

   cb = PrfQueryProfileString(HINI_SYSTEM,       // System ini
	  	   	    pszApp,			            // App
	  	   	    pprt->pszName,		      // Queue/printer name
	  	   	    "", 		               	// Default value
	  	   	    achPath,			         // Buffer to receive value
	  	   	    (ULONG)sizeof(achPath));	// Buffer size

   if (cb <= 1)			// No description
	   return; 			// Use existing one

    // Parse description

    psz = achPath;
    psz = strpbrk(psz,";");		// Find end of description
    if (psz == NULL)			// Bad format
	return; 			//  Exit without changing
    *psz = '\0';			// Null terminate desription

    // Duplicate description

    psz = ItiStrDup(hheapGlobal, achPath);
    if (psz == NULL)
	return;

    if (pprt->pszDescription != NULL)
	ItiMemFree(hheapGlobal, pprt->pszDescription);
    pprt->pszDescription = psz;
}









/***	CreatePrinter - Create a PRINTER
 *
 *	Entry
 *	    lType      - printer type (OD_DIRECT or OD_QUEUED)
 *	    pszLogAddr - logical address (LPT1 or QUEUE1)
 *	    pszPrinter - printer name
 *	    pszDriver  - driver name
 *	    pszModel   - model name
 *
 *	Exit-Success
 *	    Returns PPRINTER
 *
 *	Exit-Failure
 *	    Returns NULL
 */
PPRINTER CreatePrinter(LONG lType,    PSZ pszLogAddr, PSZ pszPrinter,
         		        PSZ pszDriver, PSZ pszModel,   BOOL bMultipleDrivers)
   {
   PPRINTER	pprt;

   pprt = ItiMemAlloc(hheapGlobal, sizeof(PRINTER), MEM_ZERO_INIT);
   if (pprt == NULL)
   	return NULL;

   // Create copies of strings to complete printer structure

   pprt->pszLogAddr = ItiStrDup(hheapGlobal, pszLogAddr); // Duplicate logical address
   pprt->pszPrinter = ItiStrDup(hheapGlobal, pszPrinter); // Duplicate printer name
   pprt->pszDriver  = ItiStrDup(hheapGlobal, pszDriver);  // Duplicate driver
   pprt->pszModel   = ItiStrDup(hheapGlobal, pszModel);  // Duplicate Model

   // Set other fields

   pprt->bMultipleDrivers = bMultipleDrivers; // Set flag
   pprt->lType = lType;		                   // Set type
   pprt->pszDescription = NULL;	             // No description, yet
   pprt->cbDriverData = 0;		                // No job properites
   pprt->pbDriverData = NULL;		             // No job properites

   // Set consistent printer "name"
   if (lType == OD_QUEUED)
	   pprt->pszName = pprt->pszLogAddr; // Use queue name
   else
   	pprt->pszName = pprt->pszPrinter; // Use printer name

   // Make sure copies succeeded

   if ((pprt->pszLogAddr     == NULL)  // One or more dups failed
	    || (pprt->pszDriver   == NULL) 
       || (pprt->pszModel    == NULL) 
       || (pprt->pszPrinter  == NULL))
      {
   	DestroyPrinter(pprt);		// Destroy printer
	   pprt = NULL;
      }

   return pprt;
   }/* End of function CreatePrinter */








/***	ClonePrinter - Create a duplicate PRINTER with a different pszLogAddr
 *
 *	Entry
 *	    pprt       - printer to clone
 *	    pszLogAddr - logical address to set (QUEUE1)
 *
 *	Exit-Success
 *	    Returns PPRINTER
 *
 *	Exit-Failure
 *	    Returns NULL
 */
PPRINTER ClonePrinter(PPRINTER pprtOriginal,char *pszLogAddr)
   {
   PPRINTER	pprt;

   pprt = CreatePrinter(pprtOriginal->lType,		   // Copy type
								pszLogAddr,			         // New logical address
								pprtOriginal->pszPrinter,	// Copy printer
								pprtOriginal->pszDriver,	// Copy driver
								pprtOriginal->pszModel, 	// Copy model
								pprtOriginal->bMultipleDrivers);	// Copy flag
   return pprt;
   } /* End of function ClonePrinter */









/***	GetPrinterData - Get printer data
 *
 *	Entry
 *	    achPrinter - printer name
 *
 *	Exit-Success
 *	    returns PPRINTER
 *
 *	    NOTE 1: if lType == OD_QUEUED, then pprt->pszLogAddr is one or
 *		    more queue names (separated by ",").
 *
 *		    The caller should clone the PRINTER structure as many
 *		    times as necessary so that there is one for each queue.
 *
 *	    NOTE 2: pprt->pszDescription is not filled in.
 *
 *		    After the caller solves Note 1, the description should
 *		    be retrieved from OS2.INI.	Use PM_SPOOLER_QUEUE_DESCR
 *		    for a queue, and PM_SPOOLER_PRINTER_DESCR for a port
 *		    (both in HINI_SYSTEM)
 *
 *	    NOTE 3: pprt->pszDriver/pszModel may not be correct
 *
 *		    After the caller solves Note 1, for each queue the
 *		    driver/model should be retrieved from PM_SPOOLER_QUEUE_DD
 *		    (in HINI_SYSTEM).
 *
 *	Exit-Failure
 *	    Returns NULL
 *
 *	Uses
 *	    achPath
 */
PPRINTER GetPrinterData(char *pszPrinter)
   {
   USHORT	cb;
   BOOL	   bComma;// True if multiple drivers
   ULONG	   lType;
   PPRINTER	pprt;
   char *	psz;
   char *	pszDriver;
   char *	pszLogAddr;
   char *	pszModel;
   char *	pszQueue;

   // Get the printer details
   cb = (USHORT)PrfQueryProfileString(HINI_SYSTEM,	// Only look in system profile
                  "PM_SPOOLER_PRINTER",	         // Application name
            	   pszPrinter, 		               // Keyname
            	   "",                     			// Default value
            	   achPath,			                  // Buffer to receive value
            	   (ULONG)sizeof(achPath));	      // Buffer size

   if (cb == 0)
	   return NULL;

   // Get the logical address name, driver name, model name, and queue name

   psz = achPath;

   // Parse off logical address (usually a port name)
   //
   // A: "LPT1;LASERJET.HP LaserJet IID;QUEUE1;;"
   //	   ^
   // B: "LPT1;LASERJET.HP LaserJet IID,PSCRIPT;QUEUE1,QUEUE2;;"
   //	   ^

   pszLogAddr = achPath;		// Set logical address
   psz = strchr(psz,';');		// Find end of log addr
   if (psz == NULL)		   	// Bad format
	   return NULL;			   // FAIL

   *psz = '\0'; 	// Null terminate log addr
   psz++;		 	// Skip separator

   // Parse off driver name and model
   //
   // NOTE:  These are provisional.  For a print queue, the driver/model
   //	      have to be retrieved (by the caller) from PM_SPOOLER_QUEUE_DD
   //	      in HINI_SYSTEM.
   //
   // A: "LPT1_LASERJET.HP LaserJet IID;QUEUE1;;"
   //		^
   // B: "LPT1_LASERJET.HP LaserJet IID,POSTSCRIPT;QUEUE1,QUEUE2;;"
   //		^

   pszDriver = psz;			   // Set driver name
   psz = strpbrk(psz,".,;");	// Find separator
   if (psz == NULL)			   // Bad format
	   return NULL;			   //  FAIL
   bComma = FALSE;			   // Assume no comma

   if (*psz == '.')
      {			// Found model name separator
   	*psz = '\0';			// Null terminate driver name
	   pszModel = psz + 1;		// Set model name
   	psz = strpbrk(pszModel,",;");	// Find end of model name

	   if (psz == NULL)		// Bad format
	      return NULL;		//  FAIL
      }
    else if (*psz == ',' || *psz == ';') // No model name
   	pszModel = pszDriver;		// Make model same as driver name


    // A: "LPT1_LASERJET_HP LaserJet IID;QUEUE1;;"
    //					^
    // B: "LPT1_LASERJET_HP LaserJet IID,POSTSCRIPT;QUEUE1,QUEUE2;;"
    //					^
    if (*psz == ',')	// More drivers, remember to skip them
       bComma = TRUE;

    *psz = '\0';		// Null terminate driver name
    psz++;				// Skip separator

    // A: "LPT1_LASERJET_HP LaserJet IID_QUEUE1;;"
    //					 ^
    // B: "LPT1_LASERJET_HP LaserJet IID_POSTSCRIPT;QUEUE1,QUEUE2;;"
    //					 ^
    if (bComma)
      {			// Need to skip rest of field
   	psz = strpbrk(psz,";"); // Skip to end of driver field
	   if (psz == NULL)		   // Bad format
	      return NULL;		   // FAIL
   	psz++;   				   // Skip separator
      }

    // A: "LPT1_LASERJET_HP LaserJet IID_QUEUE1;;"
    //						^
    // B: "LPT1_LASERJET_HP LaserJet IID_POSTSCRIPT;QUEUE1,QUEUE2;;"
    //						    ^
    // NOTE: We leave the list of queues, so that caller can enumerate
    //	     all queues!
    //
   if (*psz == ';')			// No queue
	   pszQueue = NULL;
   else
      {				// Queue present
   	pszQueue = psz; 		// Set queue name
	   psz = strpbrk(psz,";"); 	// Find end of queue name(s)!
   	if (psz == NULL)		// Bad format
	      return NULL;		//  FAIL
   	*psz = '\0';			// Null terminate queue name
      }

   /* -- Figure out whether this printer is queued or not. */
   if (pszQueue != NULL)
      {		// Printer is queued
   	lType = OD_QUEUED;
	   psz = pszQueue;
      }
   else
      {				// Printer is direct
   	lType = OD_DIRECT;
	   psz = pszLogAddr;
      }


   /* -- Create printer structure. */
   pprt = CreatePrinter(lType,		// Set type
	                     psz,			// Set logical address
                  	   pszPrinter, // Set printer
                  	   pszDriver,	// Set driver
                  	   pszModel,	// Set model
                  	   bComma);		// Set flag
   return pprt;
   }




/***	GetDriverData - Get driver data for printer (if not already set)
 *
 *	This function attempts to set the Job Properties for a printer
 *	by querying the driver.  If successful, the Job Properties are
 *	stored in the printer.	Otherwise, they are left as NULL, and
 *	the driver will use its own default job properties.
 *
 *	Entry
 *	    pprt - printer
 *
 *	Exit
 *	    pprt->pbDriverData set to driver data, if at all possible.
 */
VOID GetDriverData (PPRINTER pprt)
   {
   HAB     hab;
   LONG    cb;
   BYTE *  pb;
   LONG    rc;

   hab = pprt->pprtlist->hab;

   // See if job properties already set

   if (pprt->pbDriverData != NULL)	// They are
	   return; 			//  Nothing to do

   // Assume no job properties

   pprt->cbDriverData = 0;
   pprt->pbDriverData = NULL;

   // Get size

   cb =  DevPostDeviceModes(hab,			// Anchor block
	  	   	   	   	    NULL,		// Buffer for data (NULL returns size)
	  	   	   	   	    pprt->pszDriver,	   // Driver name
	  	   	   	   	    pprt->pszModel, 	   // Device name
	  	   	   	   	    pprt->pszPrinter,   // Printer name
	  	   	   	   	    DPDM_QUERYJOBPROP);	// Option

   if (cb == DPDM_NONE)		// No job properties available
	   return; 		  

   if (cb == DPDM_ERROR) 	// Could not get job properties
	   return; 		  

   pb = ItiMemAlloc(hheapGlobal, (USHORT)cb, MEM_ZERO_INIT); // Allocate buffer
   if (pb == NULL)			   // Could not allocate it
   	return; 		            //  Nothing to do

   // Made it this far, get driver data

   rc = DevPostDeviceModes(hab,			         // Anchor block
	   	    	    	      (VOID *)pb, 		   // Buffer for data
	   	    	    	      pprt->pszDriver,		// Driver name
	   	    	    	      pprt->pszModel,		// Device name
	   	    	    	      pprt->pszPrinter,		// Printer name
	   	    	    	      DPDM_QUERYJOBPROP);  // Option

    if (rc == DEV_OK)
       {
       pprt->cbDriverData = (USHORT)cb;
	    pprt->pbDriverData = pb;
       }
   return;
   }/* End of function GetDriverData */







/***	PrinterNextSearch - Continue enumeration of installed printers
 *
 *	Entry
 *	    psrchp - search handle from PrinterBeginSearch
 *
 *	Exit-Success
 *	    Returns PPRINTER
 *
 *	Exit-Failure
 *	    Returns NULL
 */
PPRINTER PrinterNextSearch(PSEARCHP psrchp)
   {
   PPRINTER	pprt=NULL;

    // Scan printers until we find a valid one, or the list is exhausted
   while ((*psrchp->pchNext != '\0') && (pprt == NULL))
      {
   	pprt = GetPrinterData(psrchp->pchNext); // Get printer
	   psrchp->pchNext += strlen(psrchp->pchNext)+1; // Next printer
      }
   return pprt;
   }





/***	PrinterEndSearch - End enumeration of installed printer drivers
 *
 *	Entry
 *	    psearchp - search handle from PrinterBeginSearch
 *
 *	Exit-Success
 *	    Returns TRUE
 *
 *	Exit-Failure
 *	    Returns FALSE
 */
BOOL PrinterEndSearch(PSEARCHP psrchp)
   {
   if (psrchp == NULL)
	   return TRUE;

   if (psrchp->pchName != NULL)
	   ItiMemFree(hheapGlobal, psrchp->pchName);

   ItiMemFree(hheapGlobal, psrchp);
   return TRUE;
   }







/***	PrinterBeginSearch - Start enumeration of installed printers
 *
 *	Entry
 *	    ppprt - pointer to receive pointer to printer
 *
 *	Exit-Success
 *	    Returns search handle, ppprt filled in
 *
 *	Exit-Failure
 *	    Returns NULL
 */
PSEARCHP PrinterBeginSearch(PPRINTER *ppprt)
   {
   ULONG	cb;
   ULONG	cb1;
   BOOL	f;
   PSEARCHP	psrchp;

   // Get size of buffer for printer names

   f = PrfQueryProfileSize(
	   HINI_SYSTEM,		// Only look in system profile
	   "PM_SPOOLER_PRINTER",	// Pointer to application name
	   NULL,			// Pointer to keyname
	   &cb 			// Pointer to return buffer size
	   );
   if (!f)
	   return NULL;			// Failed

   // Create search structure

   psrchp = ItiMemAlloc(hheapGlobal, sizeof(SEARCHP), MEM_ZERO_INIT);
   if (psrchp == NULL)
	   return NULL;
   psrchp->pchNext = NULL;

   // Allocate buffer for names

   psrchp->pchName = ItiMemAlloc(hheapGlobal, (USHORT)cb, MEM_ZERO_INIT);
   if (psrchp->pchName == NULL)
      {
   	PrinterEndSearch(psrchp);	// Frees search structure
	   return NULL;			// Failed
      }

   // Get all printer names

   cb1 = PrfQueryProfileString(
	   HINI_SYSTEM,		// Only look in system profile
	   "PM_SPOOLER_PRINTER",	// Application name
	   NULL,			// Keyname
	   "", 			// Default value
	   psrchp->pchName,		// Name list to be returned
	   cb				// Buffer size
	   );

   if (cb1 == 0)
      {		// No printers installed
	   PrinterEndSearch(psrchp);
	   return NULL;
      }

   // Get first printer

   psrchp->pchNext = psrchp->pchName;	// Start at first printer
   *ppprt = PrinterNextSearch(psrchp); // Get a printer
   if (*ppprt != NULL) 		// Found one
	   return psrchp;			//  Return success
   else
      {
   	PrinterEndSearch(psrchp);	// No printer found
	   return NULL;			//  Return failure
      }
   }








/***	PrintDestroyPrinterList - Destroy printer list
 *
 *	Entry
 *	    hprtlist - printer list
 *
 *	Exit-Success
 *	    Returns TRUE, list destroyed
 *
 *	Exit-Failure
 *	    Returns FALSE
 */
BOOL	  PrintDestroyPrinterList(HPRINTERLIST hprtlist)
   {
   PPRINTER	    pprt;
   PPRINTERLIST pprtlist = hprtlist;
   PPRINTER	    pprtNext;


   if (pprtlist == NULL)
	   return FALSE;

   pprt = pprtlist->pprtHead;		// Start with first printer
   while (pprt != NULL)
      {		// Loop through list
   	pprtNext = pprt->pprtNext;	// Next printer
	   DestroyPrinter(pprt);		// Destroy this one
   	pprt = pprtNext;		// Advance
      }
   return TRUE;
   }



/***	PrintQueryNextPrinter - Get a printer from a list of printers
 *
 *	Use PrintQueryPrinterInfo to get information on a printer.
 *
 *	Entry
 *	    hprtlist - printer list
 *	    hprt     - starting printer
 *			  NULL => return first printer in list
 *			 !NULL => return printer after hprt
 *
 *	Exit-Success
 *	    Returns HPRT
 *
 *	Exit-Failure
 *	    Returns NULL
 */
HPRINTER PrintQueryNextPrinter(HPRINTERLIST hprtlist,HPRINTER hprt)
   {
   PPRINTERLIST    pprtlist = hprtlist;
   PPRINTER	    pprt = hprt;


   if (pprt == NULL)			// Get first printer
	   return pprtlist->pprtHead;
   else
      {				// Get next printer
   	return pprt->pprtNext;
      }
   }


/***	PrintQueryDefaultPrinter - Return default printer
 *
 *	Entry
 *	    hprtlist - printer list
 *
 *	Exit
 *	    Returns HPRINTER of default printer.
 */
HPRINTER PrintQueryDefaultPrinter(HPRINTERLIST hprtlist)
   {
   PPRINTERLIST pprtlist = hprtlist;

   return pprtlist->pprtDefault;
   }




/***	PrintQueryPrinterInfo - Get info on a printer
 *
 *	Entry
 *	    hprt  - printer handle
 *	    index - index of printer info
 *
 *	Exit-Success
 *	    Returns requested information
 *		PQPI_NAME	  - PSZ  - Nice name
 *		PQPI_LOG_ADDRESS  - PSZ  - Logical address
 *		PQPI_DRIVER	  - PSZ  - Driver name
 *		PQPI_MODEL	  - PSZ  - Model name
 *		PQPI_TYPE	  - ULONG  - OD_QUEUED or OD_DIRECT
 *		PQPI_PRINTER	  - PSZ  - Printer name
 *		PQPI_DESCRIPTION  - PSZ  - Description
 *
 *	Exit-Failure
 *	    Returns -1L
 */
ULONG PrintQueryPrinterInfo(HPRINTER hprt,USHORT index)
   {
   PPRINTER	pprt = hprt;

   switch (index)
      {
   	case PQPI_NAME:
	      return (ULONG)(VOID FAR *)pprt->pszName;

   	case PQPI_LOG_ADDRESS:
	      return (ULONG)(VOID FAR *)pprt->pszLogAddr;

   	case PQPI_DRIVER:
	      GetDriverModel(pprt);	// Make sure we get correct data
	      return (ULONG)(VOID FAR *)pprt->pszDriver;

   	case PQPI_MODEL:
	      GetDriverModel(pprt);	// Make sure we get correct data
	      return (ULONG)(VOID FAR *)pprt->pszModel;

    	case PQPI_TYPE:
	      return pprt->lType;

    	case PQPI_PRINTER:
	      return (ULONG)(VOID FAR *)pprt->pszPrinter;

   	case PQPI_DESCRIPTION:
	      GetDescription(pprt);	// Get appropriate description
	      return (ULONG)(VOID FAR *)pprt->pszDescription;

   	default:
	      return -1L;
      }

   }/* End of function */




/***	PrintOpenDC - Open DC on specified printer
 *
 *	Entry
 *	    hab 	- Anchor block
 *	    hprt	- Printer handle
 *	    pszDataType - "PM_Q_STD" or "PM_Q_RAW"
 *
 *	Exit-Success
 *	    Returns hdc
 *
 *	Exit-Failure
 *	    Returns NULL
 */
HDC PrintOpenDC(HAB hab,HPRINTER hprt,PSZ pszDataType)
   {
   DEVOPENSTRUC    dop;
   HDC 	    hdc;
   PPRINTER	    pprt = hprt;

   GetDriverData(pprt);		// Make sure we have driver data

   dop.pszLogAddress = pprt->pszLogAddr; // Logical address
   dop.pszDriverName = pprt->pszDriver; // Driver name (PSCRIPT)
   dop.pdriv	      = (VOID *)pprt->pbDriverData; // Driver data
   dop.pszDataType   = pszDataType;	// PM_Q_STD or PM_Q_RAW

 //dop.pszComment    = "stock chart";	// Comment for OD_Q
 //dop.pszQueueProcName   = NULL;	// queue processor; NULL => use default
 //dop.pszQueueProcParams = NULL;	// parms for queue processor
 //dop.pszSpoolerParams   = NULL;	// spooler parms (use NULL!)
 //dop.pszNetworkParams   = NULL;	// network parms (use NULL!)

   hdc = DevOpenDC(
                  hab,		         	// anchor block
                  pprt->lType,	   	// DC type: OD_DIRECT or OD_QUEUED
                  "*",		         	// device info(?) "*"
                  4L, 		         	// count of info in DEVOPENSTRUC
                  (PDEVOPENDATA)&dop, 	// DEVOPENSTRUC
                  NULL);      			// Compatible DC (use NULL?)
      
   return hdc;
   }


/***	PrintQueryJobProperties - Get printer job properties
 *
 *	Entry
 *	    hprt    - printer handle
 *	    pcbData - pointer to size of pb buffer in bytes
 *			Pass 0 to determine required buffer size.
 *	    pbData  - pointer to buffer to receive job properties
 *
 *	Exit-Success
 *	    Returns TRUE, job properties retrieved
 *		pb filled in with job property data
 *		pcb filled in with size of pb data
 *
 *	Exit-Failure
 *	    Returns FALSE, pb too small for job properties
 *		pcb filled in with required size
 *
 *	Note
 *	    (1) It is valid for a printer to require no job properties.
 *		In this case, *pcb == 0.
 */
BOOL PrintQueryJobProperties(HPRINTER hprt,USHORT *pcbData,BYTE *pbData)
   {
   USHORT	cb;			// Total buffer size
   USHORT	cbData; 		// Size of driver data
   USHORT	cbDriver;		// Size of driver name (counting NUL)
   USHORT	cbModel;		// Size of model name (counting NUL)
   USHORT	cbName; 		// Size of printer name (counting NUL)
   BYTE *	pb;
   PJOBPROP	pjp;
   PPRINTER	pprt = hprt;

   GetDriverData(pprt);		// Make sure we have driver data

   cbName   = strlen(pprt->pszName)+1; // Count NUL
   cbDriver = strlen(pprt->pszDriver)+1; // Count NUL
   cbModel  = strlen(pprt->pszModel)+1; // Count NUL
   cbData   = pprt->cbDriverData;	// Driver data length

   // Check size of return buffer

   cb = sizeof(JOBPROP)+cbName+cbDriver+cbModel+cbData;
   if (*pcbData < cb)
      {              		// Check buffer size
   	*pcbData = cb;			//  Too small, return required size
	   return FALSE;			// Indicate failure
      }

   // Fill in return buffer

   pjp = (PJOBPROP)pbData;
   pjp->cb	  = cb;
   pjp->cbName   = cbName;
   pjp->cbDriver = cbDriver;
   pjp->cbModel  = cbModel;
   pjp->cbData   = cbData;

   pb = (BYTE *)pjp+sizeof(JOBPROP);	// Start of data space

   pb = memcpy(pb,pprt->pszName,cbName);
   pb += cbName;

   pb = memcpy(pb,pprt->pszDriver,cbDriver);
   pb += cbDriver;

   pb = memcpy(pb,pprt->pszModel,cbModel);
   pb += cbModel;

   memcpy(pb,pprt->pbDriverData,cbData);

   *pcbData = cb;			// Return actual size
   return TRUE;
   }


/***	PrintChangeJobProperties - Let user change printer job properties
 *
 *	Entry
 *	    hprt    - printer handle
 *
 *	Exit-Success
 *	    Returns TRUE, job properties changed
 *
 *	Exit-Failure
 *	    Returns FALSE, job properties not changed
 */
BOOL PrintChangeJobProperties(HPRINTER hprt)
   {
   HAB 	hab;
   BYTE *	pb;
   PPRINTER	pprt = hprt;
   PSZ 	pszName;
   LONG	rc;

   hab = pprt->pprtlist->hab;

   GetDriverData(pprt);		// Try to get driver data

   if (pprt->pbDriverData != NULL)
      {	// We do have it
      pb = pprt->pbDriverData;	//  Use our data as starting point
      pszName = NULL; 		// Do not use printer name
      }
   else
      {
      pb = NULL;			// We have no data
      pszName = pprt->pszName;	// Tell driver to get it from printer
      }

   /* -- This is the call that gets the device driver to envoke its
      -- control dialog box. */
   rc = DevPostDeviceModes(hab,			         // Anchor block
                           (VOID *)pb, 		   // Buffer for data
                           pprt->pszDriver,		// Driver name
                           pprt->pszModel,		// Device name
                           pszName,			      // Printer name
                           DPDM_POSTJOBPROP);	// Option
   
   return (rc == DEV_OK);
   }




/***	PrintResetJobProperties - Reset printer to default job properties
 *
 *	Entry
 *	    hprt - printer handle
 *
 *	Exit
 *	    Job properties set to default for printer
 */
VOID PrintResetJobProperties(HPRINTER hprt)
   {
   HAB 	hab;
   PPRINTER	pprt = hprt;

   hab = pprt->pprtlist->hab;

   // Free properties, if present

   if (pprt->pbDriverData != NULL)
      {
   	ItiMemFree(hheapGlobal, pprt->pbDriverData);
	   pprt->pbDriverData = NULL;
   	pprt->cbDriverData = 0;
      }

   DosBeep(1600, 10);

   // Get job properties
   GetDriverData(pprt);
   }



/***	PrintMatchPrinter - Select printer that matches job properties
 *
 *	Entry
 *	    hprtlist - printer list
 *	    cbData   - size of pb buffer in bytes
 *	    pbData   - buffer with job properties
 *
 *	Exit-Success
 *	    Returns valid HPRINTER that matches Job Properties
 *	    Supplied job properties are set for printer.
 *
 *	Exit-Failure
 *	    Returns NULL, no matching printer
 *		Caller should use default printer (PrintQueryDefaultPrinter)
 */
HPRINTER PrintMatchPrinter(HPRINTERLIST hprtlist,USHORT cbData,BYTE *pbData)
   {
   ULONG	       cb;
   HAB 	       hab;
   BYTE *	    pb; 		// Driver data
   PJOBPROP	    pjp;
   PPRINTER	    pprt;
   PPRINTER	    pprtMatch;
   PPRINTERLIST pprtlist = hprtlist;
   PSZ 	    pszDriver;		// Driver name
   PSZ 	    pszModel;		// Model name
   PSZ 	    pszName;		// Printer name


   hab = pprtlist->hab;

   // Buffer must be at least as large as header

   if (cbData < sizeof(JOBPROP))	// Too small
	   return NULL;      			// Fail

   // Check internal length

   pjp = (PJOBPROP)pbData;		// Point at header
   if (pjp->cb != cbData)		// Internal length does not match
   	return NULL;		   	//  Fail

   // Check internal counts

   cb = pjp->cbName+pjp->cbDriver+pjp->cbModel+pjp->cbData+sizeof(JOBPROP);
   if (cb > (ULONG)USHRT_MAX)		// Numbers too large
   	return NULL;
   if ((USHORT)cb != cbData)		// Lengths do not match
	   return NULL;

   // Set string/data pointers

   pszName = (BYTE *)pjp+sizeof(JOBPROP); // Name
   pszDriver = pszName + pjp->cbName;	   // Driver
   pszModel = pszDriver + pjp->cbDriver;  // Model
   pb = pszModel + pjp->cbModel;	         // Data

   // Try to find close match
   //
   // If we find a printer that matches Name, Driver, Model, and driver
   // data size, we have an exact match, and return that.
   //
   // If we find a printer that matches all but the Name, we return the
   // first such printer (after examining all printers to make sure there
   // is no exact match.
   //
   // Otherwise, we return NULL.

   pprtMatch = NULL;			// No match, yet
   for (pprt=pprtlist->pprtHead; pprt!=NULL; pprt=pprt->pprtNext)
      {
   	if ( (stricmp(pprt->pszDriver,pszDriver) == 0)
          &&(stricmp(pprt->pszModel,pszModel) == 0) )
         {
         // Match Driver.Model
         // Get driver data size
   	   cb = DevPostDeviceModes(hab,// Anchor block
                        	    NULL, // Buffer for data (NULL returns size)
                     		    pprt->pszDriver,	   // Driver name
                     		    pprt->pszModel,	   // Device name
                     		    NULL,	         	// Printer name
                     		    DPDM_QUERYJOBPROP);	// Option

         if (cb == pjp->cbData)
            {	// Match driver data size
      		if (stricmp(pprt->pszName,pszName) == 0)
               { // Exact match?
   	   	   pprtMatch = pprt;	// Exact match
	   	      break;		      // Exit loop
		         }
   		   else if (pprtMatch == NULL) // First close match
	   	    pprtMatch = pprt;	       // Remember close match
	         }
	      }
      }/* end of for loop */

   // Update job properties in printer

   if (pprtMatch != NULL)
      {		// Found a match
   	if (pprt->pbDriverData == NULL)                // No buffer, yet
	      pprt->pbDriverData = ItiMemAlloc(hheapGlobal, pjp->cbData, MEM_ZERO_INIT);

   	if (pprt->pbDriverData != NULL)
         { // Have buffer
	      memcpy(pprt->pbDriverData,pb,pjp->cbData); // Save job properties
	      pprt->cbDriverData = pjp->cbData;          // Save JP size
	      }
      }

   return pprtMatch;			// Return best match
   }



/***	SplitPrinter - Split printer if it contains multiple queue names
 *
 *	Entry
 *	    pprtlist - printer list
 *	    pprt     - printer to be split
 *
 *	Exit
 *	    Returns next PPRINTER
 */
PPRINTER SplitPrinter(PPRINTERLIST pprtlist,PPRINTER pprtOld)
   {
   PPRINTER	pprt;
   PPRINTER	pprtNext;
   PPRINTER	pprtPrev;
   PSZ 	psz;
   PSZ 	psz1;
   USHORT	cprtAdded=0;

   if (pprtOld->lType != OD_QUEUED)	// If not a queue
	   return pprtOld->pprtNext;	//  Next printer

   psz = strpbrk(pprtOld->pszLogAddr,","); // Find queue name separator
   if (psz == NULL)			// If no separator
	   return pprtOld->pprtNext;	//  Next printer

   // Split up the printer

   strcpy(achPath,pprtOld->pszLogAddr); // Get working copy of queue name
   psz = achPath;
   while (psz != NULL)
      {
   	psz1 = strpbrk(psz,",");// Find separator
	   if (psz1 != NULL)
         {		// Found one
	      *psz1 = '\0';		   // Terminate this queue name

   	   psz1++;			         // Advance to next queue name
      	}

   	pprt = ClonePrinter(pprtOld,psz); // Clone with different pszLogAddr
   	if (pprt == NULL)		// Clone failed
	       psz = NULL; 		// Exit loop
   	else
         {				// Clone successful
	      AddPrinter(pprtlist,pprt);	// Add new printer
   	   cprtAdded++;		// Printer (may have been) added
	      psz = psz1; 		// Advance to next queue (if any)
	      // Note: AddPrinter may not have actually added the printer,
   	   //	     because it the split printer may have been a duplicate.
	      //	     That is okay, though, because we use cprtAdded only
	      //	     to know if we can delete pprtOld.
      	}
      }

   // Delete old printer, if we were able to split it
   //
   // If we were not able to clone the printer at all, then we leave
   // it in the list, since the queue list was truncated to the first
   // queue name!
   //
   // The normal case, however, is that we did split it successfully,
   // so we want to delete the old printer.
   //
   // NOTE: If we were guaranteed that one of the AddPrinter calls actually
   //	     added a printer, we would *know* that pprtNext would never be
   //	     NULL.  However, it is possible that a split could result in
   //	     printers that are *all* already in the list.  Besides, the
   //	     code is more robust this way.

   pprtNext = pprtOld->pprtNext;	// Next printer
   if (cprtAdded != 0)
      {		// Have to delete old printer
   	pprtPrev = pprtOld->pprtPrev;	// Previous printer

	   if (pprtNext != NULL)		// There is a following printer
	      pprtNext->pprtPrev = pprtPrev; // Set back link

   	if (pprtPrev != NULL)		// There is a preceding printer
	      pprtPrev->pprtNext = pprtNext; // Set forward link

   	DestroyPrinter(pprtOld);	// No need for this anymore
      }

   return pprtNext;			// Next printer
   } /* End of function SplitPrinter */





/***	QueryDefaultPrinterQueue - Get the default printer and queue
 *
 *	Entry
 *	    ppszQueue	- pointer to receive pointer to queue name
 *	    ppszPrinter - pointer to receive pointer to printer name
 *
 *	Exit
 *	    One or both of ppszQueue/ppszPrinter may be set to NULL!
 *	    Caller must call MemFree on ppszQueue/Printer!
 *
 *	Uses
 *	    achPath
 */
VOID QueryDefaultPrinterQueue(PSZ *ppszQueue, PSZ *ppszPrinter)
   {
   ULONG   cb;

   // Get default printer and queue

   cb = PrfQueryProfileString(
	   HINI_USER,			// OS2.INI
	   "PM_SPOOLER",		// Application
	   "PRINTER",			// Variable
	   "", 		      	// Default value
	   achPath,			   // Buffer to receive value
	   (ULONG)sizeof(achPath));	// Buffer size

   if (cb == 0)			// Couldn't get value
	   *ppszPrinter = NULL;
   else
      {
   	TrimTrailingSemicolon(achPath); // Get rid of trailing ";"
	   *ppszPrinter = ItiStrDup(hheapGlobal, achPath);
      }

   cb = PrfQueryProfileString(
	   HINI_USER,			         // OS2.INI
	   "PM_SPOOLER",		         // Application
	   "QUEUE",			            // Variable
	   "", 			               // Default value
	   achPath,			            // Buffer to receive value
	   (ULONG)sizeof(achPath));	// Buffer size

   if (cb == 0)
	   *ppszQueue = NULL;  // Couldn't get value
   else
      {
   	TrimTrailingSemicolon(achPath); // Get rid of trailing ";"
	   *ppszQueue = ItiStrDup(hheapGlobal, achPath);
      }

   }/* End of function QueryDefaultPrinterQueue */




/***	PrintCreatePrinterList - Get list of printers
 *
 *	Use PrintQueryNextPrinter to enumerate printers.
 *	Use PrintQueryPrinterInfo to get information on a printer.
 *
 *	Entry
 *	    hab - anchor block handle
 *
 *	Exit-Success
 *	    Returns HPRINTERLIST
 *
 *	Exit-Failure
 *	    Returns NULL
 */
HPRINTERLIST PrintCreatePrinterList(HAB hab)
   {
   static PPRINTER pprt;
   PSEARCHP	    psrchp=NULL;
   PPRINTER	    pprtQueue=NULL;
   PPRINTER	    pprtPrinter=NULL;
   PPRINTERLIST    pprtlist;
   static PSZ    pszPrinter;
   static PSZ    pszQueue;

   // Build initial printer list

   psrchp = PrinterBeginSearch(&pprt); // Get first printer
   if (psrchp == NULL) 		// No printers installed
	   return NULL;			//  Fail

   pprtlist = ItiMemAlloc(hheapGlobal,sizeof(PRINTERLIST), MEM_ZERO_INIT);
   if (pprtlist == NULL)
      {		// No memory
   	PrinterEndSearch(psrchp);	// Done searching
	   return NULL;	
      }
   pprtlist->pprtHead = NULL;
   pprtlist->pprtTail = NULL;

    // Add all printers

   while (pprt != NULL)
      {		// Get all printers
   	AddPrinter(pprtlist,pprt);	// Add to list
	   pprt = PrinterNextSearch(psrchp); // Get another printer
      }
   PrinterEndSearch(psrchp);		// Done searching

   // Now we have a list of printers that may have multiple queues
   // associated with them, and may have multiple drivers.
   //
   // Here, we split entries to get one per queue
   // It may still be the case that the driver.model are incorrect
   // for the Queue.  However, we do a "lazy evaluation" of this,
   // using the bMultipleDrivers flag.  See PrintQueryPrinterInfo
   // and GetDriverModel for more details.

   // Split any multi-queue entries

   for (pprt=pprtlist->pprtHead; pprt!=NULL; )
     {
     pprt = SplitPrinter(pprtlist,pprt);	 // Split, if necessary
     }

   // Find default printer

   QueryDefaultPrinterQueue(&pszQueue,&pszPrinter);
   pprt = MatchPrinter(pprtlist,pszQueue,pszPrinter);
   if (pprt == NULL)	      		// No default printer
      pprt = pprtlist->pprtHead;	//  Choose first printer

   pprtlist->pprtDefault = pprt;	// Remember default printer

   if (pszQueue != NULL)	   	// Free default printer/queue names
      ItiMemFree(hheapGlobal, pszQueue);

   if (pszPrinter != NULL)
      ItiMemFree(hheapGlobal, pszPrinter);

   pprtlist->hab = hab;		// Remember hab
   return pprtlist;			// Return list to caller
   }



/***	HPRTFromListBox - Get selected printer
 *
 *	Entry
 *	    hwnd - hwnd of Printer Setup dialog
 *
 *	Exit
 *	    hprt of selected printer
 */
HPRINTER HPRTFromListBox(HWND hwnd)
   {
   HWND    hwndLB;
   USHORT  i;

   WinSetPointer(HWND_DESKTOP,hptrWait);

   // Get selection
   hwndLB = WinWindowFromID(hwnd, IDL_PRINTER);
   i = SHORT1FROMMR(WinSendDlgItemMsg(hwnd,IDL_PRINTER,LM_QUERYSELECTION,0L,0L));

   WinSetPointer(HWND_DESKTOP,hptrArrow);

   return (HPRINTER) (ULONG)WinSendMsg(hwndLB,LM_QUERYITEMHANDLE,MPFROMSHORT(i),0L);
   }








MRESULT EXPORT PrinterSetupDlgProc (HWND   hwnd,
                                    USHORT uMsg,
                                    MPARAM mp1,
                                    MPARAM mp2)
   {
   HPRINTER	   hprt;
   HWND	      hwndLB;
   PSZ         pszName;
   PSZ         pszNameDefault;
   
   switch (uMsg)
      {
      case WM_INITDLG:
         {
         USHORT   i;

			bNewJobProp = FALSE;
			hwndLB = WinWindowFromID(hwnd, IDL_PRINTER);
			hprt = 0;			// Enumerate full list of printers
			while ((hprt=PrintQueryNextPrinter(glb.hprtlist,hprt)) != NULL)
            {
			   pszName = (PSZ )PrintQueryPrinterInfo(hprt,PQPI_NAME);
			   // Insert item and store handle
			   i = SHORT1FROMMR(WinSendMsg(hwndLB, LM_INSERTITEM,
            				   MPFROMSHORT(LIT_SORTASCENDING),MPFROMP(pszName)));
            if ((i != LIT_MEMERROR) && (i != LIT_ERROR))
               {
   			   WinSendMsg(hwndLB,LM_SETITEMHANDLE, MPFROMSHORT(i),MPFROMP(hprt));
	   		   if (hprt == glb.hprt)        // This is the default printer
		   		   pszNameDefault = pszName; // Save name to set selection
               }
            else
               { // There was an error of somekind. 
               }

		   	}

			// Find default printer and set selection

			i = SHORT1FROMMR(WinSendMsg(hwndLB,LM_SEARCHSTRING,
					 MPFROM2SHORT(0,LIT_FIRST),MPFROMP(pszNameDefault)));
			WinSendMsg(hwndLB,LM_SELECTITEM,MPFROMSHORT(i),MPFROMSHORT(TRUE));
			break;
         }

      case WM_CLOSE:
         WinDismissDlg (hwnd, 0xffff);
         return 0;

      case WM_COMMAND:
	   	switch (SHORT1FROMMP(mp1))
            {
				case DID_OK:
				    hprt = HPRTFromListBox(hwnd); // Get selection
				    if (hprt != glb.hprt)
                  {  /* Printer destination changed */
   					glb.hprt = hprt;	   // Remember new printer
	   				bNewJobProp = TRUE;	// Force new job properties
                  }
				    glb.bNewJobProp = bNewJobProp; // User made change
				    WinDismissDlg(hwnd,TRUE);
				    return FALSE;

				case DID_PRN_SET:
				    hprt = HPRTFromListBox(hwnd);       // Get selection
				    if (PrintChangeJobProperties(hprt)) // Properties changed
				    bNewJobProp = TRUE;                 // Remember JP changed
				    return FALSE;

				case DID_PRN_RESET:
				    hprt = HPRTFromListBox(hwnd);  // Get selection
				    PrintResetJobProperties(hprt); // Reset job properties
				    bNewJobProp = TRUE;            // Remember JP changed
				    return FALSE;

				case DID_CANCEL:
				    WinDismissDlg(hwnd,FALSE);
				    return FALSE;

            case DID_HELP:
               return (0L);
				}

      } /* end switch (uMsg) */

   return WinDefDlgProc(hwnd,uMsg,mp1,mp2);

   }/* End of function PrinterSetupDlgProc */






USHORT EXPORT ItiMenuDoPrinterSetup (HWND hwndList)
   {
   USHORT uRet = 0;                                       //csp Sep 16, 1994

   glb.hab = pglobals->habMainThread;
   glb.hwndClient = pglobals->hwndAppClient;
   glb.hwndFrame = pglobals->hwndAppFrame;

   hptrWait   = WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,FALSE);
   hptrArrow = WinQuerySysPointer(HWND_DESKTOP,SPTR_ARROW,FALSE);


   /* -- Build printer list. */
   glb.hprtlist = PrintCreatePrinterList(pglobals->habMainThread);
   if (glb.hprtlist)                                     //csp Sep 16, 1994
      {
      glb.hprt = PrintQueryDefaultPrinter(glb.hprtlist); // Get default

      uRet = WinDlgBox (pglobals->hwndDesktop, pglobals->hwndAppClient,
                        PrinterSetupDlgProc, hmodMe, IDD_PRINTER_SETUP, hwndList);

      /* -- Destroy printer list. */
      if (glb.hprtlist != NULL)
         PrintDestroyPrinterList(glb.hprtlist);
      }
   else
      WinMessageBox (pglobals->habMainThread,         
                     pglobals->hwndAppClient,
                     "No Printers Installed!",
                     pglobals->pszAppName, 0,
                     MB_MOVEABLE | MB_OK | MB_ICONHAND);

   return uRet;
   }


