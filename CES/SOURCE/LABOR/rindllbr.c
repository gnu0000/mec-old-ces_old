/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by AASHTO.  All Rights Reserved.                 |
|                                                                           |
|       This program module is part of BAMS/CES, a module of BAMS,          |
|       The American Association of State Highway and Transportation        |
|       Officials' (AASHTO) Information System for Managing Transportation  |
|       Programs, a proprietary product of AASHTO, no part of which may be  |
|       reproduced or transmitted in any form or by any means, electronic,  |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from AASHTO.                  |
|                                                                           |
+--------------------------------------------------------------------------*/


#include "..\include\iti.h"       
#include "..\include\itimbase.h"  
#include "..\include\itirdefs.h"  
#include "..\include\winid.h"      
#include "rLabor.h"                   

#include "rindllbr.h"


static BOOL bAlreadyRegistered = FALSE;


/* the variable below tells the linker not to link with the 
   C run time library.  This is only need for the following case: if
   you don't use the C run time library, and you have no static variables. */
/* int _acrtused = 0; */



/*
 * hmodrLabor is the handle to this DLL
 */

HMODULE hmodrLabor;



/*
 * DllInit is called by either ITIDLLI.OBJ or ITIDLLC.OBJ.  This functino
 * must return non-zero if the initialization succeded, or FALSE
 * if initialization failed.
 */



BOOL DLLINITPROC DllInit (HMODULE hmodDll)
   {
   hmodrLabor = hmodDll;
   return TRUE;
   }





BOOL EXPORT ItiDllInitDll (VOID)
   {
   if (bAlreadyRegistered)
      return TRUE;


  if (ItiMbRegisterDialogWnd (rLabor_RptDlgBox,  rLabor_RptDlgBox,  
                              ItiDllrLaborRptDlgProc,           
                              hmodrLabor)) 
     return FALSE;

  bAlreadyRegistered = TRUE; 
  return TRUE;
  }                                        
