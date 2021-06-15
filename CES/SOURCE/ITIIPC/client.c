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


#define INCL_DOSSEMAPHORES
#define INCL_DOSERRORS
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>


#define MAX_TRIES          10
#define PROCESS_SEM_NAME   "\\sem\\iti\\dsshell\\ipc\\process"

int main (int argc, char *argv[])
   {
   HSEM     hsemStartProcess;
   USHORT   usError;
   USHORT   usCount;

   usError = 0;
   usCount = 0;

   do
      {
      usError = ItiSemCreateMutex (PROCESS_SEM_NAME,
                                   CSEM_PUBLIC,
                                   &hsemStartProcess);

      if (usError == ERROR_ALREADY_EXISTS)
         {
         /* the named pipe already exists */
         usError = ItiSemOpenMutex (PROCESS_SEM_NAME, &hsemStartProcess);
         printf ("Opened semaphore\n");
         }

      usCount++;
      } while (usError == ERROR_SEM_NOT_FOUND && usCount < MAX_TRIES);
   
   if (usError != 0)
      {
      /* oh deah. an error. */
      if (usCount >= MAX_TRIES)
         {
         /* The near-to-impossible has happened.  The universe is 
            comming to an end. */

         printf ("Error: The universe is coming to an end!\nHave a nice day ");
         printf ("\n\n\n\n(in reality, a near-to-impossible set of events has occured."
                 "\nIf the universe had really come to an end, you would have been"
                 "\nnotified of impending doom and destruction.)");
         }
      else
         printf ("Error creating/opening pipe: DOS Error %u (0x%x)\n", usError, usError);

      DosExit (EXIT_PROCESS, 0xffff);
      }

   /* we have a handle to a semaphore. */
   usError = ItiSemRequestMutex (hsemStartProcess, 0);
   
   if (usError != 0)
      {
      printf ("Error setting mutex: %u (%x)\n", usError, usError);
      } 

   printf ("Hit return to terminate\n");
   return getchar ();
   }

