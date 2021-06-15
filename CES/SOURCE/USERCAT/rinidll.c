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


#include "..\include\iti.h"       
#include "..\include\itimbase.h"  
#include "..\include\winid.h"      
#include "..\include\itirdefs.h"
#include "rUser.h"                   

#include "rinidll.h"



static BOOL bAlreadyRegistered = FALSE;

/* the variable below tells the linker not to link with the 
   C run time library.  This is only need for the following case: if
   you don't use the C run time library, and you have no static variables. */
/* int _acrtused = 0; */



/*
 * hmodModule is the handle to this DLL
 */

HMODULE hmodModule;



/*
 * DllInit is called by either ITIDLLI.OBJ or ITIDLLC.OBJ.  This functino
 * must return non-zero if the initialization succeded, or FALSE
 * if initialization failed.
 */



BOOL DLLINITPROC DllInit (HMODULE hmodDll)
   {
   hmodModule = hmodDll;
   return TRUE;
   }





BOOL EXPORT ItiDllInitDll (VOID)
   {
   if (bAlreadyRegistered)
      return TRUE;

   if (ItiMbRegisterDialogWnd (rUser_RptDlgBox,  rUser_RptDlgBox,  
                              ItiDllrUserRptDlgProc,           
                              hmodModule)) 
     return FALSE;

   bAlreadyRegistered = TRUE;
   return TRUE;                             
   }                                        
