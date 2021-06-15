//
//
// LASTVAR is not recursive! does it matter ?
//
//


/*
 * GnuMath.c
 *
 * Part of GnuMath.h
 * Part of the GnuLib library
 *
 * (C) 1993 Info Tech Inc.
 *
 * Craig Fitzgerald
 * 
 * This file provides the math functions for Math
 * 
 *  possible later enhancements:
 *    add function to add math functions
 *    add function to add math constants
 *    add function to add/get/set variables
 */

#define MEM_NULL
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "GnuMath.h"

#define MARK  0x12345678L

#define STARTFNS  15
#define STARTFNS2 0
#define STARTCON  2
#define STARTVAR  2

BIG _Eval (PSZ *ppszExp, USHORT uLevel);




/*
 * This structure is used to store
 * Function names and pointers for 
 * function calls in expressions
 */
typedef struct
   {
   PSZ     pszFn;
   PMTHFN  pfn;
   } FNDESCR;
typedef FNDESCR *PFNDESCR;

/*
 * This structure is used to store
 * Function names and expression strings for 
 * function calls in expressions
 */
typedef struct
   {
   PSZ  pszFn;
   PSZ  pszExpr;
   } FNDESCR2;
typedef FNDESCR2 *PFNDESCR2;



/*
 * This structure holds variable and constant names and values
 */
typedef struct
   {
   PSZ     psz;
   BIG     big;
   } MVAL;
typedef MVAL *PMVAL;

void BIGInitMath (void);


USHORT EVALERR     = 0;
PSZ    EVALERRSTR  = NULL;
USHORT EVALERRIDX  = 0;
PSZ    EVALERRPOS  = NULL;
PSZ    pszErrPtr   = NULL;

USHORT  LASTVAR = 0;

PFNDESCR  mathfn    = NULL;
PFNDESCR2 mathfn2   = NULL;
PMVAL     mathconst = NULL;
PMVAL     mathval   = NULL;

BOOL bPERCENT = FALSE;


char *MathErrors[] = {"No Error",                         // 0
                      "Closing Parenthesis expected ')'", // 1
                      "Opening Parenthesis expected '('", // 2
                      "Unrecognized character",           // 3
                      "Empty String",                     // 4
                      "Function Overflow",                // 5
                      "Function param out of range",      // 6
                      "Illegal function value",           // 7
                      "Function result out of range",     // 8
                      "Divide by zero error",             // 9
                      "",                                 // 10
                      NULL};                                  





/********************************************************************/
/*                                                                  */
/*  Error Routines                                                  */
/*                                                                  */
/********************************************************************/

BIG SetMathErr (USHORT uErr, PSZ pszErrPos)
   {
   BIG bg; 

   bg = 0.0;

   if (EVALERR)
      return bg;
   EVALERR    = uErr;
   EVALERRSTR = MathErrors [uErr];
   EVALERRPOS = pszErrPos;
   return bg;
   }





/*
 * Math NMI error function
 *
 *     return SetMathErr (2, *ppszExp);
 *
 */
int _cdecl _matherrl (struct _exceptionl *except)
   {
   switch (except->type)
      {
      case DOMAIN:    /*--- argument out of range        ---*/
         except->retval = 0;
         SetMathErr (6, pszErrPtr);
         break;

      case OVERFLOW:  /*--- result too large             ---*/
         except->retval = 0;
         SetMathErr (5, pszErrPtr);
         break;

      case SING:      /*--- illegal value ie log(0)      ---*/
         except->retval = 0;
         SetMathErr (7, pszErrPtr);
         break;

      case TLOSS:     /*--- total loss                   ---*/
         except->retval = 0;
         SetMathErr (8, pszErrPtr);
         break;

      case UNDERFLOW: /*--- result too small             ---*/
         except->retval = 0;
         SetMathErr (5, pszErrPtr);
         break;

      default:
         return 0;
      }
   return 1;
   }



/*
 *
 * After each call to AToBig, this fn stores the error state
 * This fn returns a non zero value in case of an error
 *
 * pszErrStr will contain a description of the error
 * *puErrIdx will contain the index of the offending char
 *    from the string passed to AToBIG
 *
 * ret   error string
 *------------------------------------------------
 * 0     No Error                         
 * 1     Closing Parenthesis expected ')' 
 * 2     Opening Parenthesis expected '(' 
 * 3     Unrecognized character           
 * 4     Empty String                     
 * 5     Function Overflow                
 * 6     Function param out of range      
 * 7     Illegal function value           
 * 8     Function result out of range     
 * 
 */
USHORT _cdecl MthIsError (PSZ *ppszErrStr, USHORT *puErrIdx)
   {
   if (ppszErrStr)
      *ppszErrStr = EVALERRSTR;
   if (puErrIdx)
      *puErrIdx = EVALERRIDX;
   return EVALERR;
   }




/********************************************************************/
/*                                                                  */
/*  BIG Math Routines                                               */
/*                                                                  */
/********************************************************************/


BOOL _cdecl MthValid (BIG bgNum)
   {
   PULONG  pulTmp = (PULONG)(PVOID)&(bgNum);

   return !(*pulTmp == MARK);
   }



void _cdecl MthInvalidate (PBIG pbgNum)
   {
   PULONG  pulTmp = (PULONG)(PVOID)pbgNum;

   *pulTmp = MARK;
   }


void _cdecl MthPercentIsMod (BOOL b)
   {
   bPERCENT = !b;
   }


/*
 * returns:
 *  0 - one or more invalid
 *  1 - both valid and matched to within .000005
 *  2 - both valid and matched to within .005
 *  3 - both valid but not matched
 *
 */
USHORT _cdecl MthMatch (PBIG pbg1, PBIG pbg2)
   {
   if (!MthValid (*pbg1)      || !MthValid (*pbg2))
      return 0;
   if (*pbg1 - *pbg2 > 0.005    || *pbg1 - *pbg2 < -0.005)
      return 3;
   if (*pbg1 - *pbg2 > 0.000005 || *pbg1 - *pbg2 < -0.000005)
      return 2;
   return 1;
   }



PSZ AddCommas (PSZ psz)
   {
   int iLen, i;

   iLen = strlen (psz) + 5;
   i = (strchr (psz, '.') ? strchr (psz, '.') - psz - 3 : strchr (psz, '\0') - psz - 3);
   for (; i > (*psz == '(' ? 2 : (*psz == '$' || *psz == '-' ? 1 : 0)); i -= 3)
      {
      memmove (psz + i + 1, psz +i, iLen);
      psz[i]=',';
      }
   return psz;
   }




/*
 *  always has commas
 *
 *  0 - 9.3
 *  1 - $8.5
 *  2 - $9.2
 *  3 - 8.5
 *  4 - 9.2
 *  5 - 12.5
 *
 *  6 - 9.3 no commas
 *  7 - 8.5 no commas
 *  8 - 9.2 no commas
 */
PSZ _cdecl MthFmat (PSZ pszNum, BIG bgNum, USHORT uFmt)
   {
   pszNum[0] = '\0';

   if (!MthValid (bgNum))
      return pszNum;

   switch (uFmt)
      {
      case 6:
      case 0:  /*---  0 -  9.3  ---*/
         if (bgNum >= 999999999.9995 || bgNum <= -999999999.9995)
            strcpy (pszNum, "*********.***");
         else
            sprintf (pszNum, "%5.3lf", (double)bgNum);
         break;
      case 1:  /*---  1 - $8.5  ---*/
         if (bgNum >= 99999999.999995 || bgNum <= -99999999.999995)
            strcpy (pszNum, "********.***** ");
         else if (bgNum >= 0)
            sprintf (pszNum, "$%7.5lf ", (double) bgNum);
         else
            sprintf (pszNum, "($%7.5lf)", (double) fabsl (bgNum));
         break;
      case 2:  /*---  2 - $9.2  ---*/
         if (bgNum >= 999999999.995 || bgNum <= -999999999.995)
            strcpy (pszNum, "*********.** ");
         else if (bgNum >= 0)
            sprintf (pszNum, "$%4.2lf ", (double)bgNum);
         else
            sprintf (pszNum, "($%4.2lf)", (double) fabsl (bgNum));
         break;
      case 7:
      case 3:  /*---  1 - 8.5  ---*/
         if (bgNum >= 99999999.999995 || bgNum <= -99999999.999995)
            strcpy (pszNum, "********.*****");
         else
            sprintf (pszNum, "%7.5lf", (double) bgNum);
         break;
      case 8:
      case 4:  /*---  2 - 9.2  ---*/
         if (bgNum >= 999999999.995 || bgNum <= -999999999.995)
            strcpy (pszNum, "*********.**");
         else
            sprintf (pszNum, "%4.2lf", (double)bgNum);
         break;
      case 9:
      case 5:  /*---  up to 17 chars ---*/
         if (bgNum > 9e12 || bgNum < -9e12)
            *pszNum = '\0';
         else
            sprintf (pszNum, "%7.5lf", (double) bgNum);
         pszNum[17]='\0';
         break;

      case 100:
      case 101:
      case 102:
      case 103:
      case 104:
      case 105:
      case 106:
      case 107:
      case 108:
      case 109:
         {
         char szTmp [20];

         if (bgNum >= 999999999.995 || bgNum <= -999999999.995)
            strcpy (pszNum, "*********");
         else
            {
            sprintf (szTmp, "%%1.%dlf", uFmt - 100);
            sprintf (pszNum, szTmp, (double)bgNum);
            }
         break;
         }


      case 10:  /*---  up to 40 chars ---*/
         if (bgNum > 9e12 || bgNum < -9e12)
            sprintf (pszNum, "%le", (double) bgNum);
         else
            sprintf (pszNum, "%7.5lf", (double) bgNum);
         pszNum[17]='\0';
         break;

      }
   /*--- Add commas to string ---*/
   if (uFmt < 6 || uFmt > 99)
      AddCommas (pszNum);
   return pszNum;
   }


/********************************************************************/
/*                                                                  */
/*                                                                  */
/*                                                                  */
/********************************************************************/


/*
 * returns TRUE if a fn is new and added
 * call with pfn = NULL to remove fn
 */
USHORT _cdecl MthAddFunction (PSZ pszName, PMTHFN pfn)
   {
   USHORT i;

   if (!mathfn)
      BIGInitMath ();

   for (i=0; mathfn[i].pszFn; i++)
      if (!stricmp (pszName, mathfn[i].pszFn))
         {
         if (pfn) /*--- change function pointer ---*/
            {
            mathfn[i].pfn = pfn;
            return i;
            }

         /*--- delete function ---*/
         if (i >= STARTFNS)
            free (mathfn[i].pszFn);
                      
         for (;mathfn[i].pszFn; i++)
            mathfn[i].pszFn = mathfn[i+1].pszFn,
            mathfn[i].pfn   = mathfn[i+1].pfn;

         mathfn = realloc (mathfn, sizeof (FNDESCR) * i);
         return i;
         }
   /*--- Add function ---*/
   mathfn = realloc (mathfn, sizeof (FNDESCR) * (i + 2));
   mathfn[i].pszFn  = strdup (pszName),   mathfn[i].pfn    = pfn;
   mathfn[i+1].pszFn= NULL,               mathfn[i+1].pfn  = NULL;
   return i;
   }


/*
 * returns TRUE if a fn is new and added
 * call with pfn = NULL to remove fn
 */
USHORT _cdecl MthAddFunction2 (PSZ pszName, PSZ pszExpr)
   {
   USHORT i;

   if (!mathfn)
      BIGInitMath ();

   for (i=0; mathfn2[i].pszFn; i++)
      if (!stricmp (pszName, mathfn2[i].pszFn))
         {
         if (pszExpr) /*--- change function pointer ---*/
            {
            free (mathfn2[i].pszExpr);
            mathfn2[i].pszExpr = strdup (pszExpr);
            return i;
            }

         /*--- delete function ---*/
         if (i >= STARTFNS)
            {
            free (mathfn2[i].pszFn);
            free (mathfn2[i].pszExpr);
            }
                      
         for (;mathfn2[i].pszFn; i++)
            mathfn2[i].pszFn   = mathfn2[i+1].pszFn,
            mathfn2[i].pszExpr = mathfn2[i+1].pszExpr;

         mathfn2 = realloc (mathfn2, sizeof (FNDESCR) * i);
         return i;
         }
   /*--- Add function ---*/
   mathfn2 = realloc (mathfn2, sizeof (FNDESCR) * (i + 2));
   mathfn2[i].pszFn  = strdup (pszName),   mathfn2[i].pszExpr    = strdup (pszExpr);
   mathfn2[i+1].pszFn= NULL,               mathfn2[i+1].pszExpr  = NULL;
   return i;
   }




USHORT _cdecl MthAddConstant (PSZ pszName, BIG big)
   {
   USHORT i;

   if (!mathfn)
      BIGInitMath ();

   for (i=0; mathconst[i].psz; i++)
      if (!stricmp (pszName, mathconst[i].psz))
         {
         /*--- change const ---*/
         mathconst[i].big = big;
         return i;
         }
   /*--- Add const ---*/
   mathconst = realloc (mathconst, sizeof (MVAL) * (i + 2));
   mathconst[i].psz   = strdup (pszName), mathconst[i].big   = big;
   mathconst[i+1].psz = NULL,             mathconst[i+1].big = 0;             
   return i;
   }


USHORT _cdecl MthAddVariable (PSZ pszName, BIG big)
   {
   USHORT i;

   if (!mathfn)
      BIGInitMath ();

   for (i=0; mathval[i].psz; i++)
      if (!stricmp (pszName, mathval[i].psz))
         {
         /*--- change val ---*/
         mathval[i].big = big;
         return i;
         }
   /*--- Add val ---*/
   mathval = realloc (mathval, sizeof (MVAL) * (i + 2));
   mathval[i].psz   = strdup (pszName), mathval[i].big   = big;
   mathval[i+1].psz = NULL,             mathval[i+1].big = 0;             
   return i;
   }




/*
 * 11/15/93 I need to do this because these functions are
 * floating around in a DLL
 */
BIG Mysinl  (BIG bg) {return sinl  (bg);}
BIG Mycosl  (BIG bg) {return cosl  (bg);}
BIG Mytanl  (BIG bg) {return tanl  (bg);}
BIG Myasinl (BIG bg) {return asinl (bg);}
BIG Myacosl (BIG bg) {return acosl (bg);}
BIG Myatanl (BIG bg) {return atanl (bg);}
BIG Myfabsl (BIG bg) {return fabsl (bg);}
BIG Myceill (BIG bg) {return ceill (bg);}
BIG Mycoshl (BIG bg) {return coshl (bg);}
BIG Mysinhl (BIG bg) {return sinhl (bg);}
BIG Myexpl  (BIG bg) {return expl  (bg);}
BIG Myfloorl(BIG bg) {return floorl(bg);}
BIG Mylog10l(BIG bg) {return log10l(bg);}
BIG Mylogl  (BIG bg) {return logl  (bg);}
BIG Mysqrtl (BIG bg) {return sqrtl (bg);}


void BIGInitMath (void)
   {
   mathfn  = malloc (sizeof (FNDESCR) * (STARTFNS+1));
   mathfn[ 0].pszFn = "sin";   mathfn[ 0].pfn = (PMTHFN) Mysinl;
   mathfn[ 1].pszFn = "cos";   mathfn[ 1].pfn = (PMTHFN) Mycosl;
   mathfn[ 2].pszFn = "tan";   mathfn[ 2].pfn = (PMTHFN) Mytanl;
   mathfn[ 3].pszFn = "asin";  mathfn[ 3].pfn = (PMTHFN) Myasinl;
   mathfn[ 4].pszFn = "acos";  mathfn[ 4].pfn = (PMTHFN) Myacosl;
   mathfn[ 5].pszFn = "atan";  mathfn[ 5].pfn = (PMTHFN) Myatanl;
   mathfn[ 6].pszFn = "abs";   mathfn[ 6].pfn = (PMTHFN) Myfabsl;
   mathfn[ 7].pszFn = "ceil";  mathfn[ 7].pfn = (PMTHFN) Myceill;
   mathfn[ 8].pszFn = "cosh";  mathfn[ 8].pfn = (PMTHFN) Mycoshl;
   mathfn[ 9].pszFn = "sinh";  mathfn[ 9].pfn = (PMTHFN) Mysinhl;
   mathfn[10].pszFn = "exp";   mathfn[10].pfn = (PMTHFN) Myexpl;
   mathfn[11].pszFn = "floor"; mathfn[11].pfn = (PMTHFN) Myfloorl;
   mathfn[12].pszFn = "log10"; mathfn[12].pfn = (PMTHFN) Mylog10l;
   mathfn[13].pszFn = "log";   mathfn[13].pfn = (PMTHFN) Mylogl;
   mathfn[14].pszFn = "sqrt";  mathfn[14].pfn = (PMTHFN) Mysqrtl;
   mathfn[15].pszFn = NULL;    mathfn[15].pfn = NULL;

   mathfn2 = malloc (sizeof (FNDESCR2) * (STARTFNS2+1));
   mathfn2[ 0].pszFn = NULL;   mathfn2[ 0].pszExpr = NULL;  

   mathconst = malloc (sizeof (MVAL) * (STARTCON+1));
   mathconst[0].psz = "pi";    mathconst[0].big = 3.1415926535;
   mathconst[1].psz = "e";     mathconst[1].big = 2.302585;
   mathconst[2].psz = NULL;    mathconst[2].big = 0.0;

   mathval = malloc (sizeof (MVAL) * (STARTVAR+1));
   mathval[0].psz = "@@";    mathval[0].big = 0.0; /*-- for expressions --*/
   mathval[1].psz = "@QUAN"; mathval[1].big = 0.0;
   mathval[2].psz = NULL;    mathval[2].big = 0.0;
   }


/********************************************************************/
/*                                                                  */
/*  Math Expression Routines                                        */
/*                                                                  */
/********************************************************************/

int SkipWhite (PSZ *ppsz)
   {
   while (**ppsz && strchr (" \t", **ppsz)) 
      (*ppsz)++;
   return **ppsz;
   }


int MthEat (PSZ *ppsz, PSZ pszList)
   {
   SkipWhite (ppsz);
   if (!**ppsz || !strchr (pszList, **ppsz))
      return 0;
   return *(*ppsz)++;
   }


BOOL IsNumber (PSZ psz)
   {
   SkipWhite (&psz);
   if (*psz && strchr ("-+", *psz))
      {
      psz++;
      SkipWhite (&psz);
      }
   return !!(psz && strchr ("0123456789.", *psz)) ;
   }


BIG  EatNumber (PSZ *ppszExp)
   {
   BIG  big;
   char szTmp[255];
   PSZ  p, p2;

   SkipWhite (ppszExp);
   
   /*--- _atold doesn't like numbers like .1 and - 2 ---*/
   p  = *ppszExp;
   p2 = szTmp;
   if (strchr ("+-", *p))
      *p2++ = *p++;
   SkipWhite (&p);
   if (*p == '.')
      *p2++ = '0';
   strncpy (p2, p, 254);
   p2[254] = '\0';
   big = _atold (szTmp);

   if (**ppszExp && strchr ("-+",               **ppszExp)) 
      (*ppszExp)++;
   SkipWhite (ppszExp);
   while (**ppszExp && strchr ("0123456789",    **ppszExp)) 
      (*ppszExp)++;
   if (**ppszExp && strchr (".",                **ppszExp)) 
      (*ppszExp)++;
   while (**ppszExp && strchr ("0123456789",    **ppszExp)) 
      (*ppszExp)++;
   if (**ppszExp && strchr ("dDeE",             **ppszExp))
      {
      (*ppszExp)++;
      if (**ppszExp && strchr ("-+",            **ppszExp)) 
         (*ppszExp)++;      
      while (**ppszExp && strchr ("0123456789", **ppszExp)) 
         (*ppszExp)++;
      }
   return big;
   }



/*********************************************************/
/*                 Variable Code                         */
/*********************************************************/



//USHORT MakeVar (PSZ *ppszVar)
//   {
//   char szTmp [40];
//   PSZ  pszTmp;
//   USHORT i;
//
//   for (i=1; i<MAXVARS && mathval[i].pszCon; i++)
//      ;
//   if (i == MAXVARS)  /*--- no more room ---*/
//      return 0;
//
//   pszTmp = szTmp;
//   *pszTmp++= *(*ppszVar)++;
//
//   while (isalnum (**ppszVar))
//      *pszTmp++= *(*ppszVar)++;
//   *pszTmp = '\0';
//   mathval[i].pszCon = strdup (szTmp);
//   mathval[i].big    = 0.0;
//   return i;
//   }
//
//
//USHORT FindVar (PSZ *ppszVar)
//   {
//   USHORT i, uLen;
//
//   for (i=1; i<MAXVARS; i++)
//      {
//      if (!mathval[i].pszCon)
//         continue;
//      uLen = strlen (mathval[i].pszCon);
//      if (!strnicmp (mathval[i].pszCon, *ppszVar, uLen) && !isalnum((*ppszVar)[uLen]))
//         {
//         *ppszVar += uLen;
//         return i;
//         }
//      }
//   return MakeVar (ppszVar);
//   }


/*
 * returns the value of a variable
 * returns 0 and *puIdx=0 if variable not found or created
 * if bMakeVar=TRUE and var not found, var is created (val=0)
 * if bIncStr=TRUE, var is eaten from head of *ppszVar
 * pbgVal is value of variable
 */
BIG GetVarValue (PSZ *ppszVar, BOOL bMakeVar, BOOL bIncStr, USHORT *puIdx)
   {
   char szTmp [40];
   PSZ  pszTmp, pszStr;
   USHORT i;

   /*--- get var of of string head ---*/
   pszTmp = szTmp;
   pszStr = *ppszVar;
   *pszTmp++= *pszStr++;
   if (*pszStr == '@')   /*--- special variable @@ for fn expr ---*/
      {
      *pszTmp++= *pszStr++;
      }
   else
      {
      while (isalnum (*pszStr))
         *pszTmp++= *pszStr++;
      }
   *pszTmp = '\0';

   if (bIncStr) *ppszVar = pszStr;

   for (i=0; mathval[i].psz; i++)
      {
      if (stricmp (szTmp, mathval[i].psz))
         continue;
      LASTVAR = *puIdx = i;
      return mathval[i].big;
      }
   if (bMakeVar)
      {
      LASTVAR = *puIdx = MthAddVariable (szTmp, 0.0);
      return 0.0;
      }
   *puIdx = 0;
   return 0.0;
   }


//BIG GetVar (PSZ *ppszVar)
//   {
//   USHORT i;
//
//   if (!(i = FindVar (ppszVar)))
//      return 0;
//   LASTVAR = i;
//   return mathval[i].big;
//   }


BOOL IsVar (PSZ *ppsz)
   {
   SkipWhite (ppsz);
   return (**ppsz == '@');
   }


BIG _cdecl MthSetVar (USHORT uIndex, BIG big)
   {
   if (!mathfn)
      BIGInitMath ();

   return mathval[uIndex].big = big;
   }



/*
 * eats a parenthesized expression
 * open parens already eaten
 *
 */
BIG ParenExp (PSZ *ppszExp)
   {
   BIG    big;

   big = _Eval (ppszExp, 0);
   if (!MthEat (ppszExp, ")"))
      return SetMathErr (1, *ppszExp);
   return big;
   }



BIG EvalAtom (PSZ *ppszExp)
   {
   USHORT i, l;
   BIG    bgTmp, bgTmp2;
   PSZ    p;

   i = l =0;
   /*--- Number ---*/
   if (IsNumber (*ppszExp))
      return EatNumber (ppszExp);

   /*--- Parenthesized Expression ---*/
   if (MthEat (ppszExp, "("))
      return ParenExp (ppszExp);

   /*--- Function ---*/
   /*
    * this convoluted way of doing things is necessary
    * because pszErrPtr must be set after the recursion
    * but before the math fn call
    */
   for (i=0; mathfn[i].pszFn; i++)
      {
      if (strnicmp (mathfn[i].pszFn, *ppszExp, l=strlen (mathfn[i].pszFn)))
         continue;

      p = *ppszExp;
      *ppszExp += l;
      if (!MthEat (ppszExp, "("))
         return SetMathErr (2, *ppszExp);

      bgTmp = ParenExp (ppszExp);
      if (EVALERR)
         return bgTmp;
      pszErrPtr = p;
      return mathfn[i].pfn(bgTmp);
      }

   /*--- Function Expression ---*/
   /*
    * this convoluted way of doing things is necessary
    * because pszErrPtr must be set after the recursion
    * but before the math fn call
    */
   for (i=0; mathfn2[i].pszFn; i++)
      {
      if (strnicmp (mathfn2[i].pszFn, *ppszExp, l=strlen (mathfn2[i].pszFn)))
         continue;

      p = *ppszExp;
      *ppszExp += l;
      if (!MthEat (ppszExp, "("))
         return SetMathErr (2, *ppszExp);

      bgTmp = ParenExp (ppszExp);
      if (EVALERR)
         return bgTmp;
      pszErrPtr = p;

      p = mathfn2[i].pszExpr;

      bgTmp2 = mathval[0].big; /*--- keep @@ scope local ---*/
      MthSetVar (0, bgTmp);
      bgTmp = _Eval (&p, 0);
      MthSetVar (0, bgTmp2);
      return bgTmp;
      }

//   /*--- Function ---*/
//   for (i=0; mathfn[i].pfn; i++)
//      {
//      if (strnicmp (mathfn[i].pszFn, *ppszExp, l=strlen (mathfn[i].pszFn)))
//         continue;
//      pszErrPtr = *ppszExp;
//      *ppszExp += l;
//      if (!MthEat (ppszExp, "("))
//         return SetMathErr (2, *ppszExp);
//      return mathfn[i].pfn(ParenExp (ppszExp));
//      }

   /*--- Constant ---*/
   for (i=0; mathconst[i].psz; i++)
      if (!strnicmp (mathconst[i].psz, *ppszExp, l=strlen (mathconst[i].psz)))
         {
         *ppszExp += l;
         return mathconst[i].big;
         }

   /*--- variables ---*/
   if (IsVar (ppszExp))
      return GetVarValue (ppszExp, TRUE, TRUE, &i);
//      return GetVar (ppszExp);

   return SetMathErr (3, *ppszExp);
   }



#define ATOMLEVEL 5
PSZ LevelOps[] = {"=", "<>", "+-", "*/%", "^"};

//   Use this to disallow assignments to variables
//   #define ATOMLEVEL 4
//   PSZ LevelOps[] = {"<>", "+-", "*/%", "^"};


BIG _Eval (PSZ *ppszExp, USHORT uLevel)
   {
   BIG big, bgTmp;
   int Op, j;
   PSZ p;

   j=0;
   if (uLevel == ATOMLEVEL)
      return EvalAtom (ppszExp);
   big = _Eval (ppszExp, uLevel+1);

   p = *ppszExp;
   while (TRUE)
      switch (Op = MthEat (ppszExp, LevelOps[uLevel]))
         {
         case '=': j = LASTVAR; big = _Eval (ppszExp, uLevel +1); MthSetVar (j, big); break;
         case '<': big *= powl (10, _Eval (ppszExp, uLevel +1)); break;
         case '>': big /= powl (10, _Eval (ppszExp, uLevel +1)); break;
         case '+': big += _Eval (ppszExp, uLevel +1); break;
         case '-': big -= _Eval (ppszExp, uLevel +1); break;
         case '*': big *= _Eval (ppszExp, uLevel +1); break;
         case '/': bgTmp = _Eval (ppszExp, uLevel +1);
                   if (bgTmp < 1E-10 && bgTmp > -1E-10)
                      SetMathErr (9, p);
                   else
                      big /= bgTmp;
                   break;
         case '%':
                   if (bPERCENT)
                      big /= (BIG)100.0; 
                   else
                      big = fmodl(big, _Eval (ppszExp, uLevel +1));
                   break;

//         case '%': big  = fmodl(big, _Eval (ppszExp, uLevel +1)); break;
         case '^': big  = powl (big, _Eval (ppszExp, uLevel +1)); break;
         default : return big;
         }
   }


///*
// * looks for a string of the form:
// *
// *  set fnName = string
// *       or
// *  set fnName() = string
// *
// */
//BOOL _cdecl MthTestForFn (PSZ psz)
//   {
//   char szName[80];
//   PSZ  p;
//
//   if (!(psz = StrSkipBy (psz, " \t")))
//      return FALSE;
//
//   if (strnicmp (psz, "set ", 4))
//      return FALSE;
//
//   if (!(psz = StrSkipBy (psz + 4, " \t")))
//      return FALSE;
//
//   for (p=szName; psz && isalnum (*psz); psz++)
//      *p++ = *psz;
//   *p = '\0';
//
//   if (!(psz = StrSkipTo (psz, "=")))
//      return FALSE;
//
//   MthAddFunction2 (szName, psz+1);
//   return TRUE;
//   }




PBIG _cdecl AToBIG (PBIG pbig, PSZ psz)
   {
   int c;
   PSZ p;

   if (!mathfn)
      BIGInitMath ();

   LASTVAR = 0;
   EVALERR = 0;
   p = psz;

   if (!p || *p == '\0' || *p == '\x0A' || *p == '\x0D')
      SetMathErr (4, p);
   else
      {
//      if (MthTestForFn (p))
//         {
//         *pbig = 0.0;
//         return pbig;
//         }
      *pbig = _Eval (&p, 0);

      if ((c = SkipWhite (&p)) && c != '\x0D' && c!= '\x0A')
         SetMathErr (3, p);
      }
   EVALERRIDX = (EVALERR && EVALERRPOS ? EVALERRPOS - psz : 0);

   if (EVALERR)
      MthInvalidate (pbig);

   return pbig;
   }



