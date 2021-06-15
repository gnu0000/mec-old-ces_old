   for (j=0; ppszJobCostSheetKey && ppszJobCostSheetKey[j]; j++)
      for (k=0; ppszJobMaterialKey && ppszJobMaterialKey[k]; k++)
      for (l=0; ppszJobCrewKey && ppszJobCrewKey[l]; l++)
         for (k=0; ppszJobCrewLaborKey && ppszJobCrewLaborKey[k]; k++)
         for (k=0; ppszJobCrewEquipmentKey && ppszJobCrewEquipmentKey[k]; k++)






void EXPORT NewRecalcJob (PSZ pszJobKey)
   {
   PSZ      pszJBKey, *ppszJBKeys, *ppszPNames, *ppszParams;
   USHORT   i, usJobItemsExist;
   HHEAP    hhpNewJobRecalc;

   hhpNewJobRecalc = ItiMemCreateHeap (4096);
   if (hhpNewJobRecalc == NULL)
      {
      ItiErrWriteDebugMessage
         ("CESUtil.calc.NewRecalcJob failed to create hhpNewJobRecalc.");
      return;
      }



   GetJobBreakdownsForJob (hhpNewJobRecalc, pszJobKey, &ppszJBKeys);
   ppszPNames = ItiStrMakePPSZ (hhpNewJobRecalc, 3, "JobKey", "JobBreakdownKey", NULL);

   if (ppszJBKeys != NULL)
      {
      /* for each JobBreakdown in the Job, calculate the JobBreakdown */
      for (i = 0; ppszJBKeys [i] != NULL; i++)
         {
         pszJBKey = ItiDbGetZStr (ppszJBKeys [i], 0);
         ppszParams = ItiStrMakePPSZ (hhpNewJobRecalc, 3, pszJobKey, pszJBKey, NULL);

         RecalcJobBreakdown (hhpNewJobRecalc, pszJobKey, pszJBKey);

   /* Check if there are any jobitems associated with a particular Job */
   /* and JobBreakdown.  If items exist, call the regular recalc       */
   /* functions.  If there are no job items in this breakdown, call a  */
   /* function to set the DetailedEstimateCost field to .00.           */
   /* A function is then called to finish recalculating JobAlternates. */

         usJobItemsExist = DoMoreJobItemsExist (hhpNewJobRecalc, pszJobKey, pszJBKey);

         if (usJobItemsExist > 0)
            ItiDBDoCalc (CalcDetailedEstimateCost, ppszPNames, ppszParams, 0);
         else
            ItiDBDoCalc(SetDetailedEstimateCostToZero, ppszPNames, ppszParams, 0);

         ItiFreeStrArray (hhpNewJobRecalc, ppszParams, 3);
         RecalcJBFunds(hhpNewJobRecalc, pszJobKey, pszJBKey);
         }/* End of for loop */

      ItiFreeStrArray (hhpNewJobRecalc, ppszJBKeys, (i?i:1));        
      ItiFreeStrArray (hhpNewJobRecalc, ppszPNames, 3);
      RecalcJFForJob(hhpNewJobRecalc, pszJobKey);
      }

   RecalcJobAltForJob(hhpNewJobRecalc, pszJobKey);
   if (NULL != hhpNewJobRecalc)
      ItiMemDestroyHeap (hhpNewJobRecalc);
   }



/* This function is used to determine whether an JobCrews exit for a particular
 * JobCostSheet. 
 * Returns 0, if the exist.  
 * Returns 1, if there are no JobCrews associated with the JobCostSheet
 *
 * Needs: hheap, 
 *        pszJobKey, 
 *        pszJobCostSheetKey 
 *
 */

// static USHORT DoJobCrewsExist (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey);




/* This function is used to determine whether JobMaterials exist for a particular
 * JobCostSheet. 
 * Returns 0, if at least one material exists.  
 * Returns 1, if there are no materials associated with the JobCostSheet
 *
 * Needs: hheap, 
 *        pszJobKey, 
 *        pszJobCostSheetKey 
 *
 */

// static USHORT DoJobMaterialsExist (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey);



/* This function is used to determine whether Laborers exist for a particular
 * JobCostSheet. 
 * Returns 0, if at least one exists.  
 * Returns 1, if there are no laborers associated with the JobCostSheet
 *
 * Needs: hheap, 
 *        pszJobKey, 
 *        pszJobCostSheetKey 
 *
 */

// static USHORT DoLaborersExist (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey);



/* This function is used to determine whether equipment exists for a particular
 * JobCostSheet. 
 * Returns 0, if at least one piece of equipment exists.  
 * Returns 1, if there is no equipment associated with the JobCostSheet
 *
 * Needs: hheap, 
 *        pszJobKey, 
 *        pszJobCostSheetKey 
 *
 */

// static USHORT DoesEquipExist (HHEAP hheap, PSZ pszJobKey, PSZ pszJobCostSheetKey);
/* This higher level calc function obtains the JobKey, JobCostSheetKey, and
 * the MaterialKey and calls functions to calculate the MaterialsUnitPrice, the
 * MaterialsUnitCost and the JobCostSheet UnitPrice
 *
 *
 * Needs: hwnd, hwndList
 *
 */

// void DoJobMaterialsUsedReCalc (HWND hwnd, HWND hwndList);


/* This higher level calc function obtains the JobKey, JobCostSheetKey, 
 * the JobCrewKey, and EquipmentKey and calls functions to calculate the 
 * EquipmentRate, EquipmentDailyCost and JobCostSheet unit price
 *
 * Needs: hwnd, hwndList
 *
 */

// void DoJobCrewEquipUsedReCalc (HWND hwnd, HWND hwndList);


/* This higher level calc function obtains the JobKey, JobCostSheetKey, 
 * from the window, and calls functions to calculate the  LaborDailyCost, EquipmentDailyCost,
 * and JobCostSheet unit price
 *
 * Needs: hwnd
 *
 */

// void DoJobCostShtReCalc (HWND hwnd);



/* This higher level calc function obtains the JobKey, JobCostSheetKey, 
 * JobCrewKey, LaborerKey from the window, and calls functions to calculate 
 * the  LaborRate, LaborDailyCost, and JobCostSheet unit price
 *
 * Needs: hwnd, hwndList
 *
 */

// void DoJobCrewLaborUsedReCalc (HWND hwnd, HWND hwndList);



/* This higher level calc function obtains the JobKey, JobCostSheetKey, 
 * from the window, and calls functions to recalculate 
 * the  MaterialsUnitCost and JobCostSheet unit price when a material
 * has been deleted.
 *
 * Needs: hwnd
 *
 */

// void DoDelJobMaterialsUsedReCalc (HWND hwnd);



/* This higher level calc function obtains the LaborerKey from 
 * the window, and calls functions to recalculate 
 * the LaborRate
 *
 * Needs: hheap
 *        hwndList
 *        JobKey
 *        JobCostSheet
 *        JobCrewKey
 *
 */

// void DoCalcJobCrewLaborRate (HHEAP hheap, HWND hwndList, PSZ pszJobKey, PSZ pszJobCostSheetKey, PSZ pszJobCrewKey);



/* This higher level calc function obtains the EquipmentKey from 
 * the window, and calls functions to recalculate the EquipmentRate
 *
 * Needs: hheap
 *        hwndList
 *        JobKey
 *        JobCostSheet
 *        JobCrewKey
 *
 */

// void DoCalcJobCrewEquipRate (HHEAP hheap, HWND hwndList, PSZ pszJobKey, PSZ pszJobCostSheetKey, PSZ pszJobCrewKey);







