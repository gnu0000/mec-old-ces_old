
#define INCL_DOSFILEMGR
#define INCL_VIO
#define INCL_KBD
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <share.h>
#include <conio.h>


#define VERSION     "6"
#define LINE_SIZE    10240

static void Usage (void)
   {  /* not used */
   printf ("\n Usage: UNECSV < infile.ext  > outfile.out \n");
   printf ("\n Where the infile.ext name is different from the outfile.out name. \n");
   }



int _cdecl main (int argc, char *argv [])
   {
   static CHAR  szStar[10] = "-\\|/+   ";
   CHAR     szMark[3] = "";
   INT   iLn = 0;
   USHORT  uMark = 0;
   char szRead [LINE_SIZE] = "";
   USHORT usEnd,  us, i, usCnt;
   BOOL bFirstLine = TRUE;
   PSZ  psz;

   usCnt = 0; 

   fprintf (stderr, "\nUnEcsv filter, version %s\n", VERSION);
   fprintf (stderr,
            "Copyright (c) 1997 by Info Tech, Inc.  All Rights Reserved.\n\n");


if ( (argc > 1) || ((argv[1][0] == '/') || (argv[1][0] == '-')) && (argv[1][1] == '?'))
   {
   //Usage ();
   printf ("\n Usage: UNECSV < infile.ext  > outfile.out \n");
   printf ("\n Where the infile.ext name is different from the outfile.out name. \n");
   return 0;
   }
else
   {
   while (fgets (szRead, sizeof szRead, stdin))
      {
      usEnd = strlen (szRead) - 2;
      usCnt++;  


      if (iLn % 50)
         {
         /* -- Show the user that something is happening. */
         szMark[0] = szStar[ uMark ];
         szMark[1] = '\0';
         fprintf(stderr, " %s \r", szMark);
         uMark++;
         if (uMark>6)
            uMark = 0;
         }
      iLn++;

      /* -- Strip trailing blanks --------------- */
      us = usEnd;
      while ((us > 0) && (szRead[us] == ' '))
         {
         us--;
         }
      if ( /* (szRead[us] == '\\') && */
           (szRead[us+1] == ' ') && (szRead[us+2] == ' '))
         {
         szRead[us+1] = szRead [usEnd+1];
         szRead[us+2] = szRead [usEnd+2];
         usEnd = us;
         }
      /* --  ------------------------------------ */


      if (szRead [usEnd] == '\\')
         {
         szRead [usEnd] = '\0';
         }
      else
         { ; }

   
      // if (usCnt == 3)
      if ((usCnt == 2) || (usCnt == 3))
         {
         if (szRead [usEnd] != '\\')
            szRead [usEnd+1] = '\0';

         if (  (szRead [usEnd-3] == 'T')
             &&(szRead [usEnd-2] == 'y')
             &&(szRead [usEnd-1] == 'p')
             &&(szRead [usEnd]   == 'e'))
            {
            if (  ( (szRead [usEnd-8] == 'U')
                  &&(szRead [usEnd-7] == 'n')
                  &&(szRead [usEnd-6] == 'i')
                  &&(szRead [usEnd-5] == 't')
                  &&(szRead [usEnd-4] == '-'))
               || ( (szRead [usEnd-7] == 'U')
                  &&(szRead [usEnd-6] == 'n')
                  &&(szRead [usEnd-5] == 'i')
                  &&(szRead [usEnd-4] == 't'))
               )
                  {
                  szRead [usEnd+1] = '\n';
                  szRead [usEnd+2] = '\0';
                  }

            }
         else
            {
            if (  (szRead [usEnd-3] == '-')
                 &&(szRead [usEnd-2] == 'S')
                 &&(szRead [usEnd-1] == 'u')
                 &&(szRead [usEnd]   == 'm'))
               {
               szRead [usEnd+1] = '\n';
               szRead [usEnd+2] = '\0';
               }

            }/* end else */


         /* -- Now change "System" to "Type" on line 3. */
         if (  (szRead [usEnd-5] == 'S')
             &&(szRead [usEnd-4] == 'y')
             &&(szRead [usEnd-3] == 's')
             &&(szRead [usEnd-2] == 't')
             &&(szRead [usEnd-1] == 'e')
             &&(szRead [usEnd]   == 'm'))
            {
            if (  ( (szRead [usEnd-10] == 'U')
                  &&(szRead [usEnd-9] == 'n')
                  &&(szRead [usEnd-8] == 'i')
                  &&(szRead [usEnd-7] == 't')
                  &&(szRead [usEnd-6] == '-'))
               || ( (szRead [usEnd-9] == 'U')
                  &&(szRead [usEnd-8] == 'n')
                  &&(szRead [usEnd-7] == 'i')
                  &&(szRead [usEnd-6] == 't'))
               )
                  {
                  szRead [usEnd-5] = 'T';
                  szRead [usEnd-4] = 'y';
                  szRead [usEnd-3] = 'p';
                  szRead [usEnd-2] = 'e';
                  szRead [usEnd-1] = '\n';
                  szRead [usEnd]   = '\0';

                  //szRead [usEnd+1] = '\n';
                  //szRead [usEnd+2] = '\0';
                  }
            }



         } /* end if (usCnt == 3 ... */



/* -- Temporary, change the unit-type values. ================== */
     // Check for the extra space at the EOL of the cesprop file.
      if (szRead[usEnd] == ' ')
         {
         szRead[usEnd]   = szRead[usEnd+1] ;
         szRead[usEnd+1] = '\0';
         // szRead[usEnd+1] = '\n'; // should this be used instead?    
         usEnd--;
         }

     // This ASSUMES the csv field is like: "E"\n
      if ( (szRead[usEnd] == '\"')
           && ((szRead[usEnd-2] == '\"') || (szRead[usEnd-3] == '\"')) )
                 /* case of  "E"\n  */       /* case of  "EC"\n  */
         {
         if ((szRead[usEnd-1] == 'E')
             || ((szRead[usEnd-2] == 'E')&&(szRead[usEnd-1] == 'C')) )
            {
            if (szRead[usEnd-3] == '\"')
               szRead[usEnd-3] = ' ';

            szRead[usEnd-2] = ' ';
            szRead[usEnd-1] = '0';
            szRead[usEnd] = '\n';  //1996
            szRead[usEnd+1] = '\0'; //1996
            }
         if ((szRead[usEnd-1] == 'M')
             || ((szRead[usEnd-2] == 'M')&&(szRead[usEnd-1] == 'C')) )
            {
            if (szRead[usEnd-3] == '\"')
               szRead[usEnd-3] = ' ';

            szRead[usEnd-2] = ' ';
            szRead[usEnd-1] = '1';
            szRead[usEnd] = '\n';  //1996
            szRead[usEnd+1] = '\0'; //1996
            }
         if (szRead[usEnd-1] == 'N')
            {
            szRead[usEnd-1] = '\"';
            szRead[usEnd] = '\n';   //1996
            szRead[usEnd+1] = '\0'; //1996
            }
         }



     // This ASSUMES the csv field is like: E\n  
      if ( (szRead[usEnd] == 'E') && (szRead[usEnd-1] == ',')
          || (szRead[usEnd] == 'M') && (szRead[usEnd-1] == ',')
          || (szRead[usEnd] == 'N') && (szRead[usEnd-1] == ',')
         )
         {
         if (szRead[usEnd] == 'E')
            {
            szRead[usEnd] = '0';
            szRead[usEnd+1] = '\n';  //1996
            szRead[usEnd+2] = '\0'; //1996
            }
         if (szRead[usEnd] == 'M')
            {
            szRead[usEnd] = '1';
            szRead[usEnd+1] = '\n';  //1996
            szRead[usEnd+2] = '\0'; //1996
            }
         if (szRead[usEnd] == 'N')
            {
            szRead[usEnd] = ' ';
            szRead[usEnd+1] = '\n';  //1996
            szRead[usEnd+2] = '\0'; //1996
            }
         }
/* -- End of temporary change to the unit-type values. ========= */



      if (bFirstLine)
         {
         bFirstLine = FALSE;
         i = 0;
         while (i != usEnd)
            {
            psz = &szRead[i];
            if (psz!=NULL && *psz=='E' && *(psz+1)=='C' && *(psz+2)=='S' && *(psz+3)=='V')
               {
               szRead[i] = 'C';
               szRead[i+1] = 'S';
               szRead[i+2] = 'V';
               szRead[i+3] = ' ';
               break;
               }
            i++;
            }/* end while */

         }

      fputs (szRead, stdout);
      }/* end of while */

   }/* end of else */
   return 0;
   }
