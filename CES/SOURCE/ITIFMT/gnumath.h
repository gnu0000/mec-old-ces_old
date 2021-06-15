/*
 * GnuMath.h
 *
 * (C) 1992,1993 Info Tech Inc.
 *
 * Craig Fitzgerald
 *
 * This file math functions
 *
 ****************************************************************************
 *
 *  QUICK REF FUNCTION INDEX
 *  ========================
 *
 * PBIG   AToBIG (pbig, psz)
 *
 * BOOL   MthValid (bgNum)
 * void   MthInvalidate (pbgNum)
 * USHORT MthMatch (pbg1, pbg2)
 * USHORT MthIsError (*ppszErrStr, *puErrIdx)
 *
 * USHORT MthAddFunction  (pszName, pfn)
 * USHORT MthAddFunction2 (pszName, pszExpr)
 * USHORT MthAddConstant  (pszName, big)
 * USHORT MthAddVariable  (pszName, big)
 * BOOL   MthTestForFn (psz)
 *
 * PSZ    MthFmat (pszNum, bgNum, uFmt)
 *
 * BIG    MthSetVar (uIndex, big)
 *
 ****************************************************************************
 */


#define QUAN 1

typedef long double BIG;

typedef BIG *PBIG;

typedef BIG (*PMTHFN) (BIG big);



/*
 * This function converts a string containing a number or
 * expression into a BIG value.  On error the number
 * returned is marked as invalid.  This fn can handle just
 * about any expression you can dream up including std math
 * functions like sinh() and log()
 * the constants pi and e are also defined
 * you can also create/define variables, see below
 *
 * initial valid functions are:
 *       abs()    acos()   asin()   atan()   ceil()
 *       cos()    cosh()   exp()    floor()  log()
 *       log10()  sin()    sinh()   sqrt()   tan()
 *
 * operators in precidence order:
 *      ()      -- parenthesis
 *      ^       -- power
 *      * / %   -- mult, div, mod
 *      + -     -- add, subtrtact
 *      < >     -- base 10 shift
 *
 * examples:
 * ---------
 *     psz: 10.5
 *  result: 10.5
 * comment: normal _atol like functionality
 *
 *     psz: 100 + .9 *(exp(1.2) / sin(pi/3)) % 20
 *  result: whatever
 * comment: % means mod.
 *
 *     psz: log (0)
 *  result: invalid number
 * comment: see error functions
 *
 *     psz: @x = 500
 *  result: 500
 * comment: @x is created and given the value 500. vars must start with @
 *
 *     psz: 100 + @x
 *  result: 600
 * comment: @x was created in the previous example
 *
 *     psz: 100 + @y
 *  result: 100
 * comment: @y was created. vars are initialized to 0
 */
PBIG _cdecl AToBIG (PBIG pbig, PSZ psz);



/*
 * This tells if bgNum has been marked as invalid with MthInvalidate
 */
BOOL _cdecl MthValid (BIG bgNum);


/*
 * This marks a BIG number as invalid
 */
void _cdecl MthInvalidate (PBIG pbgNum);


/*
 * This returns a number saying how close the numbers are to
 * each other. The returns are:
 *
 * return   meaning
 * ----------------------------
 *  0 -     one or more invalid
 *  1 -     both valid and matched to within .000005
 *  2 -     both valid and matched to within .005
 *  3 -     both valid but not matched
 */
USHORT _cdecl MthMatch (PBIG pbg1, PBIG pbg2);



/*
 * convert BIG to a string.
 *
 * uFmt string format
 * -------------------
 *  0   9.3
 *  1   $8.5
 *  2   $9.2
 *  3   8.5
 *  4   9.2
 *  5   7.5
 *
 *  6   9.3 no commas
 *  7   8.5 no commas
 *  8   9.2 no commas
 *  9   7.5 no commas
 *
 *  10  12.5 no commas. If larger use exp notation
 *
 */
PSZ _cdecl MthFmat (PSZ pszNum, BIG bgNum, USHORT uFmt);


/*
 * none of your business
 */
BIG _cdecl MthSetVar (USHORT uIndex, BIG big);


/*
 *
 * After each call to AToBig, this fn stores the error state
 * This fn returns a non zero value in case of an error
 *
 * *ppszErrStr will contain a description of the error
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
USHORT _cdecl MthIsError (PSZ *ppszErrStr, USHORT *puErrIdx);



/*
 * Adds a function that AToBIG can use
 * the function must be of type PMTHFN
 *
 * call with pfn = NULL to remove fn
 */
USHORT _cdecl MthAddFunction (PSZ pszName, PMTHFN pfn);


/*
 * Adds a function that AToBIG can use
 * the function is a string (ie: @@ * @@ / @x * pi )
 * there the variable @@ is the fn's parameter value
 *
 * call with pfn = NULL to remove fn
 *
 * functions can also by added via AToBIG and MthTestForFn like this:
 *
 *    set MyFun = @@ * sin (@@)
 */
USHORT _cdecl MthAddFunction2 (PSZ pszName, PSZ pszExpr);


/*
 * adds a constant. Like This:
 *
 *  MthAddConstant ("pi", 3.1415926);
 */
USHORT _cdecl MthAddConstant (PSZ pszName, BIG big);


/*
 * adds a variable. Prefix with a '@' Like This:
 *
 *  MthAddConstant ("@num", 212.0);
 */
USHORT _cdecl MthAddVariable (PSZ pszName, BIG big);


/*
 * checks to see if str is of the form:
 *    set Fn = @@ * 22
 * if so, it adds the function and returns true
 * This fn is automatically called by AToBIG
 */
BOOL _cdecl MthTestForFn (PSZ psz);


/*
 *  by default, % is treaded as a modulus operator
 *  if you call this fn with a FALSE value,
 *  % is treated as a percent operator.
 */
void _cdecl MthPercentIsMod (BOOL b);

