/*
 * BAMSCES.IPF
 * Charlie Fox
 * Copyright (C) 1990 AASHTO
 *
 */
                         /****************************************
                           * DO NOT WRITE TEXT AFTER COLUMN 80  *
                            ************************************
                             **********************************
                              ********************************
                               ******************************
                                ***************************/


#include "help.h"
#include "menu.h"
#include "..\include\itimenu.h"
#include "..\include\winid.h"
:userdoc.
:title.Help for BAMS/CES

/*******************/
/* File Menu Help **/
/*******************/

  :h1 res=IDM_FILE_MENU.FILE MENU
:p.The :hp4.File Menu:ehp4. lets you perform the following file-related 
functions:  opening new and existing programs and jobs, opening catalogs, 
closing windows, setting up the printer and printing, importing and 
exporting files, running browsers and other system applications, and 
exiting the program.   
:p.The File window is always available (except when a dialog box is open) and will 
always have the same menu choices, although not all the sub menus will be available 
all the time.  For example, the Print menu prints reports based on the active window; 
therefore, only the print choices that logically apply to that window will be 
available.  Nevertheless, these commands are always executable. 
These menu choices will be discussed in the order they appear under the File 
menu choice.
:p.Several of these choices call up dialog boxes (boxes that allow you to interact 
with the program to tailor the results to your preferences), and these will be 
discussed in turn.   

.*  File Menu Relate Functions Help **

     :p.:link reftype=hd res=IDM_NEW_JOB.New Job.:elink.
:p.The New Job command allows you to create an entirely new job any time 
during your work in CES.

     :p.:link reftype=hd res=IDM_NEW_PROGRAM.New Program:elink.
:p.The New Program command displays the New Program dialog box. This allows 
you to define an entirely new road construction program.

     :p.:link reftype=hd res=IDM_OPEN_JOB.Open Job:elink.
:p.The Open Job command opens a window listing all the Jobs currently in 
the database.  If no jobs exist, a skeleton screen will appear.

     :p.:link reftype=hd res=IDM_OPEN_PROGRAM.Open Program:elink.
:p.The Open Program command displays a list of the existing road construction
programs.

     :p.:link reftype=hd res=IDM_OPEN_CATALOG_MENU.Open Catalog Menu:elink.
:p.The Open Catalog command opens a window into the Catalog database.

     :p.:link reftype=hd res=IDM_CLOSE.Close:elink.
:p.The Close command closes the currently active window.

     :p.:link reftype=hd res=IDM_PRINT_MENU.Print Menu:elink.
:p.The Print command generates reports based on the active window.

     :p.:link reftype=hd res=IDM_PRINTER_SETUP.Printer Setup:elink.
:p.The Printer Setup command runs the OS/2 Printer Setup routines that 
allow the user to configure a printer.

     :p.:link reftype=hd res=IDM_IMPORT.Import:elink.
:p.The Import command imports information from a flat file, such as PEMETH or PES, 
into the CES database.

     :p.:link reftype=hd res=IDM_EXPORT. Export:elink.
:p.The Export command exports data (for instance, PES project data) to an external 
file outside the CES database.

     :p.:link reftype=hd res=IDM_RUN_MENU.Run Menu:elink.
:p.The Run command is primarily for running browsers or for running other 
programs besides CES.

     :p.:link reftype=hd res=IDM_PROCESS_STATUS.Process Status:elink.
:p.The Process Status command gives information about reports running in the background 
and also gives the percent complete for each report.

     :p.:link reftype=hd res=IDM_EXIT. Exit:elink.
:p.The Exit command closes CES.


  :h1 res=IDM_NEW_JOB.New Job
:p.The :hp4.New Job:ehp4. command allows you to create an entirely new job at any time during 
your work in CES.  Executing this command causes the full screen dialog box 
to appear.  
:p.Boxes with down-pointing arrows to their right are combination boxes
that is, they have a self-contained list from which you must choose your entry.  
In most cases, these boxes will not accept typed entry, only selected entry. 
View the lists in these boxes by clicking on the arrow or by scrolling through 
the list with the keyboard arrow keys.
:p.To create a new job, perform the following steps:  
:ul compact.
:li.Select the New Job command from the File menu.
:li.Fill in the plain entry fields by manually typing in the data.  The Location 
field must be expressed in terms of latitude and longitude (degrees, minutes, 
and seconds) and the Construction and Contingency percents must be entered as 
percent values--for 8 percent, type either .08 or 8%. 
:li.For the seven combination boxes (Checked by, Approved by, Estimator, Work 
Type, Work Day Calendar, Letting Month, and District), choose from the lists 
provided.  These fields will not accept typed-in data.   
:li.The small square next to the Location box specifies whether or not Davis-Bacon 
wages apply.  Click on this box to toggle its display.  An X in the box means the 
condition is positive (checked) and therefore Davis-Bacon wages would apply.  
:li.At this point you can accept the job, go back and make any changes necessary 
by choosing the box that needs correction, or cancel the entire entry.  You can 
use the Help command at any stage in the process to get help with the current 
menu or dialog box.
:eul.
  
  :h1 res=IDM_NEW_PROGRAM.New Program
:p.The :hp4.New Program:ehp4. command displays the New Program.  This allows 
you to define an entirely new road construction program.
:p.To create a New Program, perform the following steps:
:ul compact.
:li.Perform the File, New Program command sequence.
:li.Fill in the four entry fields.  CES can read most date formats.
:li.Accept the entry, cancel the entry, ask for help, or accept the entry 
and create another new program with the Add More command.
:eul.
  
  :h1 res=IDM_OPEN_JOB.Open Job
:p.The :hp4.Open Job:ehp4. command opens a window listing all the Jobs 
currently in the database. If no jobs exist, a skeleton screen will appear.
:p.From here, the user can cancel, ask for help, or select a job and hit OK. 
All of the list boxes provided by CES will automatically highlight (select) 
the first item in the list.  Therefore, if you select OK without choosing 
some other item, the first item will be chosen.
   
  :h1 res=IDM_OPEN_PROGRAM.Open Program
:p.The :hp4.Open Program:ehp4. command displays a list of the existing road 
construction programs.  If no programs have been defined, CES will display 
a skeleton Open Program box.
Once at this window, you can request help, select a program and hit enter 
or OK, or cancel the Open Program operation.
:p.The parent/child window hierarchy in BAMS/CES is not strict.  
Therefore, if you began your work session by opening the Program window, 
you can move to one of the job-related windows by viewing the jobs in 
this program, selecting one of them, and then proceeding as if you had 
begun with the Open Job sequence.  

  


.***************************
.* OPEN CATALOG MENU HELP **
.***************************

  :h1 res=IDM_OPEN_CATALOG_MENU.Open Catalog Menu
:p.The :hp4.Open Catalog:ehp4. commamnd opens into the Catalog database. 
The next window to be displayed will be the Catalog listing window. It will 
display 17 to 19 catalogs, depending upon your access privilege. The following 
catalogs are displayed:

  
.*  Open Catalog Menu Relate Functions Help **


     :p. :link reftype=hd res=ApplicationS.Application Catalog:elink.
:p.The Application Catalog window displays all the current applications.

     :p. :link reftype=hd res=AreaTypeCatS.Area Type Catalog:elink.
:p.The Area Type Catalog window displays a catalog (list) of area types. 

     :p. :link reftype=hd res=CodeTableS.Code Table Catalog:elink.
:p.The Code Table Catalog window displays a list of all the Code Tables.

     :p. :link reftype=hd res=CostSheetCatS.Cost Sheet Catalog:elink.
:p.The Cost Sheet Catalog window displays a list of all the current cost sheets 
in the cost-based estimation module.  

     :p. :link reftype=hd res=CountyCatS.County Catalog:elink.
:p.The County Catalog window displays the catalog of counties within the 
state. It can be accessed from the Area Catalog window by selecting an area 
and choosing the desired area in  the Counties box.  

     :p. :link reftype=hd res=CrewCatS.Crew Catalog:elink.
:p.The Crew Catalog window displays a list of all crews currently maintained 
by the cost-based estimation module.

:p. :link reftype=hd res=DefaultUnitPriceCatS.Default Unit Price Catalog:elink.
:p.The Default Unit Price Catalog window displays a catalog of standard items, 
their unit of measure, and their Default Price.   Double clicking on an item 
will bring up the Default Unit Price window.

     :p. :link reftype=hd res=EquipmentCatS. Equipment Catalog:elink.
:p.The Equipment Catalog window displays the complete list of equipment used 
in the cost-based estimation module. To get further information about a piece 
of equipment, double click on that line (or select that piece of equipement 
and hit enter).  

     :p. :link reftype=hd res=FundCatS.Fund Catalog:elink.
:p.The Fund Catalog window displays the catalog of funds that are used to 
support all or part of a job created by BAMS/CES.  

     :p. :link reftype=hd res=InflationPredictionCatS.Inflation Prediction Catalog:elink.
:p.The Inflation Prediction Catalog window shows inflation predictions that 
will affect pricing.   

     :p. :link reftype=hd res=LaborCatS.Labor Catalog:elink.
:p.The Labor Catalog window displays all the laborer types used in cost-based 
estimation.  

     :p. :link reftype=hd res=MajorItemCatS.Major Item Catalog:elink.
:p.The Major Item Catalog window displays the catalog of all major items used 
by BAMS/CES.  A major item is a general, quantifiable portion of work.  
Examples of major items are cubic yards of concrete, pounds of steel, and 
cubic yards of excavation.  Major items are not biddable items.

     :p. :link reftype=hd res=MapCatS.Map Catalog:elink.
:p.The Map Catalog window displays the catalog of all maps used by BAMS/CES.  
A map is a collection of spatial points which describesone or more geographical 
areas.  There are maps for counties and zones.  

     :p. :link reftype=hd res=MaterialCatS.Material Catalog:elink.
:p.The Material Catalog window displays the catalog of all materials used by 
BAMS/CES.  A material is anything that is used up (as opposed to just used) 
when performing work.Two examples of materials are concrete and steel.  

     :p. :link reftype=hd res=StandardItemCatS.Standard Item Catalog:elink.
:p.The Standard Item Catalog window is a list of the standard construction 
items (biddable units of work) allowed by BAMS  for use in jobs, projects, 
and proposals.  

     :p. :link reftype=hd res=UserCatS.User Catalog:elink.
:p.The User Catalog window is a list of the Users that can use CES. 

     :p. :link reftype=hd res=VendorCatS.Vendor Catalog:elink.
:p.The Vendor Catalog window displays a list of all vendors used by BAMS/CES.  

     :p. :link reftype=hd res=WorkingDayCalendarCatS.Working Day Catalog:elink.
:p.The Working Day Calendar Catalog window displays a list of working day 
calendars and describes situations for which these calendars apply.
(A working day calendar shows the number of working days in a month, as opposed 
to the total number of days in a month.)  

     :p. :link reftype=hd res=ZoneTypeCatS.Zone Type Catalog:elink.
:p.The Zone Type Catalog window can displays a list of all zone types used by 
BAMS/CES. A zone is an area of the state that does not depend on county 
boundaries.  

  :h1 res=IDM_CLOSE.Close
:p.The :hp4.Close:ehp4. command closes the currently active window.  It performs the same 
function as the dash (control bar) in the upper left-hand 
corner of each active window.  Thus, if you have several windows open, this 
command will close only the active window.  If the active window is a Program 
or Job window, this command will close the Program or Job.  
:p.Due to the tree-structure nature of this program, there will be times when many 
windows will be open at one time.  You may wish to close all of them before exiting 
or you may simply close them to keep system performance as high as possible.  In 
either case, there are two ways to close all the open windows:  The obvious way 
is to close each window in the opposite order in which they were opened.  
However, this can be frustrating and time consuming if more than three or four 
are open.
:p.The simpler way to do this is to click on the first window opened.  CES will 
display it in front of the other windows (its children).  
Close the first one (the parent) and all its children will 
close automatically with it.  This demands some foresight in that some portion 
of the window must be visible in order to be able to click on it.  In addition, 
it is helpful if you can see the window title.  
:p.Therefore, a good method for using multiple windows in CES is to move each new 
window just beneath its parent.  This is done by clicking in the title bar 
the top half inch of every screen) and dragging it to its new location.  Then 
the title bar of each window will be visible.  This yeilds other benefits as 
well since it allows you to selectively go back to earlier windows.  

.********************
.* Print Menu Help **
.********************

  :h1 res=IDM_PRINT_MENU.Print Menu
:p.The :hp4.Print:ehp4. command generates reports based on the active window.
In other words, it will print the report that summarizes the active window.
In  many cases, there will be only one choice from the print menu,
since there will be only one report based on that window.
:p.Below is the list of reports printed by CES.  As stated above, availability 
depends on the active window.  If a report title is gray, it is unavailable 
at this time.
:p.If a print option in the dialog boxes is grayed, it is currently under 
development and was not functional for this Beta release.
Note however that all the print options will be gray when the print menu is 
first activated (like the windows in CES). This allows the program time to 
process the various information it needs to perform the commands.  The length 
of time these will remain gray depends on the amount of information in the 
database and the amount of activity on the network if one is installed.


.*  Print Menu Relate Functions Help **



     :p. :link reftype=hd res=rAreas_RptDlgBox.Area Listing Report:elink.
:p.The Area Listing Report contains information about the area types that 
have been selected for printing.  The Area Listing Report is printed 
from the Area Type Catalog.      
   
     :p. :link reftype=hd res=RBASED_RptDlgBox.Base Date Listing Report:elink.
:p.The Base Date Listing Report lists all the base dates currently defined 
in the system.  This report can be printed from all the catalogs and several 
other windows. 

     :p. :link reftype=hd res=rCodeTab_RptDlgBox.Code Table Listing Report:elink.
:p.The Code Table Listing Report contains information about the contents of 
selected code tables.  You must be in the Code Table Catalog to print this 
report.

     :p. :link reftype=hd res=RCONTY_RptDlgBox.County Listing Report:elink.
:p.The County Listing Report lists all or selected counties in this job or 
program.  You can select as few or as many counties as you like.

:p. :link reftype=hd res=rEquip_RptDlgBox. Equipment Listing Report:elink.

     :p. :link reftype=hd res=rFund_RptDlgBox.Fund Listing Report:elink.
:p.The Fund Listing Report lists all or selected funds for this job or program.

     :p. :link reftype=hd res=rJobEst_RptDlgBox.Job Estimate Report:elink.
:p.The Job Estimate Report prints detailed information about a job estimate.
   
     :p. :link reftype=hd res=rJobFund_RptDlgBox.Job Funding Report:elink.
:p.The Job Funding Report displays funding information for the selected job.

     :p. :link reftype=hd res=rJobSch_RptDlgBox.Job Item Schedule Report:elink.
:p.The Job Item Schedule Report lists the items to be performed for a job.

     :p. :link reftype=hd res=rMap_RptDlgBox.Map Listing Report:elink.
:p.The Map Listing Report presents information about all or selected maps in 
a job or program.
   
     :p. :link reftype=hd res=RMAT_RptDlgBox.Materials Listing Report:elink.
:p.The Material Listing Report prints information about all or selected 
necessary materials. The user also has the option to see a listing of prices,
by zone, for each material.  Each listing would be printed immediately after 
the material it represents.  (See Material Listing Report:  Prices.)

     :p. :link reftype=hd res=rMjrIt_RptDlgBox.Major Item Listing Report:elink.
:p.The Major Item Listing Report lists information about all or selected major items. 
      
     :p. :link reftype=hd res=rPrgEst_RptDlgBox.Program Estimate Report:elink.
:p.The Program Estimate Report lists information about the job estimates in 
a program.  The user can vary the format with respect to listing order, 
subtotals, and totals.  
   
     :p. :link reftype=hd res=rPrgFnd_RptDlgBox.Program Funding Report:elink.
:p.The Program Funding Report provides information concerning the funding of 
a program.

     :p. :link reftype=hd res=rPrgSch_RptDlgBox.Program Schedule Listing Report:elink.
:p.The Program Schedule Report provides information about job scheduling within 
a program.

     :p. :link reftype=hd res=rStdIt_RptDlgBox.Standard Item Listing Report:elink.
:p.The Standard Item Listing Report lists information about all or selected 
standard items.
   
     :p. :link reftype=hd res=rUser_RptDlgBox.User Listing Report:elink.
:p.The User Listing Report displays about the users.

     :p. :link reftype=hd res=rVend_RptDlgBox.Vendor Listing Report:elink.
:p.The Vendor Listing Report lists information about all or selected vendors.
   
     :p. :link reftype=hd res=rZones_RptDlgBox.Zone Listing Report:elink.
:p.The Zone Listing Report lists information about any or all of the project 
zone types.


.*Printer SetUP **

  :h1 res=IDM_PRINTER_SETUP.Printer Setup
:p.The :hp4.Printer Setup:ehp4. command runs the OS/2 Printer Setup routines 
that allow the user to configure a printer.

.* Import  **
 
  :h1 res=IDM_IMPORT.Import
:p.The :hp4.Import:ehp4. command imports information from a flat file, such 
as PEMETH or PES, into the CES database.(See the CES System Manager's Guide 
for flat file layouts and more detailed instructions about importing data 
into CES.)

.* Export  **

  :h1 res=IDM_EXPORT. Export
:p.The :hp4.Export:ehp4. command exports data (for instance, PES project data)
to an external file outside the CES database. (See the CES System Manager's 
Guide for specific instructions about exporting data from CES to another 
BAMS module or to another program.)

.********************
.* Run Menu Help   **
.********************


  :h1 res=IDM_RUN_MENU.Run Menu
:p.The :hp4.Run:ehp4. command is primarily for running browsers or for running 
other programs besides CES. The following options are listed below.
/*:p.The Run Command Pull-Down Menu
:p.The Run command can be executed from anywhere in CES.  To use this option, 
execute the following commands:
:p.File,
:p.Run.  
:p.CES provides four browsers to facilitate searches.  Each of these will be discussed in turn.
*/

.*  Run Menu Relate Functions Help **

     :p. :link reftype=hd res=IDM_RUN_ITEM_BROWSER.Item Browser:elink.
:p.Select this option to run Item Browser.
    
     :p. :link reftype=hd res=IDM_RUN_PROPOSAL_BROWSER.Proposal Browser:elink.
:p.Select this option to run Proposal Browser.

     :p. :link reftype=hd res=IDM_RUN_FACILITY_BROWSER.Facility Browser:elink.
:p.Select this option to run Facility Browser.

     :p. :link reftype=hd res=IDM_RUN_SQL_BROWSER.SQL Browser:elink.
:p.Select this option to run SQL Browser.

     :p. :link reftype=hd res=IDM_RUN_OTHER.Other:elink.
:p.Select this option to run Other Programs besides CES.

.* Run Options Help **

  :h1 res=IDM_RUN_ITEM_BROWSER.Item Browser
:p.The :hp4.Item Browser:ehp4. searches for all appearances of this item in 
all proposals.  It will locate items that have been passed from DSS.  The boxes 
and selections in this browser are constraints to narrow the focus of the search.  
You do not have to fill out more than one of the specifications.  You can fill 
out any combination of constraints.
:p.To run the Item Browser, perform the following steps:
:ul compact.
:li.Perform the File, Run command sequence.
:li.Select Item Browser.  CES will display the dialog box.  
:li.Fill in the Item Specifications.  The Quantity field is a range. 
:li.Fill in the Proposal Specifications.  The Quantity, Estimate, and Letting 
Date fields are ranges.  The default search is for all quantities, all estimates, 
and all letting dates.  Therefore, if you leave any of these blank, they will not 
constrain the search.  If you enter text in the first of the range fields, it will 
search from that item forward.  
:li.Select the work types, road types, and districts you want included in the search.  
You can select several from each of these boxes.  As in the range selectors above, 
the default search is the entire list.  Therefore, if the work type, road type, 
and  districts boxes have no selected items, the search will not be constrained.  
Selecting some of the items limits the search to the selected items.  (See Chapter 
3, Section 5, the Tutorial chapter for a detailed description of selecting multiple 
items in CES.)
:li.The last choice is the selection box with three radio buttons that defines the 
number of proposals searched.  You can select only one of these choices.  
:li. Execute the search with the Search button located in the lower left hand corner 
of the dialog box.
:eul.

  :h1 res=IDM_RUN_PROPOSAL_BROWSER.Proposal Browser
:p.The :hp4.Proposal Browser:ehp4. searches for Proposals. You can specify as 
much or as little detail as you desire.  The ranges and other specifications 
are ways to limit the scope of the search. The default search is the entire 
list.  
:p.To run the Proposal Browser, perform the following steps:
:ul compact.
:li.Perform the File, Run command sequence.  
:li.Select the Proposal browser choice.  CES will display the dialog box.  
:li.You can specify as much or as little detail as you desire.
  The ranges and 
other specifications are ways to limit the scope of the search.  The default 
search is the entire list.  
:li.Execute the search with the search radio button located in the lower left 
hand corner of the dialog box.
:eul. 

 :h1 res=IDM_RUN_FACILITY_BROWSER.Facility Browser
:p.The :hp4.Facility Browser:ehp4. searches for Facilities where this kind of 
material is produced. You can specify as much or as little detail as you desire.
The ranges and other specifications are ways to limit the scope of the search. 
The default search is the entire list.  
:p.To run the Facility Browser, perform the following steps:
:ul compact.
:li.Perform the File, Run command sequence.  
:li.Select the Facility browser choice.    
:li.Execute the search with the search radio button located in the lower left 
hand corner of the dialog box.
:eul.

  :h1 res=IDM_RUN_SQL_BROWSER.Sql Browser
:p.The :hp4.SQL Browser:ehp4. (SQL is an abbreviation for Structured Query 
Language), therefore your search must be in SQL.  
:p.To run the SQL browser, perform the following steps:
:ul compact.
:li.Perform the File, Run command sequence.  
:li.Select the SQL Browser choice.  
:li.The Copy button to the right of the Query box copies the query to the clipboard.  
You can then paste that information into another location in CES or even into 
another program.  For example, you might wish to copy the data into a spreadsheet 
program for graphing and inclusion in another document.
:li.The Paste button pastes (inserts) the clipboard contents into the Query box.  
For example, if you had a query from some other area in your clipboard, rather 
than rewriting the query, you could simply paste it from the clipboard to the 
dialog box and perform the search.  The Paste command can also copy the clipboard 
contents into Microsoft Word and Excel. 
:li.To execute the search, click on the Execute button or press Enter.
:eul.

  :h1 res=IDM_RUN_OTHER.Other
:p.The :hp4.Run Other:ehp4. command runs other programs while still running CES.

.* Process Status Help **

  :h1 res=IDM_PROCESS_STATUS.Process Status
:p.The :hp4.Process Status:ehp4. command gives information about reports 
running in the background and also gives the percent complete for each report.  
Since OS/2 is a multi-tasking environment, you can choose one report and while 
it is generating, choose another.  Then you can check Process Status to 
determine the progress of each. 

.* Exit Help **

  :h1 res=IDM_EXIT. Exit
      :p.The :hp4.Exit:ehp4. command exits CES.

.********************   
.* Edit Menu Help  **
.********************


  :h1 res=IDM_EDIT_MENU. EDIT MENU
:p.The :hp4.Edit Menu:ehp4. allows you to alter the data in the active window 
and to copy the data to and from the clipboard.The clipboard allows the user 
to store data (anything from an item to a catalog) in memory.  This data can 
then be inserted (pasted) back into CES.  The data will remain in the 
clipboard until it is replaced by other data.  

.* Edit Menu Related Options Help **

     :p. :link reftype=hd res=IDM_ADD.Add:elink.
:p.The Add command adds another element to the currently active window.
    
     :p. :link reftype=hd res=IDM_CHANGE.Change:elink.
:p.The Change command changes an element in the currently active window.

     :p. :link reftype=hd res=IDM_DELETE.Delete:elink.
:p.The Delete command deletes an element in the currently active window.

     :p. :link reftype=hd res=IDM_CUT.Cut:elink.
:p.The Cut command deletes the currently selected element from the text and 
moves it to the clipboard.

     :p. :link reftype=hd res=IDM_COPY.Copy:elink.
:p.The Copy command stores any selected text into the clipboard.

     :p. :link reftype=hd res=IDM_PASTE.Paste:elink.
:p.The Paste command copies the clipboard contents to the currently selected
window.

.* Edit Commands Help **

  :h1 res=IDM_ADD.Add
:p.The :hp4.Add:ehp4. command  adds another element to the currently active 
window (an element could be a job, a job item, etc.)  This window is 
context-sensitive, in other words, the information added will depend upon the 
window  that is active. In most cases this will call up a blank dialog box 
for that window.  You then fill in the boxes by typing data in the plain boxes,
selecting from the lists provided in the combination boxes (arrows on the right 
of the box), and clicking on the selection boxes (little square boxes that 
toggle between checked and not checked).

  :h1 res=IDM_CHANGE.Change
:p.The :hp4.Change:ehp4. command changes an element  in the currently active 
window.  This command is also context-sensitive.  This command will call up a 
dialog box identical to that of the Edit, Add command sequence, except that 
the Change dialog box will be filled out.  You access this box by selecting 
the item you wish to change and then performing the Edit, Change command 
sequence. After you have finished your changes, you can either accept them, 
get help, or cancel.  If you select cancel, none of the changes you have made 
in this dialog box will be saved.  There is not an Add More choice button; 
therefore, you must invoke the Change dialog box each time you wish to make 
a change to some element.   

:h1 res=IDM_DELETE.Delete
:p.The :hp4.Delete:ehp4. command deletes an element from the currently active 
window.  It is also context-sensitive.  For example, to delete a county from 
the county catalog, you must open the county catalog and select the county or 
counties you wish to delete.  If nothing is selected, nothing will be deleted. 
The program will not provide a duplicate of the Add/Change dialog box but will
provide a choice box on the screen, asking you to confirm or cancel the deletion.  
The default for this question is no (the No choice will be highlighted when 
the box appears).  Therefore, if you hit enter without thinking, the item will 
not be deleted.   

  :h1 res=IDM_CUT.Cut
:p.The :hp4.Cut:ehp4. command deletes the currently selected element from the 
text and moves it to the clipboard.  You can then paste (insert) it back into 
the same location or a different one.  One example for a use of this command 
is cutting a job item to a different job.  Move to the Job Items window, 
select a Job Item, and then perform the Edit, Cut command sequence. The item 
will disappear from the window but will be retained in RAM.  You then find 
the place you want to insert it and perform the Edit, Paste command sequence.
There is no time limit for elements in the clipboard, as long as CES is running 
and nothing else is put in.  You can perform several other functions, change 
windows, and add new or change current data, before you paste the information 
to the desired location. As with all commands in a Windows style format, if 
the command is grayed, it is not functional or available at that time.

  :h1 res=IDM_COPY.Copy
:p.The :hp4.Copy:ehp4. command sequence stores any selected text into the 
clipboard.  The difference between the Cut and Copy commands is that with the 
Copy command, the clipboard contents remain in their original location as well
as being retained in the clipboard.  The Cut command deletes them from the 
text, although they can be replaced from the clipboard.

  :h1 res=IDM_PASTE.Paste
:p.The :hp4.Paste:ehp4. command copies the clipboard contents to the currently 
selected window.  If the text being copied is not the correct format, there 
will be an error message.  For example, if the window requires a numerical 
entry and the user enters text data, this will cause an error message.  However, 
if the user simply copies a personal or company name where the name of a 
bridge should have gone,the program has no way to distinguish the correct from 
the incorrect. As explained in the Cut and Copy sections, there is no time 
limit for pasting from the clipboard to the program.  As long as nothing else 
is put into the clipboard and CES is not shutdown, this data will be in the 
clipboard. Additionally, there is no limit on the number of times that an 
element can be pasted into CES.  The contents of the clipboard do not change 
after the paste operation.  This is a convenient feature if you need to insert 
one element into several places or if you insert it incorrectly the first time.  
If this happens, delete the new entry, move the cursor to the new location, and 
perform the Edit, Paste command sequence again.  If you did not add anything 
to the clipboard, the item you pasted incorrectly the first time will be pasted 
again. 

.* End of Edit Menu Functions **

   
.********************
.* View Menu Help  **
.********************

  :h1 res=IDM_VIEW_MENU.View Menu
:p.The :hp4.View Menu:ehp4.  switches focus from the current (active) window 
to a logically connected (parent or child) window.


     :p. :link reftype=hd res=IDM_PARENT.Parent:elink.
:p.This window displays the parent window.


  :h1 res=IDM_PARENT.Parent
:p.This pull-down window is divided by a horizontal line.  The :hp4.Parent:ehp4. 
command (the upper half of the box) switches focus from the current window to 
its parent window. The area below the line is a context-sensitive view option, 
in other words, it will allow you to choose between the subsets of the current 
(active) window. For example, the Job window has several child windows (Job 
Alternates, Job User List, Job Breakdown, Job Comment, etc.).  These are 
presented in list form.  Simply select the window desired.

.************************
.* Utilities Menu Help **
.************************


  :h1 res=IDM_UTILITIES_MENU.Utilities Menu
:p.The :hp4.Utilities Menu:ehp4. contains various utility commands that apply 
to the BAMS/CES application. These commands range from searches and sorts, 
to system preferences and login procedures.

.* Utilities Menu related options **



     :p. :link reftype=hd res=IDM_FIND.Find:elink.
:p.The Find command finds a specific element in a displayed list.


     :p. :link reftype=hd res=IDM_SORT.Sort:elink.
:p.The Sort command sorts a list of elements in the currently active list window.


     :p. :link reftype=hd res=IDM_UPDATE_BASE_DATE.Update Base Date:elink.
:p.The Update Base Date command attaches new reference data to all jobs.


     :p. :link reftype=hd res=IDM_UPDATE_SPEC_YEAR.Update Spec Year:elink.
:p.The Update Spec Year command gives the System Manager information on errors and/or messages.


     :p. :link reftype=hd res=IDM_SET_PREFERENCES.Set Preferences:elink.
:p.The Set Preferences command allows you to set preferences specific to your current environment.


     :p. :link reftype=hd res=IDM_CHANGE_PASSWORD.Change Password:elink.
:p.The Change Password command allows you to Change your CES account password.


     :p. :link reftype=hd res=IDM_SHOW_USERS.Show Users:elink.
:p.The Show Users command lists the users currently connected to any PC module.


     :p. :link reftype=hd res=IDM_SEND_MESSAGE.Send Message:elink.
:p.The Send Message command allows the user to send a message to another PC user on the system.


     :p. :link reftype=hd res=IDM_INSTALL.Install:elink.
:p.The Install command facilitates update installations.


     :p. :link reftype=hd res=IDM_DENY_LOGON.Deny Logon:elink.
:p.The Deny Logon command allows the System Manager to install updates or perform other 
maintenance on the system without other users logging on.


     :p. :link reftype=hd res=IDM_GENERATE_SUPPORT_REQUEST.Generate Support Request:elink.
:p.The Generate Support Request command automatically generates a support request.



.* end of utilities menu related options **


.* utilities options **

  :h1 res=IDM_FIND.Find
:p.The :hp4.Find:ehp4. command finds a specific element in a displayed list.  It is only available when a list box is active.  
:p.To use the Find command, perform the following steps:
:ul compact.
:li.Access the list box in which you wish to search.
:li.Perform the Utilities, Find menu choice sequence.
:li.CES will display the dialog box.
:li.Specify the parameters of the find, 
:li.Select OK, Cancel or Help to perform the desired function.
:li.Assume you wish to find a list of all the items with the word pipe in the short 
description.  Access the Standard Item Catalog and perform the Utilities, Find 
command sequence.  Choose short description as your data field, then type the 
phrase *PIPE* in the wildcard field.  This tells CES to find all items with the 
word pipe in the short description.  You can then specify whether CES finds 
matches earlier in the list than where you are now (previous matches) or 
later in the list (next match).  You can also choose to select all the items 
that match the search parameters.  These can then be printed for closer examination.
:eul.

  :h1 res=IDM_SORT.Sort
:p.The :hp4.Sort:ehp4. command sorts a list of elements in the currently active list window.  
:p.This command is only available when accessing a list window.  You then have 
the option to sort on many fields of a list.  For instance, you could sort 
project cost data by unit cost, by volume, or by extended cost.

:p.To use the Sort command, perform the following steps:
:ul compact.
:li.Access the list box you wish to sort.
:li.Perform the Utilities, Sort menu choice sequence.
:li.CES will display the dialog box.
:li.Specify the parameters of the search. 
:li.Select OK, Cancel, or Help to perform the desired function.
In the Sort shown above, the Standard Item Catalog will be sorted by standard 
item first, and then by code value, both in ascending.  This means that CES will 
put the items in numerical order, starting with the lowest and going up.  Within 
that list they will be grouped by code value in alphabetical order.  So, for 
example, the standard items measured in cubic yards (CY) would be listed before 
those that are measured as lump sums (LS). 
:eul.

  :h1 res=IDM_UPDATE_BASE_DATE.Update Base Date
:p.The :hp4.Update Base Date:ehp4. command attaches new reference data to all 
jobs.  Since projections of progress and interest rates are tied to this date, 
this can be an important change.
:p.To update the base date, perform the following steps:
Perform the Utilities, Update Base Date menu choice sequence.

  :h1 res=IDM_UPDATE_SPEC_YEAR.Update Spec Year
:p.The :hp4.Update Spec year:ehp4. command gives the System Manager information 
on errors and/or messages.
To update the spec year, perform the following steps:
Perform the Utilities, Update Spec Year menu choice sequence.

  :h1 res=IDM_SET_PREFERENCES.Set Preferences
:p.The :hp4.Set Preferences:ehp4. command allows you to set preferences specific 
to your current environment.  Examples of user preferences are History Size, 
Auto Window Maximizing, Request Notification on Background Jobs When Complete, 
and Request Verification on Deletes.  History Size  If you have a very large 
database that spans many years you may want to limit its size for some estimation 
jobs.  This will improve system performance.
Auto Window Maximizing  This option makes every window you open go immediately 
to full screen size.  This can be helpful if you are working with catalogs 
such as the Standard Item Catalog, which is very large. 
Request Notification on Background Jobs  This option allows you to leave a job 
and proceed on to another one.  The program will put a message box over any 
screen where you are currently working.  
Request Verification on Deletes  The default setting for this option is on.  
Thus, whenever you delete an item, you will be asked if you are sure you want 
to do this. If you hit enter without thinking, the item will not be deleted 
since the warning box opens with the No option highlighted. 
If you do not wish this warning to be displayed, you can use the Request 
Verification option to turn off this feature.
To use the Set Preferences command, perform the following steps:
Perform the Utilities, Set Preferences menu choice sequence.

  :h1 res=IDM_CHANGE_PASSWORD.Change Password
:p.The :hp4.Change Password:ehp4. is command allows you to Change your CES 
account password.
To change your password, perform the following steps:
Perform the Utilities, Change Password menu choice sequence. 

  :h1 res=IDM_SHOW_USERS.Show Users
:p.The :hp4.Show Users:ehp4. command lists the users currently connected to 
any PC module.  Show Users also provides an option to log people off the 
network. This command is available only to System Managers.

  :h1 res=IDM_SEND_MESSAGE.Send Message
:p.The :hp4.Send Message:ehp4. command allows the user to send a message to 
another PC user on the system. 
:p.To send a message to another user, perform the following steps:
:ul compact.
:li.Perform the Utilities, Send Message menu choice sequence.
:li.CES will display the dialog box.
:li.Enter the user to whom you wish to send a message, and type in the message. 
:li.Select OK, Cancel, or Help to perform the desired action.
:eul.

  :h1 res=IDM_INSTALL.Install
:p.The :hp4.Install:ehp4. command facilitates update installations.  The Deny 
Logon command could be used in this situation also.

  :h1 res=IDM_DENY_LOGON.Deny Logon
:p.The :hp4.Deny:ehp4. Logon command allows the System Manager to install 
updates or perform other maintenance on the system without other users logging 
on.  This command will be available only to System Managers.

  :h1 res=IDM_GENERATE_SUPPORT_REQUEST.Generate Support Request
:p.The :hp4.Generate Support Request:ehp4. command automatically generates a 
support request.  This is used for requesting information and for reporting 
suspected bugs in the system.  
:p.To generate a support request, perform the following steps:
:ul compact.
:li.Perform the Utilities, Generate Support Request menu choice sequence.
:li.CES will display the dialog box.
:li.Specifiy the type of request you are making by selecting from the first 
list box in the dialog box.  
:li.Specify the priority with which you think this should be dealt.
:li.Define the problem in the short and long description fields of the dialog.  
:li.Select OK to accept, help for clarification, or cancel to return to CES.  
:li.Pressing OK will automatically send the request to your printer.  Then you 
can either mail or fax the request to the support staff.
:eul.

.* end utilities options **

.************************
.* Window Menu Help    **
.************************

  :h1 res=IDM_WINDOW_MENU.WINDOW_MENU
:p.The :hp4.Window Menu:ehp4. displays a list of all the windows open.
This can be useful when there are many windows being displayed and the user 
wants to look at one that is buried.

.* Window Menu related options **


     :p. :link reftype=hd res=IDM_ARRANGE_CASCADED.Arrange Cascaded:elink.
:p.The Arrange Cascaded command arranges all of the windows in a cascaded fashion.


     :p. :link reftype=hd res=IDM_ARRANGE_TILED.Arrange Tiled:elink.
:p.The Arrange Tiled command arranges all the open windows in separate tiles on the screen.


.* end of Window menu related options **

.* Window  options **


  :h1 res=IDM_ARRANGE_CASCADED.Arrange Cascade
:p.The  :hp4.Arrange Cascade:ehp4. command arranges all of the windows in a 
cascaded fashion. The title bar of each window will be visible.
If the windows are not completely cascaded after the first cascade, perform the 
arrange command again. 

  :h1 res=IDM_ARRANGE_TILED.Arrange Tiled
:p.The :hp4.Arrange Tiled:ehp4. command arranges all the open windows in 
separate tiles on the screen.  This means all windows will be visible in their 
entirety, but will be shrunk to fit. This command can be used for operating in 
multiple windows.  For example, generating multiple reports is easy if the 
windows are tiled.  Simply open the windows necessary to run the desired 
reports, then select a report.  While it is processing, move to another window 
and select another report.  As stated in the File section, you can check the 
status of each report with the Process Status command.  

.* End Window  options **


.************************
.* Debug  Menu Help    **
.************************


/*  :h1 res=IDM_DEBUG_MENU.DEBUG_MENU
      :p.This is the help text for debug_menu.

  :h1 res=IDM_DIE_SCUM.DIE_SCUM
      :p.This is the help text for die_scum.

  :h1 res=IDM_DEBUG_BREAK.DEBUG_BREAK
      :p.This is the help text for debug_break.

  :h1 res=IDM_STACK_INFO.STACK_INFO
      :p.This is the help text for stack_info.

  :h1 res=IDM_STACK_CLEAR.STACK_CLEAR
      :p.This is the help text for stack_clear.	*/

.************************
.* Help   Menu Help    **
.************************

  :h1 res=IDM_HELPMENU.Help Menu
:p.The :hp4.Help Menu:ehp4. window is used to provide help about the BAMS/CES 
application.


  :h1 res=IDM_GENERALHELP.General Help
:p.You can obtain :hp4.Help:ehp4. at any time by pressing F1. When you request 
help, a help window is displayed the kind of help you get is determined by what 
you selected before you requested help.


   :h1 res=IDM_KEYBOARD.Keyboard
:p.The :hp4.Help Keyboard:ehp4. option explains the use of the keyboard with 
the CES application.  Most of the mouse-driven commands can be duplicated by the 
keyboard.



  :h1 res=IDM_MOUSE.Mouse
:p.The :hp4.Help Mouse:ehp4. option explains the use of  the mouse with this 
application.  This topic explains the concepts of clicking, pointing, dragging,
etc.  It will also demonstrate the specific steps required to reach a certain 
menu level.


  :h1 res=IDM_COMMANDS.Commands
:p.The :hp4.Commands Help:ehp4. option explains the use of commands with this 
application.  


  :h1 res=IDM_PROCEDURES.Procedures
:p.The :hp4.Help Procedures:ehp4. Option lists a few  procedures available in this 
application. Select a specific procedure to get further explanation about performing that
procedure.



  :h1 res=IDM_WINDOWS.Windows
:p.The :hp4.Help Windows:ehp4. option displays a list of windows available in 
CES.  You can then select the window you would like to learn more about.  
After you have read the material for the item you selected, you can either close 
the help window or press the ESC key to go back to the window list.  This is 
helpful if you have a certain function in mind but are not sure which window 
provides that function. You can simply use the trial and error process to find 
the window that gives the service you need.    


   :h1 res=IDM_GLOSSARY.Glossary
:p.The :hp4.Help Glossary:ehp4. option displays a list of  terms used by 
BAMS/CES and the help system.




.* Help Menu Menu related options **

     :p. :link reftype=hd res=HP_USING_HELP_HELP.General Help:elink.
:p.Help is always available. 


     :p. :link reftype=hd res=HP_KEYBOARD_HELP.Keyboard:elink.
:p.The Help Keyboard command explains the use of the keyboard with CES 
Application.

     :p. :link reftype=hd res=HP_MOUSE_HELP.Mouse:elink.
:p.The Help Mouse command explains the use of the Mouse with CES Application.

     :p. :link reftype=hd res=HP_COMMANDS_HELP.Commands:elink.
:p.The Help Commmands command explains the use of Commands with CES Application.

     :p. :link reftype=hd res=HP_PROCEDURES_HELP.Procedures:elink.
:p.The Help Procedures command list a few procedures available CES Application.

     :p. :link reftype=hd res=HP_WINDOWS_HELP.Windows:elink.
:p.The Help Windows command explains what Windows are available in CES 
Application.

     :p. :link reftype=hd res=HP_GLOSSARY_HELP.Glossary:elink.
:p.The Help Glossary command displays a list of terms used by BAMS/CES and the 
help system.

     :p. :link reftype=hd res=IDM_INDEX.Index:elink.
:p.The :hp4.Help Index:ehp4. command displays a list of available help topics. 
Scroll through the alphabetically arranged list to find the topic specific to 
the current problem. After you have read the material for the item you selected, 
you can either close the help window or press the ESC key to go back to the 
index list.  In this way you can use a trial and error process to find the 
information you need.   



     :p. :link reftype=hd res=IDM_ABOUT.About:elink.
:p.The Help About command activates the About box.



.* end of Help menu related options **

.* Help  options **  


  :h1 res=HP_USING_HELP_HELP.General Help
:p.You can obtain :hp4.Help:ehp4. at any time by pressing F1. When you request 
help, a help window is displayed the kind of help you get is determined by what 
you selected before you requested help.
 
   :h1 res=HP_KEYBOARD_HELP.Keyboard
:p.The :hp4.Help Keyboard:ehp4. option explains the use of the keyboard with 
the CES application.  Most of the mouse-driven commands can be duplicated by the 
keyboard.
:p.To select one item, use the arrow key.  The program will select one item at 
a time, unselecting the one previous.  In other words, you can scroll through 
the list, until you reach the desired item.  If you stop on that item, it will 
be selected.  The space bar will toggle any item from selected to unselected or 
vice-versa.  The page, end, and home keys all work with this program.  The 
page down key will move the selection bar to the very bottom of the screen, 
selecting that bottom item or the one that starts the next screen.  This is 
dependant on the amount of the bottom line showing.  This key is sensitive
to the size of the window. For example, if the window is showing 11 items, it 
will go to the 12th item and select it.  If the screen is showing 24, it will 
select the 24th or 25th.  The page up key works in similar fashion, selecting 
the top item in the current window. The end key will select the last item in the 
list, while the home key will select the first (top) item in the list.  
The shift key used with the page down key will select from the start point to 
the bottom of the screen.  The shift key with the page up key will select from 
the start point to the top of the screen.  The shift and arrow key combination 
will drag the selection bar in the direction of the arrow.  This will select 
items as it goes.  The control and arrow key combination will move the selection 
bar to a new location.  The user can then select more text with the shift and 
arrow combination.  The arrow key will select one item and will unselect any 
items currently selected. 


  :h1 res=HP_MOUSE_HELP.Mouse
:p.The :hp4.Help Mouse:ehp4. option explains the use of  the mouse with this 
application.  This topic explains the concepts of clicking, pointing, dragging,
etc.  It will also demonstrate the specific steps required to reach a certain 
menu level.
:p.To select only one item, simply click the left mouse button when the cursor 
arrow is at the desired item.  To select multiple items, click and hold the left 
button down while "dragging" the mouse.  This will select in either direction 
and,as long as you hold the mouse button down, you can unselect those you just 
selected if you overshoot the mark. The "Shift and Click" method combines the 
keyboard shift key with the mouse.  Hold the shift key down when you click on 
the item.  This will select all the items from the closest selected one to the 
cursor arrow.  This is similar to blocking text in a word processing program.  
The "Control and Click" method also uses the keyboard.  This sequence is used 
when the items you wish to block are not all together. For instance, in the 
Standard Item Catalog, the list is longer than will fit on one screen.  (An 
example of this type of a situation is printing a report or listing.  You may 
want to print only the standard items for this project rather than the whole 
Standard Item Catalog.)  Assume you wish to select the first eight, then skip 
four, then select seven more. Select the first block by clicking and dragging, 
or highlight the first one, go to the seventh and use the Shift and Click 
combination.  Then use the Control key to move the cursor over the four 
undesired items.  When the cursor arrow is at the desired items click to begin 
selecting.  Although the cursor will move without holding the control key down, 
anything selected without the control key will select that item and unselect all
the other items.  The Space Bar will act as a toggle switch for selected and 
unselected items.  It will select an unselected item and unselect a selected 
one. To unselect all the items, simply click on a single item.  The program will 
select that one in favor of all the others.  This will leave one item selected 
and it can be unselected with the space bar.  
:p.CES also has a split screen feature that is useful for veiwing
opposite ends of a very long or wide list.  The picture below
shows the Standard Item Catalog split horizontally.  This allows
you see the top and the bottom of the list at the same time.  The
two halves scroll independently of each other, and you can select
from both halves of the list.  
:p.The :hp4.Split Screen Feature:ehp4.
:p.To split a window, move the cursor arrow to the top or bottom of
the scroll bar, until the arrow changes to two parallel bars. 
Then click and drag the bar to position it.  You can position it
anywhere on the screen.  Once positioned, you will have two
halves of the screen that scroll independently of each other.  To
get rid of the bar, either double click on the divider bar or
move it to the edge of the screen.  The screen will return to
normal viewing mode.  
:p.Some windows, such as the Standard Item Catalog, have horizontal
scroll bars as well as a vertical one.   For windows such as
these, you can put a horizontal divider, a vertical divider, or
both in the window.  You put the vertical bar in the window the
same way you put in the horizontal bar.  
:p.The :hp4.4-Way Split Screen Feature:ehp4.
:p.To install the four-way divider, move the cursor arrow to the
bottom right-hand corner of the screen until it becomes a square
with four arrows.  Click and drag the icon to position the
divider bar.  To scroll through the lists, use the scroll bars as
normal.  The right-side scroll bars, in the top half, scroll data
to the left of the bar for the top half.  Similarly, the bottom
left scroll bar, scrolls the data above it, over to the vertical
divider.  
To get rid of the entire divider, double click on it.  To get rid
of one but leave the other divider, move the unwanted divider to
an edge of the window by clicking and dragging.  When the divider
is at the edge of the window, release the mouse button.  The
screen will be left with only one divider. 

  :h1 res=HP_COMMANDS_HELP.Commands
:p.The :hp4.Commands Help:ehp4. option explains the use of commands with this 
application.  
:p.There are two types of commands in CES--window-dependent and window-
independent.  Window-dependent commands yield different results, based on the 
window active when they are used.  For example, the View command can show a job 
item list, a job item breakdown, a job alternate group list, or any of several 
others,  depending on the window that was active when this command was executed.
Window-independent commands can be accessed from anywhere and will always 
provide you with the same results.  The File command is an example of this type 
of command.   It allows you to perform file-related functions such as printing, 
saving, accessing catalogs, etc., even if you are in the middle of some other 
operation or are several sub menus deep.  A very useful result of this is the 
following:  If you are trying to fill in a dialog box and do not know a code for 
an item or do not know the actual item number, you can leave the dialog box for 
a moment and execute the File command.  Since the File command is the path taken 
to all the catalogs provided by CES, you can select a catalog that will have the 
information you need (in this case, the Code Table Catalog or the Standard Item 
or Major Item Listing Catalog) and then copy that information into the dialog 
box.  This is much easier than "backing out" of all the menus you just went 
through, finding the information, then returning through that same path to get 
back to that same dialog box.  


  :h1 res=HP_PROCEDURES_HELP.Procedures
:p.The :hp4.Help Procedures:ehp4. Option lists a few  procedures available in this 
application. Select a specific procedure to get further explanation about performing that 
procedure.



.*********************Procedures For CES **********************



:sl compact.
 
          :li.:link res=200001 reftype=hd.General Structure of CES:elink.  
          :li.:link res=200002 reftype=hd.Types of Commands:elink.  
          :li.:link res=200003 reftype=hd.CES Colors:elink.  
          :li.:link res=200004 reftype=hd.Important Default Settings:elink.  
          :li.:link res=200005 reftype=hd.Running CES:elink.  
          :li.:link res=200006 reftype=hd.Password and Log-on:elink.  
          :li.:link res=200007 reftype=hd.Activating and Closing Windows:elink.  
          :li.:link res=200008 reftype=hd.Creating New Programs or Jobs:elink.  
          :li.:link res=200009 reftype=hd.Saving and Exiting:elink.  
          :li.:link res=200010 reftype=hd.The Big Picture:elink.  
          :li.:hp4.Using the different cost estimation methodologies:ehp4.  
          :li.:link res=200011 reftype=hd.Default Unit Prices:elink.  
          :li.:link res=200012 reftype=hd.Cost-Based Estimation:elink.  
          :li.:link res=200013 reftype=hd.PEMETH Averages:elink.  
          :li.:link res=200014 reftype=hd.PEMETH Regression:elink.  
          :li.:link res=200015 reftype=hd.Parametric Estimation:elink.  
          :li.:link res=200016 reftype=hd.Scheduling Job items:elink.  
          :li.:link res=200017 reftype=hd. Entering Job Comments:elink.  
          :li.:link res=200018 reftype=hd.Adding job alternates:elink.  
          :li.:link res=200019 reftype=hd.Running Browsers:elink.  

:esl.






  :h1 res=HP_WINDOWS_HELP.Windows
.************************************************************
.******** Windows Available in CES **************************
.************************************************************ 

:sl compact.
     :li.:link reftype=hd res=AreaTypeCatS.:lm margin=10.Area Type Catalog:elink.  
     :li.:link reftype=hd res=AreaTypeS.Area Window:elink.
     :li.:link reftype=hd res=AreaTypeCatD.Add/Change Area Type :elink.
     :li.:link reftype=hd res=AreaTypeD1.Add/Change Area :elink.
.*     :li.:link reftype=hd res=AreaTypeD2.areatyped2:elink.
.*     :li.:link reftype=hd res=CodeTableD.Add/Change Code Table :elink.
.*     :li.:link reftype=hd res=CodeValueD.Add/Change Code Value :elink.
     :li.:link reftype=hd res=CountyD.Add/Change County:elink.
     :li.:link reftype=hd res=CostSheetD.Add/Change Cost Sheet :elink.
     :li.:link reftype=hd res=CrewD.Add/Change Crew :elink.
.*     :li.:link reftype=hd res=CrewEquipD.Add/Change Crew Equipment:elink.
.*     :li.:link reftype=hd res=CrewLaborD.Add/Change Crew Labor:elink.
     :li.:link reftype=hd res=DefltPricesD.Add/Change Default Unit Prices:elink.
     :li.:link reftype=hd res=EquipmentD.Add/Change Equipment :elink.
.*     :li.:link reftype=hd res=EquipRateD.Add/Change Equipment Rate :elink.
.*     :li.:link reftype=hd res=FacilityD.Add/Change Facility :elink.
     :li.:link reftype=hd res=FundD.Add/Change Fund :elink.
     :li.:link reftype=hd res=InflationPredictionD.Add/Change Inflation Prediction :elink.
     :li.:link reftype=hd res=JobAltD.Add/Change Job Alternate :elink.
     :li.:link reftype=hd res=JobAltGroupD.Add/Change Job Alternate Group :elink.
     :li.:link reftype=hd res=JobBreakdownD.Add/Change Job Breakdown:elink.
     :li.:link reftype=hd res=JobBreakdownProfileE.Add/Change Job Breakdown Profile :elink.
     :li.:link reftype=hd res=AddProgJobD.Add/Change Jobs for Programs:elink.
     :li.:link reftype=hd res=JobCommentD.Add/Change Job Comment :elink.
     :li.:link reftype=hd res=LaborD.Add/Change Labor :elink.
     :li.:link reftype=hd res=LabRateD.Add/Change Labor Rate :elink.
     :li.:link reftype=hd res=MajorItemD.Add/Change Major Item :elink.
     :li.:link reftype=hd res=MapD.Add/Change Map :elink.
     :li.:link reftype=hd res=MaterialD.Add/Change Material:elink.
     :li.:link reftype=hd res=MatPricesD.Add/Change Material Price:elink.
     :li.:link reftype=hd res=ParametricProfileD.Add/Change Parametric Profile :elink.
.*     :li.:link reftype=hd res=ProgramD.Add/Change Program Window:elink.
     :li.:link reftype=hd res=AddProgUserD.Add Program User:elink.
     :li.:link reftype=hd res=ChProgUserD.Change Program User
     :li.:link reftype=hd res=SignifMajItemD.Add/Change Significant Major Item:elink.
     :li.:link reftype=hd res=StanItemD.Add/Change Standard Item:elink.
.*     :li.:link reftype=hd res=UserD.Add/Change User :elink.
     :li.:link reftype=hd res=VenEditD.Add Vendor Window:elink.
     :li.:link reftype=hd res=VendorD.Add/Change Vendor :elink.
.*     :li.:link reftype=hd res=VendorAddressD.Add/Change Vendor Address :elink.
.*     :li.:link reftype=hd res=WorkClassD.Add/Change Work Class :elink.
     :li.:link reftype=hd res=WorkDayD.Add/Change Working Day Calendar:elink.
     :li.:link reftype=hd res=WorkDayInfoD.Add/Change Working Day Calendar Info :elink.
     :li.:link reftype=hd res=ZoneD.Add/Change Zone:elink.
.*     :li.:link reftype=hd res=ZoneMapD.Add/Change Zone Maps:elink.
     :li.:link reftype=hd res=ZoneTypeD.Add/Change Zone Type:elink.
     :li.:link reftype=hd res=CategoryS.Category Window:elink.
     :li.:link reftype=hd res=CrewLbEqD.Change Crew:elink.
     :li.:link reftype=hd res=JobD.Change Job :elink.
     :li.:link reftype=hd res=ChJobBreakFundD.Change Job Breakdown Profile Window:elink.
.*     :li.:link reftype=hd res=CodeTableS.Code Table Catalog:elink.
.*     :li.:link reftype=hd res=CodeValueS.Code Value Window:elink.
     :li.:link reftype=hd res=CostSheetCatS.Cost Sheet Catalog:elink.
     :li.:link reftype=hd res=CostSheetS.Cost Sheet Window:elink.
     :li.:link reftype=hd res=CountyCatS.County Catalog:elink.
     :li.:link reftype=hd res=CrewCatS.Crew Catalog:elink.
     :li.:link reftype=hd res=CrewS.Crew Window:elink.
     :li.:link reftype=hd res=DefaultUnitPriceCatS.Default Unit Price Catalog:elink.
     :li.:link reftype=hd res=DefaultUnitPriceS.Default Unit Price Window:elink.
.*     :li.:link reftype=hd res=DefaultUserS.Default User:elink.
     :li.:link reftype=hd res=EquipmentCatS. Equipment Catalog:elink.
     :li.:link reftype=hd res=EquipmentS. Equipment Window:elink.
.*     :li.:link reftype=hd res=FacilityS.Facility:elink.
.*     :li.:link reftype=hd res=FacilityBrowserResultsS.Facility Browser Results:elink.
.*     :li.:link reftype=hd res=FacilityBrowserD.facilitybrowserd:elink.
     :li.:link reftype=hd res=FacilityCatS.Facility Catalog:elink.
.*     :li.:link reftype=hd res=FacilityForMaterialS.Facility For Material:elink.
.*     :li.:link reftype=hd res=FacilityForVendorS.Facility For Vendor:elink.
     :li.:link reftype=hd res=FundCatS.Fund Catalog:elink.
     :li.:link reftype=hd res=ProgramFundS.Program Funds Window:elink.
     :li.:link reftype=hd res=InflationPredictionCatS.Inflation Prediction Catalog:elink.
     :li.:link reftype=hd res=InflationPredictionS.Inflation Prediction Window:elink.
.*     :li.:link reftype=hd res=ItemBrowserResultsS.Item Browser Results:elink.
     :li.:link reftype=hd res=JobAlternateGroupS.Job Alternate Group Window:elink.
     :li.:link reftype=hd res=JobAlternateGroupListS.Job Alternate Group List Window:elink.
     :li.:link reftype=hd res=JobBreakdownFundS.Job Breakdown Funds Window:elink.
     :li.:link reftype=hd res=JobBreakdownS.Job Breakdown Window:elink.
     :li.:link reftype=hd res=JobBreakdownListS.Job Breakdown List:elink.
     :li.:link reftype=hd res=JobBreakdownProfileS.Job Breakdown Profile Window:elink.
     :li.:link reftype=hd res=JobBreakdownMajorItemS.Job Breakdown Major Item:elink.
     :li.:link reftype=hd res=JobCommentS.Job Comment Window:elink.
     :li.:link reftype=hd res=JobFundS.Job Fund Window:elink.
     :li.:link reftype=hd res=JobItemsS.Job Items Window:elink.
     :li.:link reftype=hd res=JobItemScheduleS.Job Item Schedule:elink.
     :li.:link reftype=hd res=JobItemS.Job Item Window:elink.
     :li.:link reftype=hd res=JobItemD.Add Job Item Window:elink.
     :li.:link reftype=hd res=JobMajItemPriceRegrS.Job Major Item Price Regression:elink.
     :li.:link reftype=hd res=JobMajItemQuantityRegrS.Job Major Item Quantity Regression Window:elink.
     :li.:link reftype=hd res=JobPemethAvgS.PEMETH Averages WIndow:elink.
     :li.:link reftype=hd res=ProgramJobS.Program Job List Window:elink.
     :li.:link reftype=hd res=JobUserS.Job User Window:elink.
     :li.:link reftype=hd res=JobS.Job Window:elink.
     :li.:link reftype=hd res=LaborCatS.Labor Catalog:elink.
     :li.:link reftype=hd res=LaborerS.Labor Wage List Window:elink.
     :li.:link reftype=hd res=MajorItemCatS.Major Item Catalog:elink.
     :li.:link reftype=hd res=MajorItemS.Major Item Window:elink.
     :li.:link reftype=hd res=MajItemPriceRegrS.Major Item Price Regression Window:elink.
     :li.:link reftype=hd res=MajItemQuantityRegrS.Major Item Quantity Regression Window :elink. 
     :li.:link reftype=hd res=MapCatS.Map Catalog:elink.
     :li.:link reftype=hd res=MaterialCatS.Material Catalog:elink.
     :li.:link reftype=hd res=MaterialPriceS.Material Price List Window:elink.
     :li.:link reftype=hd res=ProgramS.Program Window:elink.
     :li.:link reftype=hd res=OpenJobD.Open Job Window:elink.
     :li.:link reftype=hd res=OpenProgramD.Open Program Window:elink.
     :li.:link reftype=hd res=OpenProposalD.Open Proposal Window:elink.
     :li.:link reftype=hd res=OpenProjectD.Open Project Window:elink.
     :li.:link reftype=hd res=ParametricProfileCatS.Parametric Profile Catalog:elink.
     :li.:link reftype=hd res=PredecessorsD.Add Predecessors Job Item:elink.
     :li.:link reftype=hd res=ProgramUserS.Program User List Window:elink.
     :li.:link reftype=hd res=ProjectS.Project Window:elink.
     :li.:link reftype=hd res=ProjectItemS.Project Item Window:elink.
     :li.:link reftype=hd res=ProposalS.Proposal Window:elink.
.*     :li.:link reftype=hd res=PropBrowserResultsS.Proposal Browser Results:elink.
     :li.:link reftype=hd res=ProposalMajItemS.Proposal Major Item Window:elink.
     :li.:link reftype=hd res=ProposalItemS.Proposal Item Window:elink.
.*     :li.:link reftype=hd res=PropBrowserD.propbrowserd:elink.
     :li.:link reftype=hd res=SignifMajItemS.Significant Major Item:elink.
     :li.:link reftype=hd res=StandardItemCatS.Standard Item Catalog:elink.
     :li.:link reftype=hd res=StandardItemS.Standard Item Window:elink.
.*     :li.:link reftype=hd res=SuccessorsD.successorseditdlg:elink.
.*     :li.:link reftype=hd res=StdItemMajItemD.stditemmajitemeditdlg:elink.
.*     :li.:link reftype=hd res=UserCatS.User Catalog:elink.
     :li.:link reftype=hd res=VendorCatS.Vendor Catalog:elink.
     :li.:link reftype=hd res=VendorS.Vendor Window:elink.
     :li.:link reftype=hd res=WorkingDayCalendarCatS.Working Day Catalog:elink.
     :li.:link reftype=hd res=WorkingDayCalendarS.Working Day Calendar Window:elink.
     :li.:link reftype=hd res=ZoneTypeCatS.Zone Type Catalog:elink.
     :li.:link reftype=hd res=ZonesS.Zone Window:elink.

:esl.



   :h1 res=HP_GLOSSARY_HELP.Glossary
:p.The :hp4.Help Glossary:ehp4. option displays a list of  terms used by 
BAMS/CES and the help system.

:p.:hp8.Active Window.:ehp8.   A window is active when work can be performed in 
that window.  The program distinguishes the active window from inactive ones by 
illuminating the title bar more brightly.  (If you have a monochrome monitor, 
the title bar will be a darker color.)  With a color monitor, you are allowed to 
choose the color of this bar and most other parts of the screen.  To make a 
window active, simply click on it with a mouse or select it with the keyboard.  
(For detailed instructions on mouse and keyboard use, see the Tutorial chapter.)   
:p.:hp8.Area Type.:ehp8.   Just as a state may be divided into various types of
districts (e.g., congressional districts, election districts, water management 
districts), for purposes of roadway construction and maintenance a state may be 
divided into various types of areas according to terrain, market areas, distance 
from supply centers, urban vs. rural density, or other factors that may have an 
effect on pricing.  All areas are collections of one or more counties.   
:p.:hp8.Area:ehp8.   A subdivision of the state, consisting of one or more 
counties.  
:p.:hp8.Base Date:ehp8.   A base date is a date on which pricing information is 
fixed.  It is used to ensure that all pricing information within a job is 
correct for the time period encompassed by the job.  
Most of the data elements in the data base design are application
oriented; that is they reprsent some information required to perform
the tasks of cost etimation, whether or not a computer is used.  One
data element that is an important exception to that rule is the BaseDate,
which appears in many tables that contain dollar values that change over
time.  The reason for having BaseDates is relativly simple; if there were
only a single data row containing the wages for a particular kind of
laborer, updating that row?s values would changes the cost estimates for
every job using that kind of laborer.  

:p.:hp8.Browser:ehp8.   A browser is a built-in search utility that allows you 
to look for a specific item and see its related information.  This is accessed 
though the File, Run command sequence. When you select a browser, CES displays 
a dialog box. After the parameters of the search have been entered, click on OK 
or hit the enter key to execute the command.  (See Chapter 4, under the File 
menu section,  for detailed instructions in executing a browser.)
:p.:hp8.Clipboard:ehp8.   A clipboard is a space in random access memory (RAM)
reserved for particular information.  In the CES context, it refers to the space 
reserved for information that is cut or copied from one place to be inserted 
elsewhere.  This allows you to cut data from one area of a file but retain that 
data in RAM.  Then, this data can be pasted (inserted) back to the same location 
or to a different one, or even to a different program. The program will maintain 
this data in the clipboard until it is replaced by some other data.  If you exit 
the program, you will be asked if you wish to save the information in the 
clipboard.  If you save the document, the data will be saved in the clipboard.
:p.:hp8.Commands:ehp8.  The commands to perform various functions usually have 
a line under one letter of the command.  This is to assist the keyboard user in 
choosing the command.  The underlined letter in combination with the ALT key 
will enact that command. If you are in a dialog box or are selecting preferences 
for something like a print job, you only need type the letter underlined on the 
screen.  There is no need to use the ALT key.
:p.:hp8.Cost Sheet:ehp8.   A cost sheet is used to tally the costs of the 
crew(s) and material(s) needed to complete a task (a part of an item of work).  
Cost sheets are applicable under Cost-Based Estimation only.  When a cost sheet 
is used to estimate the cost of an item in a job, the cost sheet is thereafter 
referred to as a job cost sheet.    
:p.:hp8.Cost-Based Estimation:ehp8.  This is one of five installable estimation 
methods used by BAMS/CES.  It estimates project cost by determining labor, 
equipment, and material costs and summing over the number of projects and over 
time.  For further information on estimation methods used by the program, see 
Chapter 2, Cost Estimation Methodologies.  
:p.:hp8.Crew:ehp8.   A crew is a logical grouping of laborers and equipment 
used to perform an item of work.  The predefined crews that can be used on cost 
sheets are applicable under Cost-Based Estimation only.  
:p.:hp8.Davis-Bacon Wages:ehp8.   Davis-Bacon wages are waces paid to specified 
types of laborers when working on federally funded jobs.  Non Davis-Bacon wages 
are those wages paid to workers on state-funded jobs. 
:p.:hp8.Default:ehp8.   In computer terminology, a default value or setting is 
one chosen automatically by the program unless some other value is specified.  
For instance, the colors on the screen will be in a pre-set (default) 
combination unless they are changed by the user.  The BAMS/CES program has many 
default settings, which represent the most common positions for those settings. 
:p.:hp8.Default Unit Price:ehp8.  For pricing purposes, CES maintains a default
price for each standard item.  The program will use these default unit prices 
when no more rigorous method of estimation is available.  For example, regression, 
averages, and cost-based estimation need many occurrences of an item for their 
prices, which are essentially averages, before they are reliable.  If an item 
exists that does not have a sufficient history to support one of these methods, 
the program will use the unit prices "by default." However, since the default 
unit price does not take item quantity or other job-specific parameters into 
account, it may or may not accurately reflect the actual cost of a standard item 
within the context of a particular job.  For instance, one contractor could be 
working with much larger quantities (leading to lower prices) than another, or 
the area of the state in which he works could have much more inclement weather 
than the one where the original item price was recorded.  For this reason,the 
other estimation methods available to the estimator will usually provide a more 
accurate estimated unit price.  Nonetheless, each default unit price is usually 
gleaned from past experience, and the estimator may wish to compare a unit price 
produced via an alternative estimation methodology against the default unit price 
to see whether the two figures are "in the ballpark" or if one may be in error.  
:p.:hp8.Dependency Link:ehp8.  Dependency links are stated in terms of A, then 
B.  A start to finish link means that A must start before B can finish.  The 
most common type of dependency link is finish-to-start, therefore, A  must 
finish before B can start.  (See Item Dependency for further details.)
:p.:hp8.Dialog Box:ehp8.   A dialog box will be displayed when the program 
needs information from the user to complete a task.  For example, the initial 
log-on box that requests name, date, user authorization, etc. is a dialog box.  
Dialog boxes will appear when you change the default settings, generate a report,
open a new job or program, or perform any of the Edit commands.  You can add 
more changes by selecting the Add More command.  Use the tab key to  move 
through the dialog box.  The ENTER key will accept the changes already made and 
return you to the document.  Cancel changes by selecting the cancel button
or hitting the escape button on the keyboard. Accept by selecting the OK button 
or hitting the ENTER key on the keyboard.  
:p.:hp8.Equipment:ehp8.   Equipment is any mechanical tool or contrivance that 
must be bought, rented, or leased.  
:p.:hp8.Fund:ehp8.   A fund is a source of revenue that can be used to pay for 
all or part of a job.  
:p.:hp8.Item Dependency:ehp8. For scheduling purposes, a separate reference 
table in the database maintains information on item dependency.  If the start 
or finish of work on any Job Item (A) must precede, succeed, or coincide with 
the start or finish of work on another job item (B), job item (A) is dependent 
on job item (B).  For instance, a roadbed must be laid down before paving can 
begin.  Thus, asphalt or concrete are dependent on the gravel (or whatever 
material is) used for the roadbed.  The roadbed material is a predecessor to the 
paving material. The database also maintains information about lag time (the 
number of days that must elapse between the dependent items) and dependency 
(start to finish, etch).
:p.:hp8.Item:ehp8.  See Standard Item.
:p.:hp8.Job Alternate:ehp8.   A job alternate is one of several ways of 
performing a job.  For example, a bridge could have steel, concrete, or wood 
construction.  These are the job alternates.
:p.:hp8.Job Alternate Group:ehp8.  A job alternate group is a type of job for 
which there are alternate methods of construction.  If three job alternates for 
building a bridge are steel, concrete, or wood construction, the job alternate 
group would be the bridge.  
:p.:hp8.Job Breakdown:ehp8.   A Job Breakdown divides a job into its individual 
Job Items.  This method facilitates costing a project by allowing the estimator 
to work with more conveniently sized sections.  This also allows the estimator 
to develop cost estimate subtotals.   
:p.:hp8.Job Item:ehp8.  A standard item included in a job is referred to as a 
job item.   
:p.:hp8.Job:ehp8.   A job is a group of related work items.  A job may also be 
referred to as a project.  
:p.:hp8.Labor:ehp8.  Labor is work performed by people.  CES makes distinctions 
between federally funded (Davis-Bacon) and state-funded (non Davis-Bacon) 
employees, as well as between various types of workers. 
:p.:hp8.List Box:ehp8.   A list box (combination box) is a list of all the 
choices available for this particular action.  For example, the list of files 
from which you choose to open a file is a dialog box.  These are distinguished 
by a down pointing arrow on the right side of the box.  Whenever the program 
provides more than one choice with respect to an issue, it will provide the list 
in a list box.  Select a choice by cursoring down with the highlighting bar or 
by clicking on the item with the left mouse button.  
:p.:hp8.Listing:ehp8.   A listing is a printout of the contents of a single 
catalog or database table.  
:p.:hp8.Major Items:ehp8.  A major item can be any significant cost component 
of a job.  It is a non-biddable item.  A major item can consist of one 
individual standard item (e.g., excavation),  a material  that appears as a
component in numerous standard items (e.g., concrete or steel),  or a component 
of a structure which consists of one or more standard items.   Superstructures, 
traffic maintenance, earthwork, and signaling are all examples of major items.  
:p.:hp8.Material:ehp8.   A material is any quantifiable physical entity which 
is consumed in the performance of an item of work (excluding power or fuel for 
equipment).  
:p.:hp8.Parametric Estimation:ehp8.   Parametric estimation is a broad term for 
cost estimation methods that estimate costs of a project or job based on 
estimates of parameters involved in that job.  A parameter could be quantities 
of materials or items needed or prices of major items.  Parametric estimation 
uses three statistical modeling techniques:  Major Item Price Estimation, Major 
Item Quantity Estimation, and Job Breakdown Estimation.  
Job Breakdown estimation uses major item quantities and major item prices to 
estimate the total cost of a job breakdown.  The total cost accounted for by the 
major items is adjusted upward to account for missing major items and costs that 
do not fit into a major item classification. 
Major Item Price estimation uses statistical regression to determine the 
relationship between major item price and total quantity.  It can also take into 
account one or more qualitative factors such as work type, area within a state, 
and urban/rural classification. 
Major Item Quantity estimation is used to determine approximate quantities of 
the most significant major items for a job breakdown, using only job and job 
breakdown characteristics. 
The coarsest parametric estimate is one in which BAMS/CES Parametric Estimation 
Methods have been used to estimate major item quantities and major item prices 
and to adjust for missing major items.  The estimate can be refined by correcting
the major item quantities when they become known, adding quantities for other, 
less significant major items, and overriding or adjusting the major item unit 
prices produced by the system.  See Chapter 2 for additional details.  
:p.:hp8.Parent/Child Concept:ehp8.   In a Windows environment, a parent window 
is a window that has additional (more specific) data that can be accessed in an 
attached window.  In CES, child windows can be viewed by double clicking on blue 
numbers, choosing the view command and selecting one of the choices in the view 
list, or by clicking on an item in a catalog.  For example, a cost sheet catalog 
is a parent window.  A specific cost sheet that was listed in that catalog is a 
child of the catalog parent.  You would access the particular cost sheet by 
double clicking on that cost sheet.  
:p.:hp8.Parenthetical Information in the Descriptions:ehp8.   In the Reports 
chapter (Chapter 6), there are explanations that are in parentheses.  These are 
labels  that are within the main text of the report or listing, rather than 
titles in the report.  
:p.:hp8.PEMETH Averages Pricing:ehp8.   PEMETH Averages pricing is a BAMS/CES 
installable cost estimation method.  Based on item prices from previous jobs, 
average prices are computed for each item.  When sufficient historical 
occurrences of an item are available, the average prices are separated by area, 
work type, and item quantity.  See Chapter 2, PEMETH Averages, for additional 
details.  
:p.:hp8.PEMETH Regression Pricing:ehp8.   PEMETH Regression pricing is a 
BAMS/CES installable cost estimation method.  Based on item prices from previous 
jobs, regression formulas are used to compute price vs. quantity curves for each 
item.  Sufficient historical occurrences of an item are required for this 
estimation methodology to produce statistically valid results.  
The difference between regression and average pricing is that the regression 
results are continuous.  Average pricing makes price ranges into which items are 
grouped by certain characteristics.  Items very similar to each other can end up 
with very different prices based on their position relative to the cutoff.  
Therefore, regression prices are usually more accurate, and average prices are 
only used when there is an insufficient data history for the regression analysis. 
:p.:hp8.Program Fund:ehp8.  A fund that is available to pay for jobs in a 
particular program.  
:p.:hp8.Program Job:ehp8.  A job that is part of a program.  
:p.:hp8.Program:ehp8.   A program is a collection of jobs that are connected 
by time and/or funding. 
:p.Programs, Jobs, Tasks Programs are at the administrative level--they are 
combinations of jobs, but there is little actual involvement with the actual 
estimation process at this level.   Jobs are where the data is entered and used 
to develop the estimates.  Jobs are broken into tasks.
:p.:hp8.Report:ehp8.  A report is a printed display of some portion of the 
project.  Examples include a Fund Listing report, a Job Estimate report, and a 
Cost Sheet Catalog listing report.  (See Chapter 6 for detailed descriptions of 
each report produced by the program and Chapter 4 for detailed descriptions of 
how to generate and customize reports.) 
:p.:hp8.Selected Item:ehp8.   Selecting an item means highlighting the item 
either by cursoring the highlighting box to the desired item or by clicking on 
that item with the left mouse button.  If a selected item has additional 
information supporting it, such as an attached cost sheet, double clicking on 
the left mouse button will bring up that information. 
:p.:hp8.Significant Major Item:ehp8.   Significant major items are those major 
items that are most significant in estimating a job's cost.  
:p.:hp8.Spec Year:ehp8.   The spec year is a base year used for pricing 
purposes.  Its purpose is similar in nature to the base date above.  
:p.:hp8.Standard Item:ehp8.   Standard items are separately identifiable items 
of work such as tree clearing, tunnel excavation, and structure adjustment or 
repair.  These are stored in a Standard Item Catalog for easy reference and 
access by the estimator and are biddable items.  
:p.:hp8.Structure:ehp8.   A structure is a building or bridge or other non-road 
construction item that is built during and because of the construction program 
or job.
:p.:hp8.Task:ehp8.    A job item can be subdivided into its component tasks.  
The cost of each task is then estimated using the cost sheet for that task, 
which details the labor crew(s), equipment, and material(s) necessary to 
complete the task. This term is only applicable when using cost-based estimation 
to estimate the cost of a job item. 
:p.:hp8.Work Type:ehp8.   All proposals and projects tracked in the BAMS 
production modules and all jobs to be estimated in BAMS/CES can be classified 
according to the primary type of work to be performed.  The  type of work 
(project or job) often has an impact on item pricing, and many of the cost 
estimation methodologies available in BAMS/CES automatically take this into 
account when estimating the cost of a job.  
:p.:hp8.Working Day Calendar:ehp8.   The working day calendar in the BAMS/CES 
database contains information used to determine the number of working days 
available in a given calendar month.  In other words, this is an effort to 
predict all the days when construction will not proceed.  This aids in 
scheduling, since unexpected off days cause time overruns and increase costs.   
:p.:hp8.Working Day:ehp8.  A working day is any normal business day not 
suspended by a holiday.   Time-related values such as productivity (units of an 
item produced), job and job item starting dates, and job duration are usually 
measured in working days.        
:p.:hp8.Zone:ehp8.   A zone is an artificial separation of the state for 
material and labor pricing purposes.  Zones can be drawn without regard for 
county or other pre-existing logical subdivisions of a state.  
:p.:hp8.Zone Type:ehp8.   Just as a state may be divided into various types of 
districts (e.g., congressional districts, election districts, water management 
districts), for purposes of roadway construction and maintenance a state may be 
divided into various types of zones.  Zones can be drawn without regard for 
county lines or other pre-existing logical subdivisions of a state.    

 :h1 res=IDM_INDEX.Index
:p.The :hp4.Help Index:ehp4. command displays a list of available help topics. 
Scroll through the alphabetically arranged list to find the topic specific to 
the current problem. After you have read the material for the item you selected, 
you can either close the help window or press the ESC key to go back to the 
index list.  In this way you can use a trial and error process to find the 
information you need.   

 :h1 res=IDM_ABOUT.About
:p.The :hp4.About:ehp4. command  displays the CES version number and the address 
and fax number of AASHTO.  


.* End Help  options ** 

.****** Windows Follow ************************************


  :h1 res=ApplicationS.Application Catalog
:p.This is the help text for Application Catalog more to come.

  :h1 res=AreaTypeCatS.Area Type Catalog
:p.An area type is a name given to a group of areas that collectively cover the 
state. All areas have an area type.
:p.To open the :hp4.Area Type Catalog:ehp4. window, select the following menu choices and 
options:
:ul compact.
:li.File 
:li.Open Catalog 
:li.Area Type Catalog 
:eul.
:p.The :hp4.Area Type Catalog:ehp4. displays the following data fields:
:ul compact.
:li.:hp4.Area Type.:ehp4.   The Area Type field displays the name of this type of area, for 
example, Asphalt Market.
:li.:hp4.Description.:ehp4   The Description field describes this type of area and/or its 
usage.  
:eul.
 
  :h1 res=AreaTypeS.Area Window
:p.The :hp4.Area window:ehp4. displays all area breakdowns in the state for a given area 
type.  Double clicking on any area will display the list of counties in that area. 
An area is a name given to a group of counties for estimation purposes.
For example, if the area type is asphalt, the Area and Description columns in the 
window will show area-dependant variations in the price of asphalt. 

:p.To open the :hp4.Area window:ehp4., select the following menu choices and 
options:
:ul compact.
:li.File
:li.Open Catalog
:li.Area Type Catalog.  Select the Area of interest by double clicking or entering 
Alt + the underlined letter of the area.
:eul.

  :h1 res=JobPemethAvgS.PEMETH Averages WIndow
:p.The :hp4.PEMETH (Price Estimation Method) Averages:ehp4. window  presents 
information about the [computed average base unit cost] of a standard item. 
The user may use this window to determine the unit cost of a standard item.  
:p.To open this window, select the following menu choices and options:
:ul compact.
:li.File
:li.Open Catalog
:li.Standard Item Catalog
:li.View
:li.PEMETH Averages.  Note:  This window can also be opened from any standard item 
window.
To determine the unit cost of a standard item, perform the following steps:  
:li.Choose the Work Type, Area, and Base Date from the respective combination  
boxes.  These will affect the line graph showing the relationship between 
quantity and price.
:li.Select a quantity  within the graph (by clicking anywhere in the graph).  
The program will determine the unit price.
:eul.
:p.The PEMETH Averages window displays the following data fields:
:p.:hp4.(Title).:ehp4.   The title bar displays the standard item number of the selected 
item. 
:p.:hp4.Base Date.:ehp4.   The base date field shows the new Base Date.  This is declared 
immediately after the bid history is imported into BAMS/CES from BAMS/DSS.
:p.:hp4.Qty Level.:ehp4   The Qty Level field displays the range of quantities to which 
this weighted average applies.  
:p.:hp4.Work Type.:ehp4.   The Work Type field displays the type of work to which this weighted 
average applies.  
:p.Area.   The Area field presents a unique name for the area within an area type.  
:p.:hp4.(Unit Price).:ehp4.   The vertical axis of the graph is formed by the minimum, median, 
and maximum unit prices (displayed on the left of the chart).
:p.:hp4.(Quantity).:ehp4.   The horizontal axis of the graph is formed by the minimum, median, 
and maximum quantities that apply to this job average (displayed on the bottom of 
the chart). 


  :h1 res=ProgramUserS.Program User List Window
:p.To open the :hp4.Program User List:ehp4.window, select the following menu choices and options:
:ul compact.
:li.File 
:li.Open Program
:li.Select a Program
:li.View 
:li.Program User List
:eul.
:p.The Program Users window displays the following data fields:
:p.:hp4.(Title).:ehp4.   The title bar displays the ID of the selected program. 
:p.:hp4.User.:ehp4.   The User field can either display a user ID or a wildcard.  A wildcard 
can specify zero, one, or many user IDs.  
:p.:hp4.Access Privilege.:ehp4.   The Access Privilege shows the access level of the specified 
user(s).


  :h1 res=ProgramJobS.Program Job List Window
:p.To open the :hp4.Program Job List:ehp4. window, select the following menu choices and options:
:ul compact.
:li.File 
:li.Open Program
:li.Choose a program
:li.View
:li.Program Job List 
:eul.
:p.The Program Jobs window displays the following data fields:
:p.:hp4.(Title).:ehp4.   The title bar displays the ID number for the selected program. 
:p.:hp4.Job.:ehp4.   The Job field displays the ID number for each job under the selected 
program. 
:p.:hp4.Letting Date.:ehp4.   The Letting Date field displays the expected letting date for 
each program job. 
:p.:hp4.Work Begin Date.:ehp4.   The Work Begin Date field displays the expected start date 
for each program job.
:p.:hp4.Work End Date.:ehp4.   The Work End Date field displays the expected completion date 
for each program job. 
:p.:hp4.Prty.:ehp4.   The Priority field indicates the importance of each job within the program.  
1 is the highest priority, 2 is less so, etc. 
:p.:hp4.Estimated Total.:ehp4.   The Estimated Total field displays the estimated total cost for 
each job, before Construction Engineering Percent and Contingency Percent are added. 
:p.:hp4.Description.:ehp4.   The Description field displays a truncated version of the detailed 
text description of each program job. 
:p.:hp4.Adj for Inflation.:ehp4.   The Adj for Inflation field presents the estimated total cost 
of a job, including adjustment for inflation. 
:p.:hp4.The Pred of Job box.:ehp4.   The Predecessor of Job box shows all the jobs that must 
precede this job.
:p.:hp4.Job.:ehp4.   The Job label  identifies all the jobs that must be scheduled before the 
selected job.   
:p.:hp4.Link.:ehp4.   The Link field indicates the type of dependency for these jobs.  (SF = 
Start to Finish; SS = Start to Start; FS = Finish to Start; FF = Finish to 
Finish).  See the Introduction chapter for a more detailed description of 
dependency relationships.
:p.:hp4.Lag.:ehp4.   The Lag {XE "Lag "}field indicates the number of days that must elapse 
between jobs.
:p.:hp4.The Succ of Job ... box:ehp4.   The Successor of Job box shows all the jobs that 
must succeed (come after) this job.
:p.:hp4.Job.:ehp4.   The Job field identifies all the jobs that must be scheduled after the 
selected job.
:p.:hp4.Lag.:ehp4.   The Lag field indicates the number of days that must (or are expected 
to) elapse between the jobs.


  :h1 res=ProgramFundS.Program Funds Window
:p.To open the :hp4.Program Funds :ehp4.window, select the following menu choices and options:
:ul compact.
:li.File 
:li.Open Program
:li.Select a program
:li.View
:li.Program Fund List  
:eul.
:p.The Program Funds window displays the following data fields:
:p.:hp4.(Title).:ehp4.   The title bar displays the ID for the selected program.  
:p.:hp4.Fund.:ehp4.   The Fund field displays the ID for each fund available for the selected 
program.
:p.:hp4.Amount Funded.:ehp4.   The Amount Funded field shows the amount of the fund for this  
program (in dollars).
:p.:hp4.Percent Funded.:ehp4.   The Percent Funded field shows the portion (percent) of the 
total program cost supported by this fund.  
:p.:hp4.Description.:ehp4.   The Description field provides a short descriptive name for each 
fund in the list box.  


  :h1 res=ProgramS.Program Window
:p.To open the :hp4.Program:ehp4. window, select the following menu choices and options:
:ul compact.
:li.File 
:li.Open Program
:li.Select a Program.  The program command displays the Program window.
To create a new program, execute the File, New Program command sequence.  
:eul.
:p.The Program window displays the following data fields:
:p.:hp4.(Title).:ehp4.   The title bar displays the ID number of the selected program.  
:p.:hp4.Description.:ehp4.   The Description field describes the program.  
:p.:hp4.Start Date.:ehp4.   The Start Date field shows the expected start date for this program.
:p.:hp4.End Date.:ehp4.   The End Date field shows the expected completion date of this program.
:p.:hp4.No. of Jobs.:ehp4.   The No. of Jobs field presents the sum of the program jobs for this 
program.
:p.:hp4.Total Cost.:ehp4.   The Total Cost field presents the sum of the estimate totals of all 
the program jobs in this program.  This is the estimated total cost of the program. 
:p.:hp4.Total Adjusted for Inflation.:ehp4.   The Total Adjusted for Inflation field shows the 
estimated cost of the entire program, taking inflation into account.  It is the 
sum of all the estimate totals  of all the program jobs in this program  
(adjusted for inflation).  


  :h1 res=JobS.Job Window
:p.The :hp4.Job Window:ehp4. displays general information about a job.  When 
this window is active, all of the following windows can be opened:  Working 
Day Calendar, Job Alternate List, Job User List, Job Breakdown, Job Comment, 
and the Job Fund List.  
:p.To open this window, select the following menu choices and options:
:ul compact.
:li. File 
:li. Open Job 
:li. Select a job--this opens the Job window.
:eul.
:p.The Job window displays the following data fields:
:p.(Title).   The title bar provides a short, descriptive name for the selected 
job.  
:p.:hp4.Description.:ehp4. The Description field defines the selected job.  
:p.:hp4.Work Type.:ehp4.The Work Type field shows the primary type of work  for this 
job (e.g., resurfacing, widening, structure rehabilitation).  Note that the 
window displays both the code for the Work-Type and its translation.
:p.:hp4.County.:ehp4.The County field displays the identification code for the county 
and the code translation. 
:p.:hp4.District.:ehp4.The District field records the district code and its translation.
:p.:hp4.Letting Month.:ehp4.The Letting Month field displays the expected (default) 
month that the job will be let.  The program will use this month unless it is 
told otherwise.
:p.:hp4.Base Date.:ehp4.The Base Date field shows the base date for all pricing 
information used in estimating this job.  
:p.:hp4.Duration.:ehp4.The Duration field records the elapsed time of 
the project.  This value  cannot be changed directly; it is automatically set 
to the most current Base Date when the job is created or manually refreshed.  
:p.:hp4.Spec Year.:ehp4.The Spec Year field shows the year in which the Job Items for 
this job were specified.  
:p.:hp4.Convert To Calendar Days Using.:ehp4.The [Convert To ... Using] field displays 
the ID number for the working day calendar used to estimate the schedule for 
this job.
:p.:hp4.Wages.:ehp4.The Wages field will display Davis Bacon if the project 
is federally funded.  If the project is state funded, this field will display 
Non-Davis Bacon.   
:p.:hp4.Estimator.:ehp4.The Estimator field contains the User ID of the estimator 
creating this job.
:p.:hp4.Date Created.:ehp4.The Date Created field records the date the selected job 
was created.  
:p.:hp4.Last Update.:ehp4.The Last Update field records the last time the selected 
job was revised. 
:p.:hp4.Checked By.:ehp4.The Checked By field shows the User ID of the person checking 
this job.  
:p.:hp4.Approved By.:ehp4.The Approved By field shows the User ID of the person approving 
this job.  
:p.:hp4.Date Checked.:ehp4.The Date Checked field records the date the selected job 
was checked.  
:p.:hp4.Date Approved.:ehp4.The Date Approved field records the approval date for the 
selected job. 
:p.:hp4.Estimated Cost.:ehp4.The Estimated Cost field shows the expected cost of this 
job, before any percentages (or markups) have been added.
:p.:hp4.(Contingency Percentage).:ehp4.The Contingency Percentage field is a percentage 
(of total cost) to add to the total to cover unexpected events.
:p.:hp4.Contingencies.:ehp4.A contingency is an unforseeable event 
such as down time for computers, broken equipment, or an inability to acquire 
needed materials.  The Contingencies figure is calculated by multiplying the 
estimated cost by the contingency percentage.
:p.:hp4.(Construc Engr Percentage).:ehp4.The Construction Engineering Percentage is a 
percentage to add to total cost.  This enables the user to plan for unpredictable 
construction engineering costs. 
:p.:hp4.Construc Engr.:ehp4.The Construc Engr figure is calculated by multiplying the 
estimated cost by the construction engineering percent.
:p.:hp4.Estimate Total.:ehp4.The Estimate Total field is the estimated total cost of 
this job, including the construction engineering and contingency percents.

  :h1 res=JobUserS.Job User Window
:p.The :hp4.Job Users :ehp4.window displays a list of users that have authorized 
access to a particular job.  
:p.To open Job Users Window, select the following menu choices and options:
:ul compact.
:li. File
:li. Open Job
:li. Select the desired Job
:li. View
:li. Job User List--this will display a list of the authorized users on this 
project.
:eul.
:p.The Job Users window displays the following data fields:
:p.:hp4.(Title).:ehp4.The title bar shows the ID for the selected job. 
:p.:hp4.User.:ehp4.The User field shows the ID for each authorized user of the selected 
Job. 
:p.:hp4.Access Privilege.:ehp4.The Access Privilege field shows the access level of 
each authorized user of the selected job.
 
  :h1 res=JobItemsS.Job Items Window
:p.The :hp4.Job Items :ehp4.window displays descriptive information about a list 
of items used in a job breakdown.  When this window is active, the Job Item 
Schedule window and the Job Item window can be opened.   

:p.To open this window, select the following menu choices and options:  
:ul compact.
:li. File
:li. Open Job 
:li. Select the desired job 
:li. View 
:li. Job Breakdowns--This will show a list of job breakdowns.  
:li. View 
:li. Job Breakdown--This will show the specific job breakdown for a specific job number.  
:li. View 
:li. Job Items   
:eul.
:p.The Job Items window displays the following data fields:
:p.:hp4.(Title).:ehp4.The title bar shows the ID numbers for the selected Job-Breakdown and 
for the selected Job. 
:p.:hp4.Item Number.:ehp4.The Item Number field displays the standard item number for each 
job item.  
:p.:hp4.Quantity.:ehp4.The Quantity field shows the number of units of the job item that 
will be used in the selected job.
:p.:hp4.Price.:ehp4.The Price field displays the estimated unit price of a job item.
:p.:hp4.Unit.:ehp4.The Unit field shows the unit of measure  for the job item.
:p.:hp4.Source.:ehp4.The Source field identifies the estimation method used.
:p.:hp4.Extension.:ehp4.The Extension field shows the estimated cost (in dollars) of this 
:p.:hp4.job item.:ehp4.his is equal to the item quantity multiplied by unit price.  
:p.:hp4.Description.:ehp4.The Description field defines the selected item.  


  :h1 res=JobItemScheduleS.Job Item Schedule
:p.The :hp4.Job Item Schedule:ehp4. window displays scheduling information about a selected job 
item.   

:p.To view this window, select the following menu choices and options:  
:ul compact.
:li.File 
:li.Open Job
:li.Select the desired job  
:li.View 
:li.Job Breakdowns--This will show you a list of the Job Breakdowns for a specific job.   
:li.Select a specific job breakdown.
:li.View 
:li.Job Breakdown--this will display a specific job breakdown.  
:li.View
:li.Choose the Job Items option.  
:li.View
:li.Job Items Schedule 
:eul.
:p.To view all the open windows, use the Window Arrange command.  (Cascaded displays 
the title bar of each window, while tiled shows each window in its entirety, 
compressed to fit on one screen).  The cascaded method may not show all windows 
on the first attempt; execute the command again to get a complete sorting of the 
windows.   
:p.The Job Item Schedule window displays the following fields:
:p.:hp4.(Title).:ehp4.The title bar displays the Job ID for the selected Job Item Schedule. 
:p.:hp4.Item Number.:ehp4.The Item Number field displays the ID for each job item in the 
selected ob.  
:p.:hp4.Days Elapsed Before Start.:ehp4.The Days Elapsed Before Start field indicates the 
lag between the job's letting date and the day work on this job item is expected 
to begin.
:p.:hp4.Duration.:ehp4.The Duration field presents the estimated number of days 
to complete this job item.
:p.:hp4.Quantity.:ehp4.The Quantity field presents the necessary quantity of the job item.
:p.:hp4.Unit.:ehp4.The Unit field displays the unit of measure for this item.
:p.:hp4.Description.:ehp4.The Description field provides a short text description of the item.


  :h1 res=JobItemS.Job Item Window
:p.The :hp4.Job Item :ehp4.window provides detailed information about the 
selected job item.  This window can be accessed through the Job Item List 
window.  
:p.To open this window, select the following menu choices and options:  
:ul compact.
:li.File 
:li.Open Job 
:li.Choose a Job  
:li.View 
:li.Job breakdowns--This will display a list of the job breakdowns for this 
job.
:li.Select a job breakdown.
:li.View (2nd time)
:li.Job Breakdown  
:li.View (3rd time)
:li.Job Items--This will display the Items for Job Breakdown x, job y.  
:li.Select the desired item,
:li.View (4th time)
:li.Job Item--This will display the Job Item window. 
:eul.
:p.The Job Item window displays the following data fields:
:p.:hp4.(Title).:ehp4.The title bar displays the Job ID and the Standard Item Number of 
the selected job item.  
:p.:hp4.Item Number.:ehp4.The Item Number field shows the item number of the selected 
job item.
:p.:hp4.Quantity.:ehp4.The Quantity field presents the necessary quantity of the selected 
job item.
:p.:hp4.Unit.:ehp4.The Unit field shows the common unit of measure for the item.  
:p.:hp4.(Hybrid LS).:ehp4.The Hybrid LS label is 
displayed if the unit is a hybrid lump sum.  The term hybrid lump sum is used 
to define a normally unit-priced item that is bid as a  single quantity (lump 
sum).   For instance, often a job such as excavation may require a predictable ]
number of units of some material.  However, the government will ask for a bid 
for the entire job (excavation) to insulate them from the cost of paying for 
additional units of the material at a later date.  Thus, the material in question 
becomes a hybrid lump sum.  It is normally bid by unit, but for this job it is 
treated as a lump sum.
:p.:hp4.Unit Price.:ehp4.The Unit Price field shows the estimated price of one unit of 
the job item.
:p.:hp4.Extension.:ehp4.The Extension field shows the the estimated total cost for this 
job item.  It is equal to the quantity times the unit price.
:p.:hp4.Estimated Using.:ehp4.The Estimated Using field gives the ID number for the 
estimation method used.  
:p.:hp4.Estimation Comment.:ehp4.The Estimation Comment field records a comment  about 
the origin of the estimated unit price.  
:p.:hp4.Short Description.:ehp4.The Short Description field defines the item of work 
presented in this field.
:p.:hp4.Long Description.:ehp4.The Long Description field provides a detailed description 
of the selected item.
:p.:hp4.Suppl.:ehp4.scription.   The Supplemental Description field allows the estimator 
to record any additional description necessary.
:p.:hp4.Start Time.:ehp4.This is the time lag, in days, between the letting date and 
the expected start date for this item.[CHECK] 
:p.:hp4.Rounding Precision.:ehp4.This field shows the rounding precision for this item.  
This is chosen by the estimator.
:p.:hp4.Duration.:ehp4.The Duration field shows the expected time necessary to complete 
an item, in days. 
:p.:hp4.Dependency.:ehp4. A dependency relationship states which actions must come before 
other actions and also states which ones must wait for others to start.  These 
are read...if a, then b ...   For example, a start to finish (SF) dependency for 
A and B would be read, if A starts, then B can finish.  In other words, B cannot 
finish until A has started.  
:p.:hp4.Predecessors.:ehp4.The Predecessors list box shows which items 
must precede the selected job item.  It contains values from three data fields.  
The leftmost value is the  number of a standard item that must precede the 
selected job item.  The center value identifies the dependency between the items 
(SF = Start to Finish; SS = Start to Start; FS = Finish to Start; FF = Finish 
to Finish).  The rightmost value indicates the minimum number of days that must 
elapse between the items.  
(See the Introduction Chapter for a definition of dependency relationships.) 
:p.:hp4.Successors.:ehp4.The Successors list box shows which items must 
succeed (come after) the selected job item.  It contains values from three data 
fields.  The leftmost value is the number of a standard item that must succeed 
the selected job item.  The center value identifies the depandency between the 
items (SF = Start to Finish; SS = Start to Start; FS = Finish to Start; FF = 
Finish to Finish).  The rightmost value indicates the minimum number of days 
that must elapse between the items.  (See the Introduction Chapter for a 
definition of dependency relationships.) 

  :h1 res=JobFundS.Job Fund Window
:p.The :hp4.Job Funds :ehp4.window displays information about funds that support 
a job.  When this window is active, the Job Breakdown Fund List window can be 
opened.   
:p.To view this window, select the following menu choices and options:
:ul compact.
:li.File 
:li.Open Job 
:li.Select the desired job
:li.View  
:li.Job Fund list
:eul.
:p.The Job Funds window displays the following data fields:
:p.:hp4.(Title).:ehp4.The title bar displays the ID number of the selected job. 
:p.:hp4.Fund.:ehp4.The Fund field displays the ID number for each fund available for 
the selected job.  
:p.:hp4.Amount Funded.:ehp4.The Amount Funded field presents the amount of the fund, 
in dollars.  
:p.:hp4.Percent Funded.:ehp4.The Percent Funded field presents the portion of the job 
supported by this fund.
:p.:hp4.Description.:ehp4.The Description field identifies each fund. 

  :h1 res=JobCommentS.Job Comment Window
:p.The :hp4.Job Comments:ehp4. window displays any job comments made by an 
estimator, as well as the date the comments were created. 
:p.To open the Job Comments window, select the following menu choices and 
options: 
:ul compact.
:li.File
:li.Open Job 
:li.Select the desired job 
:li.View 
:li.Job comments 
:eul.
:p.The Job Comments window displays the following data fields:
:p.:hp4.(Title).:ehp4.The title bar displays the ID of the selected job. 
:p.:hp4.Date.:ehp4.The Date field records the date each comment was created.  
:p.:hp4.Comment.:ehp4.The Comment field displays the comment entered by the estimator.  
:p.:hp4.Time.:ehp4.The Time field displays the time each comment was created.  
:p.:hp4.Estimator.:ehp4.The Estimator field shows the User ID of the estimator entering 
the comment. 
:p.:hp4.Archive File.:ehp4.The Archive File field shows the name of 
a file in which an archival copy of the project was saved.  An archive file is 
a file that is no longer active, yet is stored for future reference.  If this 
file is blank, this field will also be blank.

  :h1 res=JobBreakdownFundS.Job Breakdown Funds Window
:p.The :hp4.Job Breakdown Funds:ehp4. window can be opened from the Job Breakdown 
window or the Job Fund List window.  It displays the breakdown of funds financing 
each job.  
:p.To access this window, select the following menu choices and options:
:ul compact.
:li.File 
:li.Open Job 
:li.Select the desired job 
:li.View  
:li.Job Breakdowns--This displays the Job Breakdown for Job X window.  
:li.View--This  displays the specific job breakdown for that job.
:li.View  
:li.Job Fund list 
:eul.
:p.The Job Breakdown Funds window displays the following fields:
:p.:hp4.(Title).:ehp4.The window title bar displays the ID numbers of the selected job 
and job breakdown.
:p.:hp4.Fund.:ehp4.The Fund field displays a unique identifier for each fund available.  
:p.:hp4.Amount Funded.:ehp4.The Amount Funded field shows the amount of the fund, in 
dollars.  
:p.:hp4.Percent Funded.:ehp4.The Percent Funded field shows the portion of the job paid 
for by the selected fund.
:p.:hp4.Description.:ehp4.The Description field defines each fund. 

  :h1 res=JobBreakdownS.Job Breakdown Window
:p.To view the :hp4.Job Breakdown :ehp4.window, select the following menu 
choices and options:  
:ul compact.
:li. File
:li. Open Job
:li. Select a job 
:li. View  
:li. View Job Breakdowns 
:li. The Job Item List and the Job Item Schedule windows may also be opened 
from this window.
:eul.
:p.The Non-Structure Job Breakdown window displays the following data fields:
:p.:hp4.(Title).:ehp4.The title bar displays the ID numbers for the selected job and 
job breakdown.  
:p.:hp4.Description.:ehp4.The Description field defines the selected job breakdown.
:p.:hp4.Length.:ehp4.The Length field displays the length of the work represented by 
this breakdown (in miles).
:p.:hp4.Road Type.:ehp4.The Road Type field displays the type of road being estimated 
(e.g., four lane divided highway, two-lane paved).
:p.:hp4.Pavement Width.:ehp4.The Pavement Width field shows the width of paving for 
this breakdown, in feet.
:p.:hp4.Pavement Depth.:ehp4.The Pavement Depth field shows the pavement depth, in 
inches, for this breakdown.
:p.:hp4.Structure Type.:ehp4.The Structure Type field indicates the type of structure 
being built.  Note that both the structure type code and the translation are 
displayed for the Structure Type.
:p.:hp4.Detailed Estimate Total.:ehp4.The Detailed Estimate Total field shows the 
total cost for this job breakdown using per-item estimates.  
:p.:hp4.Preliminary Estimate Total.:ehp4.The Preliminary Estimate Total field displays 
the total cost for this job breakdown, as computed by the job breakdown 
estimation module.
:p.:hp4.(Name of Method).:ehp4.The Name of Method field records the job breakdown 
estimation method that was used to arrive at the preliminary estimate.

  :h1 res=JobAlternateGroupS.Job Alternate Group Window
:p.The :hp4.Job Alternate Group:ehp4. window displays information about a job
alternate group and the various ways the group can be designed.  A job alternate 
is an alternate method of performing a job.  For instance, if the job is a 
bridge, three alternates (types of bridge designs) would be wood, metal, or 
steel.  A job alternate group would be the actual structure (a group of  
bridges, on-ramps, etc.) that had several different possible methods of design 
or construction.
:p.To open  the Job Alternate Group Window, select the following menu choices 
and options:
:ul compact.
:li. File 
:li. Open Job 
:li. Select a job by double clicking on the job or highlighting and pressing 
enter.  
:li. View 
:li. Job Alternate Groups  
:eul.
:p.The Job Alternate Groups window displays the following data fields:
:p.:hp4.(Title).:ehp4.The window title bar displays the ID numbers of the selected job 
and job alternate group.  
:p.:hp4.Description.:ehp4.The Description field defines the selected job alternate 
group.
:p.:hp4.Chosen Estimate.:ehp4.The Chosen Estimate field shows the estimated cost of 
the chosen Job Alternate (type of design) in this job alternate group (the 
structure).  From the example above, if concrete cojstruction were chosen, 
the estimate would be for that type of construction.  
:p.:hp4.(Chosen Job Alternate).:ehp4.
:p.:hp4.Job Alternate.:ehp4.The Job Alternate field displays the ID  number for each 
available job alternate.
:p.:hp4.Description.:ehp4.The Description field describes a job alternate.
:p.:hp4.Estimate.:ehp4.The Estimate field shows the cost of the displayed job alternate.

  :h1 res=DefaultUserS.Default User
      :p.This is the help text for defaultuserstatic

  :h1 res=JobBreakdownListS.Job Breakdown List
      :p.This window displays all of the job breakdowns for a given job.  
To see the job breakdown, doble-click on the breakdown you wish to see.


  :h1 res=JobAlternateGroupListS.Job Alternate Group List Window
:p.The :hp4.Job Alternate Group List :ehp4.window displays a list of all the job alternates for the 
selected job alternate group.  A job alternate is an alternate method of performing 
a job.  For instance, if the job is a bridge, three alternates (types of bridge 
designs) would be wood, metal, or steel.  A job alternate group would be the 
actual structure (a group of  bridges, on-ramps, etc.) that had several different 
possible methods of design or construction.

:p.To open the Job Alternate Group window, perform the following steps:
:ul compact.
:li. File 
:li. Open Job 
:li. Select a job by double clicking on the job or highlighting and pressing enter.  
:li. View 
:li. Job Alternate Groups  
:li. View 
:li. Job Alternate Group
:eul.
:p.The Job Alternate Group window displays the following data fields:
:p.:hp4.(Title).:ehp4.The window title bar displays the ID numbers of the selected job and 
job alternate group.  
:p.:hp4.Description.:ehp4.The Description field defines the selected job alternate group.
:p.:hp4.Chosen Estimate.:ehp4.The Chosen Estimate field shows the estimated cost of the 
chosen Job Alternate (type of design) in this job alternate group (the structure).  
From the example above, if concrete construction were chosen, the estimate would be 
for that type of construction.  
(Chosen Job Alternate).   
:p.:hp4.Job Alternate.:ehp4.The Job Alternate field displays the ID  number for each available 
job alternate.
:p.:hp4.Description.:ehp4.The Description field describes a job alternate.
:p.:hp4.Estimate.:ehp4.The Estimate field shows the cost of the displayed job alternate.

  :h1 res=ProposalS.Proposal Window
:p.The :hp4.Proposal :ehp4.window displays a list of all the proposals for a job. 
:p.To open the Proposal window, perform the following steps:
:ul compact.
:li. File 
:li. Open Job 
:li. Select a job by double clicking on the job or highlighting and pressing enter.  
:li. View 
:li. Proposal  
:li. 
:li. 
:eul.
:p.The Proposal window displays the following data fields:


  :h1 res=BidderS.Bidder Window
:p.:p.The :hp4.Bidder :ehp4.window displays a list of all the bidders for a job. 
:p.To open the Bidder window, perform the following steps:
:ul compact.
:li. File 
:li. Open Job 
:li. Select a job by double clicking on the job or highlighting and pressing enter.  
:li. View 
:li. Bidder  
:li. 
:li. 
:eul.
:p.The Bidder window displays the following data fields:


  :h1 res=ProjectS.Project Window
:p.The :hp4.Project :ehp4.window displays a list of all the projects for a job. 
:p.To open the project window, perform the following steps:
:ul compact.
:li. File 
:li. Open Job 
:li. Select a job by double clicking on the job or highlighting and pressing enter.  
:li. View 
:li. Project  
:li. 
:li. 
:eul.
:p.The Project window displays the following data fields:

  :h1 res=ProposalItemS.Proposal Item Window
:p.The :hp4.Proposal Item :ehp4.window displays a list of all the Proposal Items for a job. 
:p.To open the Proposal Item window, perform the following steps:
:ul compact.
:li. File 
:li. Open Job 
:li. Select a job by double clicking on the job or highlighting and pressing enter.  
:li. View 
:li.   
:li. 
:li. 
:eul.
:p.The Proposal Item window displays the following data fields:


  :h1 res=ProposalMajItemS.Proposal Major Item Window
:p.The :hp4.Proposal Major Item :ehp4.window displays a list of all the Proposal Major Items for a job. 
:p.To open the Proposal Major Item window, perform the following steps:
:ul compact.
:li. File 
:li. Open Job 
:li. Select a job by double clicking on the job or highlighting and pressing enter.  
:li. View 
:li.   
:li. 
:li. 
:eul.
:p.The Proposal Major Item window displays the following data fields:


  :h1 res=CategoryS.Category Window
:p.The :hp4.Category :ehp4.window displays a list of all the Categorys for a job. 
:p.To open the Category window, perform the following steps:
:ul compact.
:li. File 
:li. Open Job 
:li. Select a job by double clicking on the job or highlighting and pressing enter.  
:li. View 
:li.   
:li. 
:li. 
:eul.
:p.The Category window displays the following data fields:


  :h1 res=ProjectItemS.Project Item Window
:p.The :hp4.Project Item:ehp4. window displays a list of all the project Itemss for a job. 
:p.To open the Project Item window, perform the following steps:
:ul compact.
:li. File 
:li. Open Job 
:li. Select a job by double clicking on the job or highlighting and pressing enter.  
:li. View 
:li. Project Item  
:li. 
:li. 
:eul.
:p.The Project Item window displays the following data fields:


  :h1 res=CodeTableS.Code Table Catalog
:p.The :hp4.Code Table Catalog:ehp4. window displays a list of
all the current code tables.  
:p.To view a code table catalog, select the following menu choices and options: 
:ul compact.
:li.File
:li.Open Catalog
:li.Code Table Catalog
:eul.
:p.The Code Table Catalog window displays the following data fields:
:p.:hp4.(Title).:ehp4. The title field shows the name of this catalog.
:p.:hp4.Code Table.:ehp4. The code table provides an identifying code for each code table.
:p.:hp4.Description.:ehp4. The description field provides a description for each code table.


  :h1 res=CodeValueS.Code Value Window
:p.The :hp4.Code Value:ehp4. window displays the code values. 

:p.To open a code value window, select the following menu choices and options:
:ul compact.
:li.File
:li.Open Catalog
:li.Code Table Catalog
:li.Choose the desired code tablet by double clicking on that code table or by 
highlighting the code and pressing enter. 
:eul.
:p.The Code Value window displays the following data fields: 


  :h1 res=CostSheetCatS.Cost Sheet Catalog
:p.The :hp4.Cost Sheet Catalog:ehp4. window displays a list of
all the current cost sheets in the cost-based estimation module.  
:p.To view a cost sheet catalog, select the following menu choices and options: 
:ul compact.
:li.File
:li.Open Catalog
:li.Cost Sheet Catalog
:eul.
:p.The Cost Sheet Catalog window displays the following data fields:
:p.:hp4.(Title).:ehp4. The title field shows the name of this catalog.
:p.:hp4.Cost Sheet.:ehp4. The cost sheet provides an identifying code for each cost sheet.
:p.:hp4.Description.:ehp4. The description field provides a description for each cost sheet.


  :h1 res=CostSheetS.Cost Sheet Window
:p.Each individual :hp4.Cost Sheet:ehp4. displays data used to cost a task  by scratch-based 
estimation.  It includes information such as material needed, crew requirements 
(equipment and labor), and overhead and contractor markup percentages.  
To view an individual cost sheet, double click or highlight and select the desired 
cost sheet.  Only one cost sheet can be viewed at a time.  Therefore, if another 
is selected, the first will be unselected.  To add a new cost sheet, you must 
be in the Cost Sheet Catalog window.  

:p.To open a cost sheet, select the following menu choices and options:
:ul compact.
:li.File
:li.Open Catalog
:li.Cost Sheet Catalog
:li.Choose the desired cost sheet by double clicking on that cost sheet or by 
highlighting the cost sheet and pressing enter. 
:eul.
:p.The Cost Sheet window displays the following data fields: 
:p.:hp4.(Title).:ehp4.The cost sheet ID of the selected cost sheet is displayed in the 
window title bar.  
:p.:hp4.Description.:ehp4.The Description field presents the assumptions and details on 
which the selected cost sheet is based.
:p.:hp4.Production Rate.:ehp4.The Production Rate field shows the default production 
rate (in units of the estimated job item) per day.  This is used to determine 
daily labor and equipment costs.  
:p.:hp4.Production Granularity.:ehp4.The Production Granularity field shows the unit of 
time (e.g., day) to which the work on a job item must conform.  Production rate 
is adjusted so that the work requires a whole number of the unit of time.  For 
instance,  a job might take nine hours to complete, but in reality the crew will 
probably spend a day and a half or two full days.  The production rate would be 
adjusted to a whole number of days.
:p.:hp4.Default Hours/Day.:ehp4.The Default Hours/Day field shows the number of hours per 
day the crews are expected to work.  The program will use this number in its 
calculations (by default) unless another number is specified.
:p.:hp4.The Overhead Fields:  The Overhead fields show the amount that 
will be added to the program cost to cover overhead.:ehp4.verhead expense is an 
accounting convention which describes costs that are not directly assignable 
to one particular area or group.  For instance, it would be practically impossible 
to divide up the costs of lighting in a building by office or even by floor.  
These costs can be fixed or variable.  The following three percentages are a way 
to include these costs in the estimate
:p.:hp4.Materials Overhead.:ehp4.The Materials-Overhead Pct. field shows the percentage 
markup the contractor will add to the material cost to cover overhead 
(non-assignable) expenses.    
:p.:hp4.Equipment Overhead.:ehp4.The Equipment-Overhead field shows the percentage the 
contractor will add to the equipment cost to account for his or her cost of 
using equipment.  
:p.:hp4.Labor Overhead.:ehp4.The Labor Overhead field shows 
the percentage the contractor will add to the labor cost to account for fixed 
costs (e.g., maintaining an office and paying office staff).
:p.:hp4.Markup Pct.:ehp4.The Markup Pct field displays an estimation of the percentage 
to add to the estimated total job item cost to account for the contractor's 
profit.  

  :h1 res=CountyCatS.County Catalog
:p.The :hp4.County Catalog:ehp4. window displays the catalog of counties within 
the state.  It can be accessed from the Area Catalog window by selecting an area 
and choosing the desired area in  the Counties box.  
P.To open the County Catalog window, select the following menu choices and options:
:ul compact.
:li.File
:li.Open Catalog
:li.County Catalog 
:eul.
:p.The County Catalog window displays the following data fields:
:p.:hp4.County.:ehp4.The County field shows the abbreviation or code that uniquely 
identifies a county.  
:p.:hp4.Name.:ehp4.The Name field shows the name of the county.

  :h1 res=CrewCatS.Crew Catalog
:p.The :hp4.Crew Catalog:ehp4. window displays a list of all crews currently 
maintained by the cost-based estimation module. 
:p.To open the Crew Catalog window, select the following menu choices and options:
:ul compact.
:li.File
:li.Open Catalog
:li.Crew Catalog
:eul.
:p.The Crew Catalog window displays the following data fields:
:p.:hp4.Crew.:ehp4.The Crew field shows the Crew ID of each crew in the catalog. 
:p.:hp4.Description.:ehp4.The Description field shows the labor and equipment that comprises 
each crew. 


  :h1 res=CrewS.Crew Window
:p.The :hp4.Crew Window:ehp4. displays crew information such as laborers and 
equipment needed for this crew.  
:p.To open the Crew window, select the following menu choices and options:
:ul compact.
:li.File
:li.Open Catalog
:li.Crew Catalog
:li.Select the Crew by double clicking on a specific crew or by highlighting 
and pressing the enter key.  The program will display a breakdown of the 
selected crew.
:eul.
:p.The Crew window displays the following fields:
:p.:hp4.(Title).:ehp4.The window title bar displays the Crew ID of the selected crew. 
:p.:hp4.Description.:ehp4.The Description field lists the labor and equipment that 
comprise the selected crew. 
:p.The Left Half of the Window:
:p.:hp4.Laborer.:ehp4.The Laborer field presents the ID for each type of laborer in 
the selected crew.  
:p.:hp4.Quantity.:ehp4.The Quantity field (in the left half of this window) shows 
the amount of this particular kind of laborer needed for this crew.
:p.:hp4.Description.:ehp4.The Description field (in the left half of this window) 
defines each type of laborer needed. 
:p.The Right Half of the Window:
:p.:hp4.Equipment.:ehp4.The Equipment fields shows the ID for each type of equipment 
used by the crew. 
:p.:hp4.Quantity.:ehp4.This Quantity field (in the right half of this window) displays 
the amount of this kind of equipment used by this crew.  
:p.:hp4.Description.:ehp4.The Description field (in the right half of this window) 
defines each type of equipment used by this crew. 

  :h1 res=DefaultUnitPriceCatS.Default Unit Price Catalog
:p.The :hp4.Default Unit Price Catalog:ehp4. window displays a catalog of 
standard items, their unit of measure, and their Default Price.
Double clicking on an item will bring up the Default Unit Price window.
:p.To open this window, select the following menu choices and options:
:ul compact.
:li.File
:li.Open Catalog
:li.Default Unit Price Catalog
:eul.
:p.The Default Unit Price Catalog window displays the following data fields: 
:p.:hp4.Item Number.:ehp4.The Item Number field shows the number for each standard item.
:p.:hp4.Description.:ehp4.The Description field defines the default price for each item.  
This will be truncated, if necessary, to fit in the space provided.  
:p.:hp4.Unit.:ehp4.The Unit field shows the unit of measure for each standard item.  
:p.:hp4.Default Price.:ehp4.The Default Price field shows the price that will be included 
in the estimation unless the user specifies otherwise. 


  :h1 res=DefaultUnitPriceS.Default Unit Price Window
:p.The :hp4.Default Unit Price:ehp4. window shows a default unit price for an item.  A default 
price is a price that will be used by the program unless the user specifies 
otherwise.  
:p.To open :hp4.Default Unit Price Window:ehp4., select the following menu 
choices and options:
:ul compact.
:li. File
:li. Open Catalog
:li. Default Unit Price Catalog
:li. Select an item to view the default unit price breakdown.
:eul.
:p.The Default Unit Price window displays the following data fields:
:p.:hp4.(Title).:ehp4.The title bar displays the Standard Item Number for the selected 
standard item.  
:p.:hp4.Item Number.:ehp4.The Item Number field displays the item number for the 
selected standard item again.    
:p.:hp4.Item Description.:ehp4.The Item Description field provides a short description 
of the item.
:p.:hp4.Unit.:ehp4.The Unit field displays the common unit of measure for the selected 
item.
:p.:hp4.Price Description.:ehp4.The Price Description field provides a short description 
or explanation of the default price.  
:p.:hp4.Price.:ehp4.The Price field displays the default unit price for the selected item.  
The program will use this price "by default" unless another price is entered. 

  :h1 res=FacilityBrowserResultsS.Facility Browser Results
      :p.This is the help text for facilitybrowserresultss

  :h1 res=ItemBrowserResultsS.Item Browser Results
      :p.This is the help text for itembrowserresultsstatic

  :h1 res=PropBrowserResultsS.Proposal Browser Results
      :p.This is the help text for propbrowserresultss

  :h1 res=EquipmentCatS. Equipment Catalog
:p.The :hp4.Equipment Catalog:ehp4. window displays the complete list of 
equipment used in the cost-based estimation module. To get further information 
about a piece of equipment, double click on that line (or select that piece of 
equipment and hit enter).  
:p.To open this window, select the following menu choices and options:
:ul compact.
:li.File
:li.Open Catalog
:li.Equipment Catalog
:eul.
:p.The Equipment Catalog window displays the following data fields:
:p.:hp4.Equipment.:ehp4.The Equipment field displays the unique ID for each piece of 
equipment in the catalog. 
:p.:hp4.Description.:ehp4.The Description field describes each piece of equipment in the 
catalog.  The description is truncated, if necessary, to fit on one line in the 
space allocated.  
:p.:hp4.Zone Type.:ehp4.The Zone Type field displays the zone into which each piece of 
equipment is categorized. 


  :h1 res=EquipmentS. Equipment Window
:p.The :hp4.Equipment Window:ehp4. displays the regular and overtime rates for 
Equipment in the Equipment Catalog.  
:p.To open this window, select the following menu choices and options:
:ul compact.
:li.Select File
:li.Open Catalog
:li.Equipment Catalog
:eul.
:p.Double click on one line or highlight and select one line to view an equipment 
window.  
:p.The Equipment window displays the following data fields:
:p.:hp4.(Title).:ehp4.The title bar displays the Equipment ID for the selected piece 
of equipment. 
:p.:hp4.Description.:ehp4.The Description field defines the selected piece of equipment.
:p.:hp4.Zone Type.:ehp4.The Zone Type field shows the ID of the zone type in which the 
selected piece of equipment is used.  
:p.:hp4.Zone.:ehp4.The Zone field displays each zone in which the piece of equipment 
has been priced. 
:p.:hp4.Hourly Rate.:ehp4.The Hourly Rate field shows the regular rate for using one 
piece of the selected equipment within a specific zone. 
:p.:hp4.Overtime Rate.:ehp4.The Overtime Rate field shows the overtime rate for this 
piece of equipment within a specific zone.   

  :h1 res=FacilityCatS.Facility Catalog
      :p.This is the help text for facility_cat_static

  :h1 res=FacilityForVendorS.Facility For Vendor
      :p.This is the help text for facilityforvendor_static

  :h1 res=FacilityForMaterialS.Facility For Material
      :p.This is the help text for facilityformaterial_static

  :h1 res=FacilityS.Facility
      :p.This is the help text for facility_static

  :h1 res=FundCatS.Fund Catalog
:p.The :hp4.Fund Catalog:ehp4. window displays the catalog of funds that are 
used to support all or part of a job created by BAMS/CES.  
:p.To open this window, select the following menu choices and options: 
:ul compact.
:li.File
:li.Open Catalog
:li.Fund Catalog
:eul.
:p.The Fund Catalog window displays the following data fields:
:p.:hp4.Fund.:ehp4.The Fund field shows the unique identifier for each fund in the catalog.  
:p.:hp4.Name.:ehp4.The Name field provides a short, descriptive name for each fund in the 
fund catalog. 
:p.:hp4.Description.:ehp4.The Description field describes each fund in the fund catalog.  


  :h1 res=InflationPredictionCatS.Inflation Prediction Catalog
:p.The :hp4.Inflation Prediction Catalog:ehp4. window shows inflation 
predictions that will affect pricing.   
:p.To open this window, select the following menu choices and options:
:ul compact.
:li.File
:li.Open Catalog
:li.Inflation Prediction Catalog
:eul.
:p.The Inflation Prediction Catalog window displays the following data fields:
:p.:hp4.Inflation Prediction.:ehp4.The Inflation Prediction field displays the unique name 
for each inflation prediction in the catalog .  
:p.:hp4.Initial Prediction.:ehp4.The Initial Prediction field displays the annual 
inflation rate to be used  for an inflation prediction. This will be used until 
the first inflation change entry. 
:p.:hp4.Comment.:ehp4.The Comment field contains a short comment regarding a given 
inflation prediction.  

  :h1 res=InflationPredictionS.Inflation Prediction Window
:p.The :hp4.Inflation Prediction Window:ehp4. displays information about an 
inflation prediction for a program job.  Entries in the list box indicate when 
a particular inflation rate is expected to change and the expected rate after 
the change.
:p.To open this window, select the following menu choices and options;
:ul compact.
:li.File
:li.Open program
:li.Select the desired program  
:li.View
:li.Program Job List.  This will display the "jobs for program x, program x" 
window.  
:li.View
:li.Inflation Prediction  window.  These windows can be arranged (with the 
Window arrange command) to be all on the screen at once (tiled) or just the 
title bars showing (cascaded).   
:eul.
:p.The Inflation Prediction window displays the following data fields:
:p.:hp4.(Title).:ehp4.The title bar shows the Inflation Prediction ID.  If this window 
was activated from the Program Job List window, it would show the Program ID.  
If it was activated from the Inflation Prediction Catalog window, it would 
show the job ID.
:p.:hp4.Job Description.:ehp4.The Job Description field describes the selected job.  
:p.:hp4.Prediction Description.:ehp4.The Prediction Description field describes the 
assumptions made for the selected inflation prediction.
:p.:hp4.Initial Prediction.:ehp4.The Initial Prediction field shows the annual 
inflation rate.  This will be used for times before (or up to) the first 
inflation change.  
:p.:hp4.Change Date.:ehp4.The Change Date field displays the date on which the new 
prediction takes effect.
:p.:hp4.Prediction.:ehp4.The Prediction field shows the annual inflation rate to use 
from the Change Date forward, until the next change.  
:p.:hp4.Comment.:ehp4.The Comment field provides any additional information about the 
Inflation Change prediction.


  :h1 res=LaborCatS.Labor Catalog
:p.The :hp4.Labor Catalog:ehp4. window displays all the laborer types used in 
cost-based estimation. 
:p.To open this window, select the following menu choices and options:
:ul compact.
:li.File 
:li.Open Catalog
:li.Labor Catalog
:eul. 
:p.The Labor Catalog window displays the following data fields:
:p.:hp4.Laborer.:ehp4.The Laborer field displays a short, unique identifier for each type 
of laborer in the list box.  
Description.   The Description field provides a more complete description of 
each type of laborer.  This will be truncated, if necessary, to fit in the space 
allocated.
:p.:hp4.Zone Type.:ehp4.The Zone-Type field displays the ID of the zone type to which each 
type of laborer belongs.  

  :h1 res=LaborerS.Labor Wage List Window
:p.The :hp4.Labor Wage List:ehp4. window allows the user to get more detailed wage information 
about the cost of a specific laborer.  It displays the Davis-Bacon and 
non-Davis-Bacon wage costs for the selected laborer (for both regular and 
overtime rates).
:p.To open the Labor Wage List window, select the following menu choices and options:
:ul compact.
:li.File 
:li.Open Catalog
:li.Select Labor Catalog
:li.Select a laborer (by double clicking or cursoring) to view the Labor Wage List 
window.
:eul.
:p.The Labor Wage List window displays the following data fields:
:p.:hp4.(Title).:ehp4.   The title bar displays the ID for the selected laborer.  
:p.:hp4.Zone Type.:ehp4.   The Zone Type field displays the zone type to which the selected 
laborer belongs.
:p.:hp4.Description.:ehp4.   The Description field is a more complete description of this kind 
of laborer.  This will be truncated, if necessary, to fit in the space allocated.
:p.:hp4.Zone.:ehp4.   The Zone field displays the ID for each zone for which laborer pricing 
information is available.
:p.:hp4.Davis-Bacon Hourly Rate.:ehp4.   This is the regular rate (dollars per hour) for the 
selected laborer when working on a federally funded job.  
:p.:hp4.Davis-Bacon Overtime Rate.:ehp4.   This is the overtime rate (dollars per hour) for 
the selected laborer when working on a federally funded job.  
:p.:hp4.Non-Davis-Bacon Hourly Rate.:ehp4.   This is the regular rate (dollars per hour) for 
the selected laborer when working on a state-funded job.
:p.:hp4.Non-Davis-Bacon Overtime Rate.:ehp4.   This is the overtime rate (dollars per hour) 
for the selected laborer when working on a state-funded job.


  :h1 res=MajorItemCatS.Major Item Catalog
:p.The :hp4.Major Item Catalog:ehp4. window displays the catalog of all major 
items used by BAMS/CES.  A major item is a general, quantifiable portion of work.
Examples of major items are cubic yards of concrete, pounds of steel, and cubic 
yards of excavation.  Major items are not biddable items.
:p.To view the Major Item Catalog window, select the following menu choices and options:
:ul compact.
:li.File
:li.Open Catalog
:li.Major Item Catalog
:eul.
:p.The Major Item Catalog window displays the following data fields:
:p.:hp4.Major Item.:ehp4.The Major Item field displays the unique identifier for each major 
item.  
:p.:hp4.Description.:ehp4.The Description field defines the major item. 
Unit.   The Unit field displays the common unit of measure for this major item.  
For example, if the major item is Concrete, the Common Unit  would be cubic yards.  

  :h1 res=MajorItemS.Major Item Window
:p.The :hp4.Major Item :ehp4.window displays information about the indicated 
major item, such as a list of standard items used for this major item and the 
percentage of the major item's cost and quantity for which each standard item 
is responsible.  
:p.To open the Major Item window, select the following menu choices and options:
:ul compact.
:li.File 
:li.Open Catalog
:li.Select a major item either by cursoring to the desired item or by clicking 
with the mouse.
:li.Double click on the selected item. 
:eul.
:p.The Major Item window displays the following data fields:
:p.:hp4.(Window Title).:ehp4.The title bar displays the ID for the selected major item.
:p.:hp4.Major Item.:ehp4.The Major Item field displays the unique identifier for the 
selected major item.  
:p.:hp4.Description.:ehp4.The Description field defines the selected major item.  
:p.:hp4.Common Unit.:ehp4.The Common Unit field shows the most common unit of measure 
for this major item.
:p.:hp4.Standard Item.:ehp4.The Standard Item field shows the unique identifier for each 
standard item.
:p.:hp4.Quantity %.:ehp4.The Quantity % field shows the portion of the major item quantity 
due to each unit of the standard item.
:p.:hp4.Cost %.:ehp4.The Cost % field shows the portion of the major item's cost that 
is due to the standard item. 

  :h1 res=MapCatS.Map Catalog
:p.The :hp4.Map Catalog:ehp4. window displays the catalog of all maps used by 
BAMS/CES.  A map is a collection of spatial points which describe some or more 
geographical areas.  There are maps for counties and zones.  
:p.To open the Map Catalog window, select the following menu choices and options:
:ul compact.
:li.File 
:li.Open Catalog
:li.Map Catalog
:eul.
:p.The Map Catalog window displays the following data fields:
:p.:hp4.Map.:ehp4.   The Map field shows the unique identifier for a given map.  
:p.:hp4.Description.:ehp4.The Description field provides a short descriptive name for a map, 
for example, Heavy Equipment Operator - Zone 2A.

  :h1 res=MaterialCatS.Material Catalog
:p.The :hp4.Material Catalog:ehp4. window displays the catalog of all materials 
used by BAMS/CES.  A material is anything that is used up (as opposed to just 
used) when performing work.   Two examples of materials are concrete and steel.  
:p.To open this window, select the following menu choices and options:
:ul compact.
:li.File 
:li.Open Catalog
:li.Material Catalog
:eul.
:p.The Material Catalog window displays the following data fields:
:p.:hp4.Material.:ehp4.The Material field displays the identification code for each type of 
material.
:p.:hp4.Description.:ehp4.The Description field defines each type of material.
:p.:hp4.Unit.:ehp4.The Unit field displays the unit of measure used to price this material. 

  :h1 res=MaterialPriceS.Material Price List Window
:p.The :hp4.Material Price List :ehp4.window allows the user to investigate the 
price of a particular material.  Material prices may be different, depending  
upon the particular zone and the base date of the material.  
:p.To open this window, select the following menu choices and options:
:ul compact.
:li.File
:li.Open Catalog 
:li.Material Catalog
:li.Select a material.  Double click on that material to see the price list 
window for that material.
:eul.
:p.The Material Price List window displays the following data fields:
:p.:hp4.(Title).:ehp4.The title bar displays the ID for the selected material. 
:p.:hp4.Description.:ehp4.The Description field defines the selected material.  This 
will be truncated, if necessary, to fit in the space provided.
:p.:hp4.Zone Type.:ehp4.The Zone Type field shows the ID of the zone type for this 
material.
:p.:hp4.Zone.:ehp4.The Zone field displays the zones for which pricing information is 
available.  
:p.:hp4.Price.:ehp4.The Price field displays the unit cost for this kind of material, 
in the corresponding zone, on or after the base date.  
:p.:hp4.Unit.:ehp4.The Unit field displays the unit of measure for this material.    

  :h1 res=JobBreakdownProfileS.Job Breakdown Profile Window
:p.The :hp4.Job Breakdown Profile :ehp4.window displays the major items for 
this profile.  Each major item displayed shows its quantity and price, along 
with its typical and current percentages of total job breakdown cost.  
The quantities and prices of Job Breakdown Major Items can be edited.  major 
items of a job breakdown can be added or deleted.
:p.To open the Job Breakdown Profile window, select the following menu choices 
and options:
:ul compact.
:li.File 
:li.Open Job 
:li.Select the desired job 
:li.View  
:li.Job Breakdowns--This displays the "Job Breakdown for Job X" window.  
:li.View 
:li.Job Breakdown
:li.View  
:li.Job Breakdown Profile.
:eul.
:p.The Job Breakdown Profile window displays the following fields:
:p.:hp4.(Title).:ehp4.The title bar displays the ID numbers for the parametric profile 
and the selected job breakdown.  
:p.:hp4.Description.:ehp4.The Description field describes the situations for which 
this profile is appropriate.  
:p.:hp4.Major Item.:ehp4.The Major Item field provides the unique identifier (maximum 
4 characters) for each major item. 
:p.:hp4.Quantity.:ehp4.The Quantity field displays the required quantity (in common 
units) of each job major item.  
:p.:hp4.Unit.:ehp4.The Unit field shows the common unit of measure for this major item.  
:p.:hp4.Price.:ehp4.The Price shows the unit price of the major item.  
:p.:hp4.Extension.:ehp4.The Extension field shows the extended price for each major 
item.  It is calculated by multiplying the unit price times the quantity.  
:p.:hp4.Job Breakdown Typical.:ehp4.The Job Breakdown Typical field shows the typical 
portion (of the job breakdown total) this major item represents.
:p.:hp4.Job Breakdown Current.:ehp4.The Job Breakdown Current field shows the current 
portion (of the job breakdown total) this major item represents.  This is 
calculated by dividing the [extension price for an item] by the [estimated 
total cost of the job breakdown].
:p.:hp4.Description.:ehp4.The Description field defines each major item displayed in 
the list box.  
:p.:hp4.Totals.:ehp4.The Totals field presents the typical and current percentage 
totals and the total extension price.  

  :h1 res=ParametricProfileCatS.Parametric Profile Catalog
:p.The :hp4.Parametric Profile Catalog :ehp4.window displays a list of the parametric profiles.  
A parametric profile is a description of the parameters  used in estimating 
an item.  A parameter is a set of physical properties whose 
values determine the characteristics, behavior or cost of an item.   
In  this context, they are factors that affect the cost of an item.  Some 
parameters for a road might be its length, width, number of lanes, location, 
construction material, intended use, etc.  These profiles can be added, change, 
or deleted using this window.  The user can open the Significant Major Item 
window from this window to obtain more information about the significant major 
items that affect this estimation.  
:p.To open this window, select the following menu choices and options:
:ul compact.
:li.File
:li.Open Catalog
:li.Parametric Profile Catalog
:eul.
:p.The Parametric Profile window displays the following data fields:
:p.:hp4.Profile.:ehp4.The Profile field displays the unique identifier for each profile 
(maximum 16 characters).  
:p.:hp4.Description.:ehp4.The Description field defines the situations in which each 
profile can be reasonably applied.  This will be truncated, if necessary, to 
fit on one line.  

  :h1 res=JobBreakdownMajorItemS.Job Breakdown Major Item
      :p.This is the help text for jobbreakdownmajitemstatic

  :h1 res=SignifMajItemS.Significant Major Item
:p.The :hp4.Significant Major Item:ehp4. window displays a list of the 
significant major items for the selected parametric profile.  A significant 
major item is a major item deemed to contribute a large portion of the cost 
of a job or standard item.  These major items can be added to, changed, or 
deleted from the selected profile using this window.  However, the sum of 
the Percent-of-Cost fields of all the significant major items for a parametric 
profile cannot exceed 100%.  
:p.To open this window, select the following menu choices and options:
:ul compact.
:li.File
:li.Open Catalog
:li.Parametric Profile Catalog
:li.Select a parametric profile
:li.View
:li.Significant Major Item list.  Note:  This window can also be opened from the 
Job Breakdown Profile window.
:eul.
:p.The Significant Major Item window displays the following data fields:
:p.:hp4.(Title).:ehp4.The title bar displays the ID for the selected parametric profile. 
:p.:hp4.Major Item.:ehp4.The Major Item field displays theID for each major item. 
:p.:hp4.Description.:ehp4.The Description field defines the major items displayed.  The 
description will be truncated, if necessary, to fit in the space allocated.  
:p.:hp4.% of Cost.:ehp4.The % of Cost field displays the portion of total cost this major 
item represents.   
:p.:hp4.Total.:ehp4.The Total field is calculated by summing all % of Cost values.  The 
total cannot exceed 100%.  


  :h1 res=JobMajItemPriceRegrS.Job Major Item Price Regression
:p.The :hp4.Job Major Item Price Regression :ehp4.window displays price 
regression data for the selected job major item.  No editing can be performed 
in this window.  
:p.To open this window, select the following menu choices and options:
:ul compact.
:li.File
:li.Open Job
:li.Select the desired job.
:li.View
:li.Job Breakdowns
:li.View
:li.View Job Breakdown
:li.View
:li.Job Major Item List
:li.View
:li.Job Major Item Price Regression  
:eul.
:p.The Major Item Price Regression window displays the following data fields:
:p.:hp4.(Window Title).:ehp4.The title bar displays the Job and Major Item ID numbers  
for the selected job major items.  
:p.:hp4.Job Breakdown.:ehp4.The Job Breakdown field presents the ID number for the 
selected job breakdown.
:p.:hp4.Description.:ehp4.The Description field records a short text description for 
the selected job breakdown.  It will be truncated, if necessary, to fit on 
one line.
:p.:hp4.Major Item Description.:ehp4.The Major Item Description field defines the 
selected major item.  It will be truncated, if necessary, to fit on one line.
:p.:hp4.Base Quantity.:ehp4.The Base Quantity field shows the median value of all 
quantities observed for this major item.
:p.:hp4.(Units).:ehp4.The Units field shows the common unit of measure for the selected 
major item.
:p.:hp4.Quantity.:ehp4.The Quantity field shows the required quantity  of this job 
major item (in common units).  
:p.:hp4.(Quantity Computation Method).:ehp4.The Quantity Computation Method field 
shows the method used to calculate the quantity.  This is calculated by taking 
the log2 of the [difference between the (Major Item Quantity) and the (Price 
Recression Median Quantity)].  The Median Quantity is the median of all 
quantities observed for the selected major item.  
:p.:hp4.Base Unit Price.:ehp4.The Base Unit Price field displays the (statistically 
predicted) unit price for the median quantity.
:p.:hp4.Adj.:ehp4.it Price.   The Adj. Unit Price field displays the base unit price 
multiplied by a quantity adjustment.  This figure is calculated by taking the 
log2 of the [difference between  (Quantity) and the (Median Quantity)].  This 
is then multiplied by the [Quantity Adjustment].
:p.:hp4.(Computation Method).:ehp4.The text in paretheses just below the Adj. Unit 
Price field shows the method that was used to achieve the adjusted unit price.   
:p.:hp4.Unit Price.:ehp4.The Unit Price is calculated by taking the log2 of the 
[difference between the (Quantity) and the (Median Quantity)].  This result 
is then multiplied by the Quantity Adjustment.
:p.:hp4.Qualitative Variable.:ehp4.The Qualitative Variable field shows the identifying 
name of each qualitative variable.  This type of variable attempts to assess 
some of the non-quantifiable issues that could affect the price estimation.  
The quantity adjustment applies to this qualitative variable. 
:p.:hp4.Value.:ehp4.The Value field shows the value of the corresponding variable.  
:p.:hp4.Adjustment.:ehp4.he Adjustment field records the adjustment applied to the 
major item unit price.  
:p.:hp4.(Adjusted Unit Price).:ehp4.This is the (statistically predicted) unit price  
for the median quantity.  

  :h1 res=JobMajItemQuantityRegrS.Job Major Item Quantity Regression Window
:p.The :hp4.Major Item Quantity Regression :ehp4.window displays quantity 
regression data for the selected job major item.  It can be accessed from the 
job breakdown major item window.  No editing can be performed in this window.
:p.To open this window, select the following menu choices and options:
:ul compact.
:li.File
:li.Open Job
:li.Select the desired job.
:li.View
:li.Job Breakdowns
:li.View
:li.View Job Breakdown
:li.Job Major Item List
:li.View
:li.Job Major Item Quantity Regression
:eul.
:p.The Major Item Quantity Regression window displays the following data fields:
:p.:hp4.(Title).:ehp4.The title bar displays the ID numbers for the selected job and 
major item.  
:p.:hp4.Job Breakdown.:ehp4.The Job Breakdown field presents the ID number for the 
selected job breakdown.  
:p.:hp4.Description.:ehp4.The Description field provides a short text description of 
this job breakdown.  It will be truncated, if necessary, to fit on one line.
:p.:hp4.Major Item.:ehp4.The Major Item field defines the selected major item.  It 
will be truncated, if necessary, to fit on one line.  
:p.:hp4.(Length).:ehp4.The length field shows the length of work shown by this 
breakdown (in miles).
:p.:hp4.(Base Coefficient).:ehp4.The Base Coefficient field displays the major item 
quantity regression base coefficient.  A base coefficient is the basic starting 
point for the regression equation.  This is the coefficient before any 
adjustments have been made.
:p.:hp4.(Depth).:ehp4.The Depth field shows the pavement depth for this breakdown in 
inches.
:p.:hp4.(Depth Adj.:ehp4.  This is the  Depth Adjustment for this Regression.
:p.:hp4.(Width).:ehp4.The Width field shows the paving width for this breakdown in 
feet.
:p.:hp4.(Width Adj.:ehp4.  This is the Width Adjustment for this regression .
:p.:hp4.(Cross Section Adj.:ehp4.  This is the Cross Section Adjustment for this 
regression.  
:p.:hp4.(Structure Base).:ehp4.The Structure Base field displays the Structure Base 
Coefficient for this Regression.  This coefficient is before any adjustments 
have been made.
:p.:hp4.(Length).:ehp4.The Length field shows the length of the proposed structure in 
feet.
:p.:hp4.(Structure Length Adj.:ehp4.  The Structure Length Adj. field shows the length 
adjustment to the structure base coefficient.  
:p.:hp4.(Width).:ehp4.The Width field shows the width of the proposed structure in 
feet.
:p.:hp4.(Structure Width Adj.:ehp4.  The Structure Width Adjustment field shows the 
width adjustment to the structure base coefficient for this regression .
:p.:hp4.(Structure Area Adj.:ehp4.  The Structure Area Adjustment field shows the area 
adjustment for the structure base coefficient.  
:p.:hp4.Qualitative Variable.:ehp4.This is the qualitative variable to which this 
adjustment applies.  As above, a qualitative variable allows the user to make 
allowances for non-quantifiable factors that could influence the regression 
estimate.
:p.:hp4.Value.:ehp4.The Value displays the value of the above qualitative variable. 
:p.:hp4.Adjustment.:ehp4.The Adjustment field shows the magnitude of the adjustment 
to apply to the major item quantity.
:p.:hp4.Quantity = .:ehp4.The Quantity = field shows the quantity of this job major 
item (in common units).
:p.:hp4.(Unit).:ehp4.The Unit field shows the common unit of measure for this job 
major item.  

  :h1 res=MajItemPriceRegrS.Major Item Price Regression Window
:p.The :hp4.Major Item Price Regression:ehp4. window displays detailed price regression data for 
a single major item.  Regression is a statistical tool for 
predicting events.  In the CES context it is used to predict prices, after 
all important variables have been taken into account.  Examples of these variables 
could include weather, inflation, size of contractor firm, etc.  The hallmark of 
regression is its ability to factor otherwise non-quantifiable (qualitative) 
variables into the prediction process.   

:p.To open this window, select the following menu choices and options:
:ul compact.
:li.File
:li.Open Catalog
:li.Major Item Catalog
:li.Select a Major Item
:li.View
:li.Major Item Price Regression
:eul.
:p.The Major Item Price Regression window displays the following data fields:
:p.:hp4.(Title).:ehp4.The title bar shows ID for the selected major item. 
:p.:hp4.Description.:ehp4.The Description field defines the selected major item.
:p.:hp4.Median Quantity.:ehp4.The Median Quantity field displays the median (middle) of 
all quantities observed for this major item.
:p.:hp4.(Units).:ehp4.The Units field displays the common unit of measure for quantities 
of this major item.
:p.:hp4.Base Unit Price.:ehp4.The Base Unit Price field shows the (statistically predicted) 
unit price for the median quantity.
:p.:hp4.Base unit.:ehp4.  The Base Unit... field shows the percentage that should be added 
to the unit price every time the quantity is doubled.  This should usually be 
negative, as price usually decreases as quantity increases.  
:p.:hp4.Qualitative Variable.:ehp4.The Qualitative Variable field shows the list of qualitative 
variables for this regression.  A qualitative variable is an attempt to capture non-quantifiable influences on price in the 
regression to improve its accuracy.
:p.:hp4.Qualitative Adj.:ehp4.or.   The Qualitative Adj...for field displays the Qualitative 
Variable selected from the previous list box.  The list box below this (Qualitative 
Adj for) field displays the adjustments for the selected qualitative variable.
:p.:hp4.Value.:ehp4.The Value field shows the value of the qualitative variable selected.  
:p.:hp4.Adjustment.:ehp4.The Adjustment field shows the percentage adjustment that will be 
applied to the major item unit price.  

  :h1 res=MajItemQuantityRegrS.Major Item Quantity Regression Window 
:p.The :hp4.Major Item Quantity Regression:ehp4. window displays quantity regression data for 
a single major item.  No editing can be done in this window.  This regression 
functions in a similar manner to the major item price Regression just previous.  
The difference is that the quantity regression predicts quantities rather than 
prices.
:p.To open this window, select the following menu choices and options:
:ul compact.
:li. File
:li.Open Catalog
:li.Major Item Catalog
:li.Select a Major Item
:li.View
:li.Major Item Quantity Regression
:eul.
:p.The Major Item Quantity Regression window displays the following data fields:
:p.:hp4.(Title).:ehp4.The title bar displays the ID for the selected major item.  
:p.:hp4.Description.:ehp4.The Description field defines the selected major item.  
:p.:hp4.(Base Coefficient).:ehp4.This field shows the Base Coefficient 
for this Major Item Quantity Regression.  A base coefficient is a base price with 
no adjustments made for depth, width, etc.  This is the starting point for the 
regression.
:p.The regression adjusts the base coefficient on three variables for 
pavement and three for structures, as well as on several qualitative 
variables.  These combine to improve regression accuracy by taking all 
the factors (that influence price) into account.
:p.:hp4.(Depth Adj.:ehp4. This field adjusts for the depth of the pavement. 
:p.:hp4.(Width Adj.:ehp4.  This field adjusts for the width of the proposed pavement 
section.
:p.:hp4.(Cross Section Adj.:ehp4.  This field adjusts 
for the cross section of the pavement section.
:p.:hp4.(Structure Base Coefficient).:ehp4.This field shows the base coefficient for 
a structure (see above for a detailed description of a base coefficient).   
This base coefficient is adjusted on the length, width, and area of the proposed 
structure.
:p.:hp4.(Structure Length Adj.:ehp4.  This field shows the adjustment the regression will 
make for the length of the proposed structure.
:p.:hp4.(Structure Width Adj.:ehp4.  This field shows the adjustment the regression will 
make depending on the width of the structure.
:p.:hp4.(Structure Area Adj.:ehp4.  This field shows the adjustment the regression will 
make depending on the area of the structure.
:p.:hp4.Qualitative Variable.:ehp4.The Qualitative Variable field displays the qualitative 
variables for this quantity regression.
:p.:hp4.Qualitative Adj.:ehp4.or.   The Qualitative Adj...for field displays the currently 
selected qualitative variable.  The list box below this field displays the 
adjustments for the selected qualitative variable.
:p.:hp4.Value.:ehp4.The Value field shows the value to which this adjustment applies. 
:p.:hp4.Adjustment.:ehp4.The Adjustment field shows the adjustment that the regression 
will apply to the major item quantity.


  :h1 res=StandardItemCatS.Standard Item Catalog
:p.The :hp4.Standard Item Catalog:ehp4. window is a list of the standard
construction items (biddable units of work) allowed by BAMS  for use in jobs, 
projects, and proposals.  
:p.To open this window, select the following menu choices and options:
:ul compact.
:li.File
:li.Open Catalog
:li.Standard Item Catalog
:eul.
:p.The Standard Item Catalog window displays the following data fields:
:p.:hp4.Item Number.:ehp4.The Item Number field contains the (unique) standard item number 
for each item in the catalog.  
:p.:hp4.Short Description.:ehp4.The Short Description field presents a cursory definition 
of each standard item. 
:p.:hp4.Unit.:ehp4.The Unit field displays the standard unit of measure for the standard 
item.  The unit will appear in parentheses if the item is a lump sum item.

  :h1 res=StandardItemS.Standard Item Window
:p.The :hp4.Standard Item:ehp4. window displays information about the specific standard 
item indicated in the title bar.  This window performs several functions.  
First, it indicates a list of standard items that must precede or succeed the 
specified (selected) standard item and provides information as to the type of 
dependency and the number of days that must elapse between the dependent items.  
Second, it  provides a list of major items that utilize this standard item.
 Note that this major item list is not editable.
:p.To open this window, select the following menu choices and options:
:ul compact.
:li.File 
:li.Open Catalog
:li.Standard Item Catalog
:li.Select a Standard Item and click on it to view the Standard Item window.
:eul.
:p.The Standard Item window displays the following data fields:
:p.:hp4.(Window Title).:ehp4.The title bar displays the number of the standard 
item being displayed.
:p.:hp4.Item Number.:ehp4.The Item Number field shows the Standard Item Number 
of the selected item.
:p.:hp4.(Lump Sum).:ehp4.The Lump Sum Item field displays whether or not this 
item is a Lump Sum item.  A lump sum item is one that is measured in its 
entirety rather than by units.
:p.:hp4.Unit.:ehp4.The Unit field shows the unit of measure for the standard item.  
If the item is a lump sum or hybrid lump sum item, the unit will be followed 
by (LS) or (Hybrid LS), respectively. 
:p.:hp4.Default Production.:ehp4.The Default Production field shows the default 
(expected) number of units produced per day of work.  The program will use 
this number unless another is specified.
:p.:hp4.Default Start Time.:ehp4.The Default Start Time field 
shows the percentage of the job that must be complete before this item of 
work will begin.
:p.:hp4.Rounding Precision.:ehp4.The Rounding Precision field displays the rounding 
precision in use.  This can be specified by the user.
:p.:hp4.Spec Year.:ehp4.The Spec Year field shows the year in which this standard 
item was specified.
:p.:hp4.Measurement Units.:ehp4.The Measurement Units field shows the unit 
system of the Standard Item - 'N' for Neither or both, 'E' for English, 
and 'M' for Metric.
:p.:hp4.Predecessors.:ehp4.The Predecessors list box shows the standard items that 
must precede the selected standard item.  It contains values from three data 
fields:  The leftmost value is the number of a standard item that precedes 
the selected standard item.  The center value identifies the dependency between 
the items (SF = Start to Finish; SS = Start to Start; FS = Finish to Start; 
FF = Finish to Finish).  The rightmost value indicates the minimum number of 
days  that must elapse between the items.  
:p.(See the Introduction Chapter for a definition of dependency relationships.) 
:p.:hp4.Successors.:ehp4.The Successors list box shows the standard items that must 
succeed (come after) the selected item.  It contains values from three data 
fields:  The leftmost value is the number of a standard item that succeeds 
the selected standard item.  The center value identifies the dependency 
between the items  (SF = Start to Finish; SS = Start to Start; FS = Finish to 
Start; FF = Finish to Finish).  The rightmost value indicates the minimum number 
of days  that must elapse between the items.  
:p.(See the Introduction Chapter for a definition of dependency relationships.) 
:p.:hp4.Short Description.:ehp4.The Short Description field defines the standard item.
:p.:hp4.Long Description.:ehp4.The Long Description field provdes a detailed description 
of the standard item.
:p.:hp4.Major Item.:ehp4.The Major Item field shows the major item that relates to the 
selected standard item.
:p.:hp4.Common Unit.:ehp4.The Common Unit field shows the common unit of measure for 
the major item.
:p.:hp4.Description.:ehp4.The Description field defines this major item.

  :h1 res=UserCatS.User Catalog
:p.This is the help text for User Catalog more to come later.

  :h1 res=VendorCatS.Vendor Catalog
:p.The :hp4.Vendor Catalog:ehp4. window displays a list of all vendors used by 
BAMS/CES.  
:p.To open this window, select the following menu choices and options:
:ul compact.
:li.File
:li.Open Catalog
:li.Vendor Catalog  
:eul.
:p.The Vendor Catalog window displays the following data fields:
:p.:hp4.Vendor.:ehp4.The Vendor field defines each vendor in the catalog.  
:p.:hp4.Short Name.:ehp4.The Short Name field shows an abbreviated version of each vendor 
name.
:p.:hp4.Type.:ehp4.The Type field shows the code number for each vendor. 
:p.:hp4.Class.:ehp4.The Class field shows the kind of certification held by this vendor.  
:p.:hp4.DBE.:ehp4.The DBE field displays the DBE classification for a vendor, if applicable.  

     :h1 res=VendorS.Vendor Window
:p.The :hp4.Vendor Window:ehp4. displays information about a particular vendor.  This 
includes name, type of vendor, certification, and DBE class as well as full 
address and phone number.  (The information about the vendor is maintained in 
the Vendor Catalog.) 
:p.To open this window, select the following menu choices and options:
:ul compact.
:li.File
:li.Open Catalog
:li.Vendor Catalog
:li.Double click on or select a particular vendor.
:eul.
:p.The Vendor window displays the following data fields:
:p.:hp4.(Title).:ehp4.The title bar identifies the selected vendor.  
:p.:hp4.Vendor.:ehp4.The Vendor field displays a short, unique identifier for the 
selected vendor.
:p.:hp4.Short Name.:ehp4.The Short Name field shows an abbreviation of the vendor's 
name.  
:p.:hp4.Name.:ehp4.The Name field shows the complete name of the vendor.  
:p.:hp4.Type.:ehp4.The Type field shows the type of vendor.  
:p.:hp4.Certification.:ehp4.The Certification field displays the type of certification 
held by the vendor.  
:p.:hp4.DBE Class.:ehp4.The DBE Class field provides the DBE classification for the 
vendor, if applicable.  
:p.:hp4.Work Class.:ehp4.The Work Class box lists classification codes for the vendor 
on the left side and the translation in the right-hand column.
:p.:hp4.Vendor Address.:ehp4.The Vendor Address field shows the street address of the 
vendor.  
:p.:hp4.City.:ehp4.The City field shows the city that goes with the preceding street 
address.
:p.:hp4.State.:ehp4.The State field shows the state for the preceding street address.
:p.:hp4.Zip Code.:ehp4.The Zip Code field shows the zip code for the preceding street 
address.  
:p.:hp4.Phone Number.:ehp4.The Phone Number field shows the telephone number for the 
preceding street address.  

  :h1 res=WorkingDayCalendarCatS.Working Day Calendar Catalog
:p.The :hp4.Working Day Calendar Catalog:ehp4. window displays a list of working 
day calendars and describes situations for which these calendars apply.
(A working day calendar shows the number of working days in a month, as opposed 
to the total number of days in a month.)  
:p.To open this window, select the following menu choices and options:
:ul compact.
:li.File 
:li.Open Catalog 
:li.Working Day Calendar Catalog
:eul.
:p.The Working Day Calendar Catalog window displays the following data fields:
:p.:hp4.Working Day Calendar.:ehp4.The Working Day Calendar field shows the ID for each 
working day calendar. 
:p.:hp4.Description.:ehp4.The description field defines each working day calendar. 

  :h1 res=WorkingDayCalendarS.Working Day Calendar Window
:p.The :hp4.Working Day Calendar :ehp4.window displays information about a 
specific working day calendar.  A working day calendar shows the days in a month 
that the construction crews are expected to work.  This will be different from 
a normal calendar because it distinguishes between work and non-work days.  
:p.To open this window, select the following menu choices and options:
:ul compact.
:li.File 
:li.Open Catalog
:li.Working Day Calendar Catalog
:li.Double click on or select one particular calendar.  The program will display 
the working day calendar window for that calendar.
:eul.
:p.The Working Day Calendar window displays the following data fields:
:p.:hp4.(Title).:ehp4.The title bar displays ID for the selected working day calendar.  
:p.:hp4.Month.:ehp4.The Month field indicates the month covered by this prediction.
:p.:hp4.Year.:ehp4.The Year field shows the year of this prediction.
:p.:hp4.Working Days.:ehp4.The Working Days field shows the expected number of working 
days in the stated month.
:p.:hp4.Calendar Days.:ehp4.The Calendar Days field shows the actual number of days in 
each month.


  :h1 res=ZoneTypeCatS.Zone Type Catalog
:p.The :hp4.Zone Type Catalog:ehp4. window can displays a list of all zone types 
used by BAMS/CES.  A zone is an area of the state that does not depend on county 
boundaries.  
:p.To open the Zone Type Catalog window, select the following menu choices and 
options:
:ul compact.
:li.File 
:li.Open Catalog
:li.Zone Type Catalog
:eul.
:p.The Zone Type Catalog window displays the following data fields:
:p.:hp4.Zone Type.:ehp4.The Zone Type field provides a short name (maximum 16 characters) 
used to uniquely identify each zone type.
:p.:hp4.Description.:ehp4.The Description field defines each zone type. 

  :h1 res=ZonesS.Zone Window
:p.The :hp4.Zone Window:ehp4. displays information about zones of a particular type in the Zone Catalog.  A zone is an area of the state that does not depend on county boundaries.
:p.To open this window, select the following menu choices and options:
:ul compact.
:li.File 
:li.Open Catalog
:li.Zone Type Catalog
:li.Double click on or select a particular zone type.  The program will display the Zone window, which lists the zones in that zone type.
:eul.
:p.The Zone window displays the following data fields:
:p.:hp4.(Title).:ehp4.The title bar displays the ID for the selected zone type.  This is a unique identifier, for example, "Carpenter" or "Heavy Equipment Operator" for this zone type.  
:p.:hp4.Zone.:ehp4.The Zone field defines each zone within the selected zone type.  
:p.:hp4.Description.:ehp4.The Description field provides a description for each zone.  
:p.:hp4.Map Name.:ehp4.The Map Name field defines the map of each zone.  

  :h1 res=AreaTypeCatD.Add/Change Area Type 
:p.To add or change areas, perform the following steps:
:ul compact.
:li.Select the area desired.
:li.Press the insert key to add and Control C to Change or click on Edit, Add/Change.  The program will display the dialog box with 
three fields--Area ID, Description, and a Counties Table.  
:li.Fill in the three fields and hit the enter key or click on OK to accept the 
additions.
To change the list of counties in the current area, either to add new ones or 
delete old ones, perform the following steps:
:li.Select the county desired. 
:li.Fill in the data required. 
:li.Select More to add more and save those already added, or OK to quit back to 
the area window.  
:eul.

  :h1 res=AreaTypeD1.Add/Change Area 
:p.To add or change areas, perform the following steps:
:ul compact.
:li.Select the area desired.
:li.Press the insert key to add and Control C to Change or click on Edit, Add/Change.  The program will display the dialog box with 
three fields--Area ID, Description, and a Counties Table.  
:li.Fill in the three fields and hit the enter key or click on OK to accept the 
additions.
:eul.
:p.To change the list of counties in the current area, either to add new ones or 
delete old ones, perform the following steps:
:ul compact.
:li.Select the county desired. 
:li.Fill in the data required. 
:li.Select More to add more and save those already added, or OK to quit back to 
the area window.  
:eul.

  :h1 res=AreaTypeD2.areatyped2
      :p.To enter additional area types, perform the following steps:
:ul compact.
:li.Make the Area Type window active by clicking on it (or selecting it).
:li.Hit the insert key, or perform the Edit, Add command sequence.  CES will 
display the Add Area Type dialog box.
:li.Fill in the Area Type and Description fields.  
:li.Accept the entry by hitting enter or selecting the OK button.  To cancel 
the operation entirely, press escape or click on the cancel button.  To add 
more than one area type, click on Add More or use the Alt + M combination. 
:eul.

  :h1 res=rAreas_RptDlgBox.Area Listing Report
:p.To print the :hp4.Area Listing Report:ehp4., perform the following steps:
:p.Open the Area Type Catalog and perform the print sequence.
Select the Area Listing report.

  :h1 res=rSIAvg_RptDlgBox.Regression Standard Item Average Listing Report
      :p.This is the help text for rsiavg_rptdlgbox

  :h1 res=OpenJobD.Open Job Window
:p.:p.The :hp4.Open Job:ehp4. command opens a window listing all the Jobs 
currently in the database. If no jobs exist, a skeleton screen will appear.
:p.From here, the user can cancel, ask for help, or select a job and hit OK. 
All of the list boxes provided by CES will automatically highlight (select) 
the first item in the list.  Therefore, if you select OK without choosing 
some other item, the first item will be chosen.
:p.If you want to delete an old job, select the job in the list, and press 
the Delete button.  If you want to copy an existing job to a new job, select
the job in the list, and press the copy button.


  :h1 res=OpenProgramD.Open Program Window
:p.:p.The :hp4.Open Program:ehp4. command displays a list of the existing road 
construction programs.  If no programs have been defined, CES will display 
a skeleton Open Program box.
Once at this window, you can request help, select a program and hit enter 
or OK, or cancel the Open Program operation.
:p.The parent/child window hierarchy in BAMS/CES is not strict.  
Therefore, if you began your work session by opening the Program window, 
you can move to one of the job-related windows by viewing the jobs in 
this program, selecting one of them, and then proceeding as if you had 
begun with the Open Job sequence.  
:p.If you want to delete an old program, select the program in the list, and press 
the Delete button.  


  :h1 res=JobItemD.Add Job Item Window
:p.The :hp4.Job Item:ehp4. window provides detailed information about the selected job item.  
This window can be accessed through the Job Items window.  
:p.To open this window, select the following menu choices and options:  
:ul compact.
:li.File 
:li.Open Job 
:li.Choose a Job  
:li.View 
:li.Job breakdowns--This will display a list of the job breakdowns for this job.
:li.Select a job breakdown.
:li.View (2nd time)
:li.Job Breakdown  
:li.View (3rd time)
:li.Job Items--This will display the Items for Job Breakdown x, job y.  
:li.Select the desired item,
:li.View (4th time)
:li.Job Item--This will display the Job Item window. 
:eul.
The Job Item window displays the following data fields:
:p.:hp4.(Title).:ehp4.The title bar displays the Job ID and the Standard Item Number of the 
selected job item.  
:p.:hp4.Item Number.:ehp4.The Item Number field shows the item number of the selected job 
item.
:p.:hp4.Quantity.:ehp4.The Quantity field presents the necessary quantity of the selected 
job item.
:p.:hp4.Unit.:ehp4.The Unit field shows the common unit of measure for the item.  
:p.:hp4.(Hybrid LS).:ehp4.The Hybrid LS label is 
displayed if the unit is a hybrid lump sum.  The term hybrid lump sum 
is used to define a normally unit-priced item that is bid 
as a  single quantity (lump sum).   For instance, often a job such as excavation 
may require a predictable number of units of some material.  However, the 
government will ask for a bid for the entire job (excavation) to insulate 
them from the cost of paying for additional units of the material at a later 
date.  Thus, the material in question becomes a hybrid lump sum.  It is normally 
bid by unit, but for this job it is treated as a lump sum.
:p.:hp4.Unit Price.:ehp4.The Unit Price field shows the estimated price of one unit of the 
job item.
:p.:hp4.Extension.:ehp4.The Extension field shows the the estimated total cost for this job 
item.  It is equal to the quantity times the unit price.
:p.:hp4.Estimated Using.:ehp4.The Estimated Using field gives the ID number for the estimation 
method used.  
:p.:hp4.Estimation Comment.:ehp4.The Estimation Comment field records a comment  about the 
origin of the estimated unit price.  
:p.:hp4.Short Description.:ehp4.The Short Description field defines the item of work presented 
in this field.
:p.:hp4.Long Description.:ehp4.The Long Description field provides a detailed description of 
the selected item.
:p.:hp4.Suppl.:ehp4.scription.   The Supplemental Description field allows the estimator to 
record any additional description necessary.
:p.:hp4.Start Time.:ehp4.This is the time lag, in days, between the letting date and the 
expected start date for this item.[CHECK] 
:p.:hp4.Rounding Precision.:ehp4.This field shows the rounding precision for this item.  
This is chosen by the estimator.
:p.:hp4.Duration.:ehp4.The Duration field shows the expected time necessary to complete 
an item, in days. 
:p.:hp4.Dependency.:ehp4. A dependency relationship states which actions must come before 
other actions and also states which ones must wait for others to start.  These 
are read...if a, then b ...   For example, a start to finish (SF) dependency for 
A and B would be read, if A starts, then B can finish.  In other words, B cannot 
finish until A has started.  
:p.:hp4.Predecessors.:ehp4.The Predecessors list box shows which items 
must precede the selected job item.  It contains values from three data fields.  
The leftmost value is the  number of a standard item that must precede the selected 
job item.  The center value identifies the dependency between the items (SF = Start 
to Finish; SS = Start to Start; FS = Finish to Start; FF = Finish to Finish).  The 
rightmost value indicates the minimum number of days that must elapse between the 
items. (See the Introduction Chapter for a definition of dependency relationships.) 
:p.:hp4.Successors.:ehp4.The Successors list box shows which items must 
succeed (come after) the selected job item.  It contains values from three data 
fields.  The leftmost value is the number of a standard item that must succeed 
the selected job item.  The center value identifies the dependency between the 
items (SF = Start to Finish; SS = Start to Start; FS = Finish to Start; FF = 
Finish to Finish).  The rightmost value indicates the minimum number of days 
that must elapse between the items.  (See the Introduction Chapter for a definition 
of dependency relationships.) 

  :h1 res=JobD.Change Job 
:p.The :hp4.Change Job:ehp4. dialog box.
:p.To make changes in this job, perform the following steps: 
:ul compact.
:li.Perform the Edit, Change command sequence.  CES will display a dialog box with 
a field for each piece of information in the job window.  Note:  This field has 
many combination boxes (designated by arrows in the right-hand end of the box).  
These boxes will not accept user-typed data--the input must come from the 
combination box list. 
:li.For keyboard users, do not hit enter when you select a line in a combination 
box.  This will cause the program to enter the incomplete data in the combination 
box and return you to the window you were modifying.  You will then have to go 
through the Edit, Change sequence again.  Instead, use the tab and arrow keys 
to move through the dialog box fields.
:eul.


  :h1 res=JobAltGroupD.Add/Change Job Alternate Group 
:p.To add or change :hp4.Job Alternate Groups,:ehp4. perform the following steps:
:ul compact.
:li.Activate the Job Alternate Groups window.
:li.Hit insert to add and Control C to Change or click on Edit, Add/Change.  CES will display the dialog box pictured above.
:li.Fill in the two data entry boxes.
:li.Select one of the four choice buttons (OK, Cancel, Help, or Add More) to perform 
the desired function.  Selecting Add More will enter the values currently in the 
dialog box and clear the box for the upcoming entry.
:eul.

  :h1 res=JobAltD.Add/Change Job Alternate 
:p.To Add or change :hp4.Job Alternate:ehp4. perform the following steps:
:ul compact.
:li.Activate the Job Alternate Group window.
:li.Hit insert to add and Control C to Change or click on Edit, Add/Change.  CES will display the dialog box.
:li.Fill in the two data entry boxes.
:li.Select one of the four choice buttons (OK, Cancel, Help, or Add More) to perform 
the desired function.  Selecting Add More will enter the values currently in the 
dialog box and clear the box for the next entry.
:eul.

  :h1 res=JobCommentD.Add/Change Job Comment 
:p.To add or change :hp4.Job Comments:ehp4. perform the following steps:
:ul compact.
:li.Activate the Job Comments window.
:li.Hit the insert key to add and Control C to Change or click on Edit, Add/Change.  CES will display the dialog box pictured 
above. 
:li.Type in the comment, then choose one of the four choice buttons (OK, Cancel, Help, 
or Add More) to perform the desired function.  If you choose Add More, the data 
currently in the dialog will be entered into the database, and the box will be 
cleared for the new data.
:eul.

  :h1 res=JobBreakdownD.Add/Change Job Breakdown
:p.To add or change a :hp4.Job Breakdown:ehp4., perform the following steps:
:ul compact.
:li.Activate the Job Breakdown window.
:li.Hit the insert key to add and Control C to Change or click on Edit, Add/Change. 
:li.Fill in the eight plain boxes (job ID, length, width, depth, and structure ID, 
length ande width, and description) and the three combination boxes (road, structure 
and work type).
:li.Length is in miles, pavement width is in feet, and pavement depth is in inches.  
The structure ID can be longer (approximately 12 characters) than the job ID 
(4 characters maximum).  The structure length is in miles; the structure width 
is in feet. 
:li. When you are finished entering data, select one of the four choice buttons  
(OK, Cancel, Help, or Add More) to perform the desired action.  The Add More 
command will enter the current entries and clear the boxes for upcoming entries.
When you make a selection from a combination box, use the tab key to move to the 
next box.  If you hit enter, the program automatically accepts all data entered 
up to this point and returns you to the Job Breakdown screen.
There is also a box that allows you to choose whether or not you want a detailed 
estimate.  A non-detailed estimate would use parametric estimation.  If there are 
job items, the program will use a more specific type of estimation method.
If the job breakdown is for a structure (such as a bridge), the Job Breakdown 
Structure window will be visible.  If the job is for something other than a 
structure (such as a road or an excavation), the Non-Structure Job Breakdown 
window will be visible.  
:eul.

  :h1 res=DefJobUsersD.addchdefjobuserseditdlg
      :p.This is the help text for addchdefjobuserseditdlg

  :h1 res=DefProgUsersD.addchdefproguserseditdlg
      :p.This is the help text for addchdefproguserseditdlg

  :h1 res=ChJobUserD.chjobusereditdlg
      :p.This is the help text for chjobusereditdlg

  :h1 res=AddJobUserD.Add Job User
:p.To add :hp4.New Users:ehp4., perform the following steps:
:ul compact.
:li.Activate the Job Users window.
:li.Hit the insert key or click on Edit, Add.  CES will display the dialog box.
:li.Type the User ID in the first field and select from the access privilege 
list provided in the combination box.
:li. Select one of the four choice buttons (OK, Cancel, Help, or Add More) to 
perform the desired action. 
:li.To change any of these numbers, use the Edit, Change command to reopen a 
closed box. 
:eul.

  :h1 res=JobNewD.Add Job Window
:p.The :hp4.Add Job :ehp4.window creates a new Job.
:ul compact.
:li.Fill in the plain entry fields by manually typing the data in.
:li.The Location field must be expressed in terms of latitude and longitude 
(degrees, minutes, and seconds) and the Construction and Contingency percents
must be entered as percent values--for 8 percent, type either .08 or 8%.
:li.For the seven combination boxes (Checked by, Approved by, Estimator, Work 
Type, Work Day Calendar, Letting Month, and District), choose from the lists 
provided  These fields will not accept typed-in data.
:li.The small square next to the Location box specifies whether or not 
Davis-Bacon wages apply  Click on this box to toggle its display  An X in the 
box means the condition is positive (checked) and therefore Davis-Bacon wages 
would apply.
:li.At this point you can accept the job, go back and make any changes necessary 
by choosing the box that needs correction, or cancel the entire entry  You can 
use the Help command at any stage in the process to get help with the current
menu or dialog box.
:eul.

  :h1 res=ProgramD.Add/Change Program Window
      :p.This is the help text for Add/Change Program Window

  :h1 res=ChProgJobD.chprogjobeditdlg
      :p.This is the help text for chprogjobeditdlg

  :h1 res=AddProgJobD.Add/Change Jobs for Programs
:p.To add or change :hp4.Jobs for Programs:ehp4. perform the following steps:
:ul compact.
:li.Activate the Program Job List window.
:li.Hit the insert key to add and Control C to Change or click on Edit, Add/Change
:li.The Letting Date, Work Begin Date, and Work End Date fields require dates. 
The program recognizes most date formatting options.  For instance, you can 
enter 1/1/90 or January 1, 1990. 
:li.The Priority field requires a number that represents priority level.  For 
example, 1 would represent first priority.
:li.Choose an inflation prediction from the list provided by the combination 
box.  (If you wish a different inflation prediction scenario, you may add it 
in the Inflation Prediction Catalog.) 
:li.Select one of the four choice buttons (OK, Cancel, Help, or Add More) to 
perform the desired action.  Selecting the Add More option enters the current 
data and clears the screen for new data.
:li.To change any of the data in the jobs, select the job you wish to update, 
then execute the Edit, Change command sequence.  The program will display the 
dialog box for that job.  Then make and accept changes the same way you would 
for entering new data.
:eul.

  :h1 res=AddProgUserD.Add Program User
:p.To add :hp4.New Users:ehp4., perform the following steps:
:ul compact.
:li.Make the Program User List window active,
:li.Hit the insert key or execute the Edit, Add command sequence.  CES will display 
the dialog box.
:li.Type in the user ID and choose the Access Privilege from the combination box.
:li.Select one of the four choice buttons (OK, Cancel, Help, or Add More) to perform 
the desired action.  Selecting the Add More button enters the current data and clears 
the screen for new data. 
:eul.

  :h1 res=ChProgUserD.Change Program User
:p.To change :hp4.New Users:ehp4., perform the following steps:
:ul compact.
:li.Make the Program User List window active,
:li.Press Control C or execute the Edit, Change command sequence.  CES will display 
the dialog box.
:li.Type in the user ID and choose the Access Privilege from the combination box.
:li.Select one of the four choice buttons (OK, Cancel, Help) to perform 
the desired action.   
:eul.

  :h1 res=ChJobBreakFundD.Change Job Breakdown Profile Window
:p.To change :hp4.Job Breakdown Profile:ehp4. perform the following steps:
:ul compact.
:li.Activate the Job Breakdown Profile window.
:li.Perform the Edit, Change command sequence.  CES will display the dialog box. 
:li.Enter the values you wish to change in the dialog box.  Remember that percentages 
must be entered in decimal form or whole number form with a percent sign 
attached.
:li.Choose from one of the four choice buttons (OK, Cancel or Help) 
to perform the desired action.
:eul.
  
   :h1 res=AddJobBreakFundD.Add Job Breakdown Profile Window
:p.To add :hp4.Job Breakdown Profiles:ehp4., perform the following steps:
:ul compact.
:li.Activate the Job Breakdown Profile window.
:li.Perform the Edit, Add command sequence.  CES will display the dialog box. 
:li.Enter the values requested by the dialog box.  Remember that percentages 
must be entered in decimal form or whole number form with a percent sign 
attached.
:li.Choose from one of the four choice buttons (OK, Cancel, Add More, or Help) 
to perform the desired action.
:eul.
  
   :h1 res=ItemSchedD.Change Item Schedule
:p.To Change :hp4.Item Schedule:ehp4., perform the following steps: 
:ul compact.
:li.Activate the Job Item Schedule window.
:li.Press control C or click on Edit, Change.  This displays the dialog box.  
:li.As shown, the dialog box presents the Standard Item Catalog.  Select from this 
list to insert the values in the the Item Number, Description, and Unit fields. 
:li.The six plain boxes (Quantity, Unit Price, Supplemental Description, Duration, 
Delay, and Estimation Comment) require typed entry.  Duration and Delay are stated 
in days.  Select from the lists provided for the three combination boxes 
(Estimation Method, Alternate Group, and Alternate).
:li.Select from the four choice buttons to accept, cancel, or accept.
:eul.

  :h1 res=PredecessorsD.Add Predecessors Job Item
:p.To add :hp4.Items to the Schedule:ehp4., perform the following steps: 
:ul compact.
:li.Activate the Job Item Schedule window.
:li.Press the insert key or click on Edit, Add.  This displays the dialog box.  
:li.As shown, the dialog box presents the Standard Item Catalog.  Select from this 
list to insert the values in the the Item Number, Description, and Unit fields. 
:li.The six plain boxes (Quantity, Unit Price, Supplemental Description, Duration, 
Delay, and Estimation Comment) require typed entry.  Duration and Delay are stated 
in days.  Select from the lists provided for the three combination boxes 
(Estimation Method, Alternate Group, and Alternate).
:li.Select from the four choice buttons to accept, cancel, or accept and add 
more data.
:eul.

  :h1 res=SuccessorsD.successorseditdlg
      :p.This is the help text for successorseditdlg

  :h1 res=DefltJobProgUsersD.defltjobproguserseditdlg
      :p.This is the help text for defltjobproguserseditdlg

  :h1 res=OpenProposalD.Open Proposal Window
:p.The :hp4.Open Proposal :ehp4.window displays the proposal for a job. 
:p.To open the Open Proposal window, perform the following steps:
:ul compact.
:li. File 
:li. Open Job 
:li. Select a job by double clicking on the job or highlighting and pressing enter.  
:li. View 
:li. Proposal  
:li. select a proposal
:li. 
:eul.
:p.The Open Proposal window displays the following data fields:


  :h1 res=OpenProjectD.Open Project Window
:p.The :hp4.Open Project :ehp4.window displays a list of all the Projects for a job. 
:p.To open the Open Project window, perform the following steps:
:ul compact.
:li. File 
:li. Open Job 
:li. Select a job by double clicking on the job or highlighting and pressing enter.  
:li. View 
:li. Project  
:li. select a project
:li. 
:eul.
:p.The Project window displays the following data fields:


  :h1 res=RBASED_RptDlgBox.Base Date Listing Report
:p.The :hp4.Base Date Listing Report:ehp4. report can be printed from all the catalogs and several other windows. 
:p.To print the Base Date Listing Report, perform the following steps:
:p.Perform the print sequence and select this report.   There are no variations 
possible with this report, therefore, the only choices offered by the dialog box 
are to get help, cancel the print, or complete the print job.

  :h1 res=rJECmt_RptDlgBox.rjobcmt_rptdlgbox
      :p.This is the help text for rjobcmt_rptdlgbox

  :h1 res=rJobEst_RptDlgBox.Job Estimate Listing Report
:p.To print the :hp4.Job Estimate Report:ehp4., perform the following steps:
:p.Open the Job Window.
Perform the print sequence.
CES provides you with a list of reports available at this time.  You will have 
the option to print three attachments to the Job Estimate reports--the 
Cost-Based Estimate, the PEMETH Averages report, and the PEMETH Regression report.  
The program will print the default report requested unless you cancel.  However, 
you can tailor the presentation of this report with this box.  In this case, the 
first choice (unchecked) says it will print the long item descriptions by 
default.  You can select to print only the short descriptions if the complete 
descriptions are unimportant for this print job.  The short description will be 
the beginning text of the long description truncated to a standard size. Also 
printable from this window will be the cost-based estimate, PEMETH averages, 
PEMETH regression. The second choice allows you to include the comments made 
about the various levels of this job, presented in a comment log.  The default 
(unchecked) report will not include these comments. 
 
  :h1 res=rJobFund_RptDlgBox.Job Funding Report
:p.To print the :hp4.Job Funding Report:ehp4., perform the following steps:
:p.Open the Job Funds window.  
Perform the print sequence and select the Job Funding report.  CES prints only 
one version of this report; therefore, the only choices are to get help, print 
the job, or cancel the job. 

  :h1 res=rJobSch_RptDlgBox.Job Item Scheduling Report
:p.To print the :hp4.Job Item Schedule Report:ehp4., perform the following steps:
:p.Open the Job Item Schedule window.  The path of windows is as follows:  Job, 
Job Breakdowns, Job Breakdown, Job Items, Job Item, Job Item Schedule. (See 
Chapter 5 for details or the appendix for a flowchart path to this window.)
Select the Job Item Schedule.  The default report includes all job items, but 
you can select to print only the scheduled items or only the critical path items.   
You can also choose the order in which the list is sorted and the criteria by 
which items will be selected to print.  The choices in each rectangle are 
mutually exclusive, that is, you cannot choose more than one choice per rectangle 
per print job.  After making the selections, either print or cancel the print job.

  :h1 res=rPrgEst_RptDlgBox.Program Estimate Listing Report
:p.To print the :hp4.Program Estimate Report:ehp4., perform the following steps:
:p.Open a Program.
Perform the print sequence.
Select the Program Estimate Report.  Choose one choice per rectangle to select 
the variable to sort on, the order to list the subtotals, and whether or not to 
inflate (account for inflation) to a specific date.  If you choose to inflate 
to a specific date, you must click on the Use button before printing, or the 
program will disregard the date you entered.  If the date you enter is invalid, 
the program will provide an error message.  The default date for inflation 
predictions will usually be the letting date.  

  :h1 res=rPrgFnd_RptDlgBox.Program Funding Listing Report
:p.To print the :hp4.Program Estimate Report:ehp4., perform the following steps:
:p.Open a Program.
Perform the print sequence.
Select the Program Estimate Report.  Choose one choice per rectangle to select 
the variable to sort on, the order to list the subtotals, and whether or not to 
inflate (account for inflation) to a specific date.  If you choose to inflate to 
a specific date, you must click on the Use button before printing, or the 
program will disregard the date you entered.  If the date you enter is invalid, 
the program will provide an error message.  The default date for inflation 
predictions will usually be the letting date.  

  :h1 res=rPrgSch_RptDlgBox.Program Schedule Listing Report
:p.To print the :hp4.Program Schedule Listing Report:ehp4., perform the following steps:
:p.Open the Program window.
Perform the print sequence and select the Program Schedule Report.  Click on the 
Inflate to Letting Date box to toggle between the default and specified date.  
An X in the box (the checked condition) causes the program to inflate to the 
letting date.  The default estimation for this window is in constant dollars.  
Choose the order of presentation for the program jobs by selecting one of the 
choices in the first rectangle. Choose the jobs to be listed by selecting the 
option that applies in the second rectangle.  

  :h1 res=CodeTableD.Add/Change Code Table 
      :p.This is the help text for code_table_editdlg

  :h1 res=CodeValueD.Add/Change Code Value 
      :p.This is the help text for code_value_editdlg

  :h1 res=rCodeTab_RptDlgBox.Code Table Listing Report
:p.To print the :hp4.Code Table Listing Report:ehp4., perform the following steps:
:p.Open the Code Table Catalog. 
Perform the print sequence and select the Code Table Listing.  This box allows 
you to choose between printing the entire Code Table Catalog (the default mode) 
or printing only those tables you have selected.  (See the Tutorial chapter for 
a description of how to select multiple items in CES.)

  :h1 res=CostSheetD.Add/Change Cost Sheet 
:p.To add/change a :hp4.Cost Sheet:ehp4. to the catalog, perform the following steps:
:p.Activate the Cost Sheet window, then hit the insert key to add and Control C to Change or click on Edit 
Add/Change command sequence.  The dialog box will appear.  Following are the 
conventions and default values that apply to this dialog box:  
:ul compact.
:li.The production rate should be a number.  If  you type [3 per day], for 
instance, it will automatically compute a value such as 30.  
:li.The granularity must be the singular of either day, week, month, or year.   
The default hours per day will be eight.  
:li.The overhead and profit percentages must be typed in as decimals--The percent 
field requires a percentage.  Eight percent can be entered either as .08 or 8%.  
Entering a whole number only, such as 8, would signify to the computer 800 percent.  
:li.Select one of the choice buttons (OK, Cancel, Help, or Add More) to perform 
the desired action.  If you select Add More, the program will record the entered 
value and clear the dialog box for new entries.  If  you press enter or cancel, 
the program will send you back to the Cost Sheet Catalog.
:eul.

  :h1 res=CountyD.Add/Change County 
:p.To add or change :hp4.Counties:ehp4. to the County Catalog, perform the following steps:
:ul compact.
:li.Activate the County Catalog window.
:li.Press the insert key to add and Control C to Change or click on Edit, Add/Change.  The program will display the 
dialog box with two fields--County ID and County Name.  
:li.Enter the desired counties, then either cancel, add more, or enter the data.  
:eul.
:p.The County Catalog window displays the following data fields:
:p.:hp4.County.:ehp4.   The County field shows the abbreviation or code that uniquely identifies
a county.  
:p.:hp4.Name.:ehp4.   The Name field shows the name of the county.
  
   :h1 res=RCONTY_RptDlgBox.County Listing Report
:p.To print the :hp4.County Listing Report:ehp4., perform the following steps:
:p.Open the County Listing Catalog.
Perform the print sequence and select the County Listing Report. This box allows 
you to choose between printing the entire County Listing Report (the default 
mode) or printing only those counties you have selected.  (See the Tutorial 
Chapter for a discussion of selecting multiple items in CES.)

  :h1 res=CrewD.Add/Change Crew 
:p.To add or change (crews) in the :hp4.Crew Catalog:ehp4., perform the following steps: 
:ul compact.
:li.Activate the Crew Catalog window.
:li.Hit the insert key to add and Control C to Change or click on Edit, Add/Change.  Enter the data; you can accept 
the data, reject it, or add more.  The Crew ID field can have up to eight 
characters.  The description field accepts text or numeric input.
:li. Choose one of the four choice buttons (OK, Cancel, Help, Add More) to 
perform the desired action.
:eul.
  
   :h1 res=CrewLbEqD.Change Crew
:p.:p.To change (crews) in the :hp4.Crew Catalog:ehp4., perform the following steps: 
:ul compact.
:li.Activate the Crew Catalog window.
:li.Hit Control C to Change or click on Edit, Change.  Enter the data; you can accept 
the data, reject it, or add more.  The Crew ID field can have up to eight 
characters.  The description field accepts text or numeric input.
:li. Choose one of the four choice buttons (OK, Cancel, Help) to 
perform the desired action.
:eul.


  :h1 res=CrewLaborD.Add/Change Crew Labor
      :p.This is the help text for addchcrewlaboreditdlg

  :h1 res=CrewEquipD.Add/Change Crew Equipment
      :p.This is the help text for addchcrewequipeditdlg

  :h1 res=DefltPricesD.Add/Change Default Unit Prices
:p.To add or change the :hp4.Default Unit Prices:ehp4. perform the following steps:
:ul compact.
:li.Activate the Default Unit Prices window.
:li.Press insert to add and Control C to Change or click on Edit, Add/Change.  
:li.Select the item you wish to add or change.  CES will automatically enter the item number 
in the first field.
:li.Make the price change or add and record the description.
:li.Select one of the four choice buttons (OK, Cancel, Help, Add More) to perform 
the desired action.
Since the PEMETH (Price Estimation Method) Averages section is an Estimation 
Module, the windows for this module are displayed in the BAMS/CES display area.  
The user will activate the windows from the BAMS/CES menu bar.  The following 
model displays the paths to the windows that contain PEMETH weighted averages.
:eul.
  
  :h1 res=FacilityBrowserD.facilitybrowserd
      :p.This is the help text for facilitybrowserd
   
  :h1 res=PropBrowserD.propbrowserd
      :p.This is the help text for propbrowserd

  :h1 res=rPrjt_RptDlgBox.rprjt_rptdlgbox
      :p.This is the help text for rprjt_rptdlgbox

  :h1 res=EquipmentD.Add/Change Equipment 
:p.To add or change :hp4.Equipment:ehp4. to the catalog, perform the following steps:
:ul compact.
:li.Activate the Equipment Catalog window.
:li.Fill in the ID and Description fields.
:li.Press insert to add and Control C to Change or click on Edit, Add/Change.  
:li.The Zone Type ID field is a combination box.  Select from the list provided 
to complete this box.  
:li.Select one of the four choice buttons (OK, Cancel, Help, Add More) to perform 
the desired action.
:eul.
  
   :h1 res=EquipRateD.Add/Change Equipment Rate 
      :p.This is the help text for equiprateeditdlg

  :h1 res=rEquip_RptDlgBox. Equipment Listing Report
      :p.This is the help text for requip_rptdlgbox

  :h1 res=FacilityD.Add/Change Facility 
      :p.This is the help text for facility_editdlg

  :h1 res=FundD.Add/Change Fund 
      :p.To add or change a :hp4.Fund:ehp4. to the catalog, perform the following steps:
:ul compact.
:li.Activate the Fund Catalog window.
:li.Press the insert key to add and Control C to Change or click on Edit, Add/Change.  .  The program displays the 
dialog box with three fields--a Fund ID, a description, and a (fund) Name field.
:li.Fill in the data, then enter, cancel or add more.  
:eul.

  :h1 res=rFund_RptDlgBox.Fund Listing Report
:p.To print the :hp4.Fund Listing Report:ehp4. perform the following steps:
:p.Open the Fund Catalog.
Perform the print sequence and select the Fund Listing Report. This box allows 
you to choose between printing the entire Fund Listing Report (the default mode) 
or printing only those funds you have selected.  (See the Tutorial chapter for 
a discussion of selecting multiple items in CES.)



  :h1 res=InflationPredictionD.Add/Change Inflation Prediction 
:p.To add/change :hp4.Inflation Predictions:ehp4., perform the following steps:
:ul compact.
:li.Activate the Inflation Prediction Catalog. 
:li.Press the insert key to add and Control C to Change or click on Edit, Add/Change.  .  The program will display the 
dialog box.  
:li.Enter the data.  The percent field requires a percentage.  Eight percent 
can be entered either as .08 or 8%.  Entering a whole number only, such as 8, 
would signify to the computer 800 percent.  
:li.Enter (or select OK), cancel, or add more data.
:eul.
  
   :h1 res=LaborD.Add/Change Labor 
:p.To add/change :hp4.Labor:ehp4. classifications, perform the following steps:
:ul compact.
:li.Activate the Labor Catalog window.
:li.Hit the insert key to add and Control C to Change or click on Edit, Add/Change.  .  The 
program will display the dialog box.
:li.Type in the Laborer ID and Description fields.
:li.Select the desired Zone Type ID from the combination box.  Using the Tab 
key will keep you in the dialog box--you will then have to hit enter or OK to 
return to the Labor Catalog.  Hitting enter to select the ID will enter the 
data and return you immediately to the Labor Catalog.
:eul.
  
   :h1 res=LabRateD.Add/Change Labor Rate 
:p.To add or change a :hp4.Labor Wage Rate:ehp4., perform the following steps:
:ul compact.
:li.Open the Labor Wage List window.
:li.Press insert to add and Control C to Change or click on Edit, Add/Change. CES will display the dialog box.
:li.Enter in the Davis- and non-Davis-Bacon wage rates both for overtime and regular 
wages.  The zone must be chosen from the provided list.
:li.Select OK, Cancel, Help, or Add More to perform the desired action.
:eul.


  :h1 res=MajorItemD.Add/Change Major Item 
:p.To add/change :hp4.Major Items:ehp4. to the catalog, perform the following steps:
:ul compact.
:li.Activate the Major Item Catalog window.
:li.Hit the insert key to add and Control C to Change or click on Edit, Add/Change.  .  
The program will display the dialog.
:li.Fill in the major item ID and the description.
:li.Select a common unit from the combination box provided.
:li.Enter your data, cancel, or add more data.
:eul.
  
   :h1 res=StdItemMajItemD.stditemmajitemeditdlg
      :p.This is the help text for stditemmajitemeditdlg

  :h1 res=rMjrIt_RptDlgBox.Major Item Listing Report
:p.To print the :hp4.Major Item Listing Report:ehp4., perform the following steps:
:p.Open the Major Item Catalog.
Perform the print sequence and select the Major Item Listing.  The Price 
Estimation section will be printed as part of the Major Item Listing.  Every 
major item that has a Major Item Price Regression record will generate this 
section.  It will be inserted after the Standard Items section of the Major Item 
Listing.  The Quantity Estimation section of the Major Item Listing will also be 
printed as part of the Major Item Listing.  Every major item that has a Major 
Item Quantity Regression record will generate this section.  This section will 
be inserted after the Standard Items section of the Major Item Listing, and 
following the Price Estimation section (if one exists).



  :h1 res=MapD.Add/Change Map 
:p.To add/change :hp4.Maps:ehp4. to the catalog, perform the following steps:
:ul compact.
:li.Activate the Map Catalog window. 
:li.Hit the insert key to add and Control C to Change or click on Edit, Add/Change. The program 
displays the dialog box.
:li.Type in a map ID and description.
:li.Select one of the four choice buttons (OK, Cancel, Help, or Add More) to 
perform the desired function.  Selecting Add More will add the data currently 
in the fields and then clear the screen for new data.
:eul.
  
   :h1 res=rMap_RptDlgBox.Map Listing Report
:p.To print the :hp4.Map Listing Report:ehp4., perform the following steps:
:p.Open the Map Catalog.
Perform the print sequence and select the Map Listing Report. 
Click on the square to toggle between printing the entire Map Listing (the 
default setting) and printing only the funds you have selected.  An X in the box 
(the checked position) represents the positive condition.  In this case it will
print only the maps you have selected. 

  :h1 res=MaterialD.Add/Change Material 
:p.To add/change :hp4.Materials:ehp4. to this list, perform the following steps:
:ul compact.
:li.Activate the Material Price List window.
:li.Hit the insert key to add and Control C to Change or click on Edit, Add/Change.
:li.Fill in the fields in the displayed dialog box.  The Unit and Zone Type 
ID fields are combination boxes and therefore only accept data chosen from the 
provided lists.  Click on the down arrow to the right of the field or use the 
up and down arrow keys to scroll through the options provided.
:li.Choose one of the four choice buttons (OK, Cancel, Help, or Add More) to 
perform the desired function.
:eul.
  
   :h1 res=MatPricesD.Add/Change Material Price
:p.To add or change :hp4.Material Price:ehp4. perform the following steps:
:ul compact.
:li.Activate the Material Price list window.
:li.Hit the insert key to add and Control C to Change or click on Edit, Add/Change. CES displays 
the dialog box.
:li.Enter or change the new material price.
:li.Select the zones to add to from the combination box. 
:li.Choose one of the four choice buttons (OK, Cancel, Help, or Add More) to 
perform the desired function.
:eul.


  :h1 res=RMAT_RptDlgBox.Materials Listing Report
:p.To print the :hp4.Material Listing Report:ehp4., perform the following steps:
:p.Open the Material Listing Catalog and select the Material Listing Report.  
This box allows you to choose the order in which the materials are listed (each 
choice is mutually exclusive) and whether to print the full list or only the 
tables you select.  An X in the box (the checked position) means it will print 
the selected materials only.  You can also include the Zone prices by checking 
the Include Zone Prices box. CES also provides the option to print the facilities 
section and prices section attachments to the Material Listing report.


  :h1 res=ParametricProfileD.Add/Change Parametric Profile 
:p.To add or change :hp4.Parametric Profile:ehp4.  perform the following steps:
:ul compact.
:li.Activate the Parametric Profile window.
:li.Hit the insert key to add and Control C to Change or click on Edit, Add/Change. CES displays the dialog 
box.
:li.Fill in the ID and description in the displayed dialog box.  
:li.Select one of the four choice buttons (OK, Cancel, Help, or Add More) to perform 
the desired function.
:eul.


  :h1 res=SignifMajItemD.Add/Change Significant Major Item
:p.To add or change :hp4.Significant Major Items:ehp4., perform the following steps:
:ul compact.
:li.Activate the Significant Major Item window.
:li.Hit the insert key to add and Control C to Change or click on Edit, Add/Change.  CES displays the dialog 
box.
:li.Select the major item you wish to add from the list box provided.  CES will 
automatically enter the identifier for this major item.    
:li.Enter in the percent of cost you expect this major item to represent.  This 
must be entered as a decimal (.08 for 8 percent).
:li.Choose one of the four choice buttons (OK, Cancel, Help, or Add More) to perform 
the desired function.
:eul.

  :h1 res=JobBreakdownProfileE.Add/Change Job Breakdown Profile 
:p.To add or change :hp4.Job Breakdown Profiles:ehp4., perform the following steps:
:ul compact.
:li.Activate the Job Breakdown Profile window.
:li.Hit the insert key to add and Control C to Change or click on Edit, Add/Change.  CES will display the dialog box. 
:li.Enter the values requested by the dialog box.  Remember that percentages 
must be entered in decimal form or whole number form with a percent sign 
attached.
:li.Choose from one of the four choice buttons (OK, Cancel, Add More, or Help) 
to perform the desired action.
:eul.

  :h1 res=rStdIt_RptDlgBox.Standard Item Listing Report
:p.To print the :hp4.Standard Item Listing Report:ehp4., perform the following steps:
:p.Open the Standard Item Catalog.
Perform the print sequence and select the Standard Item Listing. Choose whether 
or not to limit the selection of standard items to a specified year by pulling 
down the list in the SpecYr list box.  If the box is left blank, all items will 
be listed. Choose whether to print only the selected items or to print all of 
them (subject to the first condition).  In other words, if you chose a spec year, 
the total list of Standard Items  is smaller than it would have been if you had 
not specified a year.  If you then choose to print all the standard items (you 
left the second choice blank), the program will print all the items from the 
smaller list (the items from the selected spec year).  Choose which additional 
price estimation method(s) to include with the last four choices.  The Standard 
Item Listing Report will print the Cost-Based Estimate section for each standard 
item if the following conditions are met:  (1) the standard item has one or more 
Cost-Based Estimation records associated with it,  and (2)  the user selects to 
include the price bases in the report.  This section will be printed following 
the main text of the Standard Item Listing. The Standard Item Listing report 
will print the PEMETH Averages section for each standard item if the following 
conditions are met:  (1) it has a PEMETH historical weighted average record, and 
(2) the user selects to include the price bases in the report.  This section will 
be printed following the main text of the Standard Item Listing. The Standard 
Item Listing Report will print the PEMETH Regression section for each standard 
if the following conditions are met:  (1) the item has a PEMETH Regression 
record associated with it, and (2) the user selects to include the price bases 
in the report.  This section will be printed after the main text of the Standard 
Item Listing.

  :h1 res=StanItemD.Add/Change Standard Item
:p.To add or change :hp4.Standard Items:ehp4., perform the following steps:
:ul compact.
:li.Activate the Standard Item Catalog window.
:li.Hit the insert key to add and Control C to Change or click on Edit, Add/Change.  The 
program will display a dialog box with 8 fields.  
:li.Enter the appropriate data, and tab to move from one field to the next.  
:li.The Unit field drop-down box lists the available units of measure for the 
item.  Some of the entries may appear twice in the list, for example EACH; 
the first occurance of the entry is common to both the English and metric 
measurement systems, the second occurance is exclusive to the currently 
active measurement system that was selected from the Utilities menu. 
:li.The Default Start Time field is expressed as a percent of the job.  For 
instance, if a standard item could not be used until the job was 10 percent 
complete, you would enter the decimal value .10.  If you enter a whole number 
value such as 8, the program will compute that to be 800 percent.
:li.The unit and rounding precision fields are combination boxes (designated 
by the down arrow in the right side of the box).  You must select from the supplied 
lists for the data for these cells.  
:li.The small square just above the OK button is a check box.  This box allows 
you to choose whether the item is costed by unit or by lump sum.  Click on this 
box to toggle between the two values.  An X in the box represents the "true" 
condition while a blank box represents a "false" condition.
:li.Select one of the four choice buttons  (OK, Cancel, Help, or Add More) to 
perform the desired function.  Selecting Add More enters the existing data and 
clears the screen for new data. 
:eul.
  
   :h1 res=rUser_RptDlgBox.User Listing Report
:p.To print the :hp4.User Listing Report:ehp4., perform the following steps:
:p.Open the User Catalog.  If you have only user status, you will not be able to 
access this catalog and therefore will not be able to print this report. 
Perform the print sequence and select the User Listing. 

  :h1 res=UserD.Add/Change User 
      :p.This is the help text for user_editdlg

  :h1 res=rVend_RptDlgBox.Vendor Listing Report
:p.To print the :hp4.Vendor Listing Report:ehp4., perform the following steps
:p.Open the Vendor Catalog and perform the print sequence.
Select the Vendor Listing report.

  :h1 res=VendorD.Add/Change Vendor 
:p.To add or change :hp4.Vendors:ehp4., perform the following steps:
:ul compact.
:li.Activate the Vendor Catalog window.
:li.Hit the insert key to add and Control C to Change or click on Edit, Add/Change.  The 
program will display the dialog box.
:li.Enter the vendor ID, truncated name, and full name in the appropriate 
boxes.  Move from box to box with the tab key.
:li.Select from the lists for the three combination boxes (Type, Certification, 
and DBE Classification).
:li.Choose one of the four choice buttons (OK, Cancel, Help, or Add More) to 
perform the desired activity.  Selecting the Add More command enters the currently 
displayed data and clears the screen for new data.
:eul.
  
   :h1 res=VenEditD.Add Vendor Window
:p.To add :hp4.New Vendors:ehp4., perform the following steps:
:ul compact.
:li.Activate the Vendor Catalog window.
:li.Hit the insert key or execute the Edit, Add command sequence.  The program 
will display the dialog box.
:li.Enter the vendor ID, truncated name, and full name in the appropriate boxes.  
Move from box to box with the tab key.
:li.Select from the lists for the three combination boxes (Type, Certification, 
and DBE Classification).
:li.Choose one of the four choice buttons (OK, Cancel, Help, or Add More) to 
perform the desired activity.  Selecting the Add More command enters the currently 
displayed data and clears the screen for new data.
:eul.
  
   :h1 res=VendorAddressD.Add/Change Vendor Address 
      :p.This is the help text for vendoraddresseditdlg

  :h1 res=WorkClassD.Add/Change Work Class 
      :p.This is the help text for workclasseditdlg

  :h1 res=WorkDayD.Add/Change Working Day Calendar
:p.To add or change :hp4.Working Day Calendars:ehp4. perform the following steps:
:ul compact.
:li.Activate the Working Day Calendar Catalog. 
:li.Hit the insert key to add and Control C to Change or click on Edit, Add/Change.  CES will display 
the dialog box.
:li.Enter the ID and the description of the new calendar.  
:li.Choose one of the four choice buttons (OK, Cancel, Help, or Add More) to perform 
the desired action.  Selecting the Add More button enters the displayed data and 
clears the screen for new data.
:eul.

     :h1 res=WorkDayInfoD.Add/Change Working Day Calendar Info
:p.To add or change information in a :hp4.Working Day Calendar:ehp4. perform the following steps:
:ul compact.
:li.Activate the Working Day Calendar window. 
:li.Hit the insert key or execute the Edit, Add command sequence.  CES will display 
the dialog box.
:li.Fill in the plain boxes and select from the list provided for the Month 
combination box. 
:li.Select one of the four choice buttons (OK, Cancel, Help, or Add More) to 
perform the desired action.  Selecting the Add More button enters the displayed 
data and clears the screen for new data.


  :h1 res=rZones_RptDlgBox.Zone Listing Report
:p.To print the :hp4.Zone Listing Report:ehp4., perform the following steps
:p.Open the Zone Type Catalog and perform the print sequence.

  :h1 res=ZoneTypeD.Add/Change Zone Type 
:p.To add or change :hp4.Zone Types:ehp4., perform the following steps:
:ul compact.
:li.Hit the insert key to add and Control C to Change or click on Edit, Add/Change.
:li.Fill in the Zone Type ID and Description fields, then either accept the 
data, cancel, or accept and add more.
:eul.
  
   :h1 res=ZoneD.Add/Change Zone 
:p.To add or change :hp4.Zones:ehp4., perform the following steps:
:ul compact.
:li.Hit the insert key to add and Control C to Change or click on Edit, Add/Change.  The 
program will display the dialog box.
:li.Fill in the Zone ID and Description fields.
:li.You can either add or delete zone maps that go with this zone or leave 
the list as is.
:li.Accept, Cancel, or Add More data.
:eul.
  
   :h1 res=ZoneMapD.Add/Change Zone Maps
      :p.This is the help text for addchzonemapeditdlg



   
   :h1 res=200001.General Structure of CES  
      :p.BAMS/CES operates through a series of layered windows, with each
successive window offering more detailed information than the
previous window.  At each window level, you have the option of
editing (adding, changing, or deleting) the contents of the
current window or moving to the next window in the chain (by
viewing subset windows).  In this way, you are able to plan in
global terms, to develop a rough estimate of cost, and then add
more specific information to hone the estimate.

      
   :h1 res=200002.Types of Commands  
      :p.There are two types of commands in CES--window-dependent and
window-independent.  Window-dependent commands yield different
results, based on the window active when they are used.  For
example, the View command can show a job item list, a job item
breakdown, a job alternate group list, or any of seveal others, 
depending on the window that was active when this command was
executed.  
:sl compact.
:li.Window-independent commands can be accessed from anywhere and
will always provide you with the same results.  The File command
is an example of this type of command.   It allows you to perform
file-related functions such as printing, saving, accessing
catalogs, etc., even if you are in the middle of some other
operation or are several sub-menus deep.  
:li.A very useful result of this is the following:  if you are trying
to fill in a dialog box and do not know a code for an item or do
not know the actual item number, you can leave the dialog box for
a moment and execute the File command.  Since the File command is
the path taken to all the catalogs provided by CES, you can
select a catalog that will have the information you need (in this
case, the Code Table Catalog or the Standard Item or Major Item
Listing Catalog) and then copy that information into the dialog
box.  This is much easier than "backing out" of all the menus you
just went through, finding the information, then returning
through that same path to get back to that same dialog box.  
:esl.


   :h1 res=200003.CES Colors  
      :p.Window Backgroung colors
CES uses specific colors for windows and data inside windows to
convey the status of that window or data.  
:sl compact.
:li.When you perform an action such as entering the data for a new
job, the program will digest that information and provide you
with another window that presents the data in a logical way.  The
window may be visible but not yet accessible, due to the amount
of processing time necessary.  CES indicates this either by
displaying the message <working> in the title bar and/or by
keeping the window gray until it is fully functional, at which
time it will become white.  
:li.Data Colors
:li.The data in CES windows is presented in the following colors on
the screen:  black, red, and blue.  The names of lists, windows,
or data titles are presented in black.  The actual information
will be presented in either red or blue.  Blue numbers signify
that this number is the result of  computations or comes from
several sources or has some other discoverable origin.  
Double clicking (clicking twice in rapid succession with the left
mouse button) on one of these blue numbers will access the next
level of detail for this number.  This process can be continued
until the data item is broken down to individual unit cost for
each component part.  
:li.Numbers such as total cost and totals adjusted for inflation will
be in blue.   For example, after opening an existing program, you
will see a total line and a total adjusted for inflation line in
blue.  Double clicking on either of these numbers will take you
to the job level for this program, providing you with a list of
all the jobs that are a part of this program.  Continue this
process to see a specific job, and then a specific job breakdown
of that job, and then the cost sheets, etc. 
:li.Gray vs. Black Commands in Menus
:li.As in other Windows format programs, if a menu selection makes no
sense at the current time or is denied to you for some other
reason, that command will be gray instead of black.  This allows
you to see all the possible commands, even if they are not
accessible to you at the moment.  You can then determine why a
particular command is not available and what you have to do to
make it available.   
:li.For instance, when you first open CES, the Print, Import and
Export, and Close commands make no sense since there are no files
open.  Similarly, there is nothing to edit, so the whole Edit
menu will be gray.  The Utilities and View menus will be active
since you can adjust global viewing and presentation preferences
without having a file open.  CES also records user status, due to
the sensitive nature of some of the data.  Therefore, some of the
lists will be view-only or will not even open, depending on the
status of the user.
:esl.

   :h1 res=200004.Important Default Settings  
      :p.Combination (list) Boxes
:sl compact.
:li.The combination boxes in CES will always highlight the first
choice in the list.  Therefore if you immediately hit enter or
select OK without looking, that item will be selected. 
Dialog Boxes
:li.CES has many dialog boxes.  These are boxes that request
information from 

the user in order to complete an action.  In
most cases, when these boxes ask for a measurement value such as
depth of pavement, they do not specifiy the unit in which the
value will be measured.  Some common values and their units
follow:  
:li.o For pavement:  depth is measured in inches, width in feet, and
length in miles.  
:li.o For structures, length and width are measured in feet.  
:li.o Whenever the program asks for location, you must enter latitude
and longitude, in degrees, minutes, and seconds.   
:li.o Percentage values must be typed in as decimals or as a number
with a percent sign after it (8 percent would be entered as .08
or 8%).  A whole number will be interpreted as that number times
100%.  Therefore, if you type just the number 8, CES would record
it as 800 percent. 
:li.The dialog boxes that come with the Edit, Change command sequence
will highlight the OK choice by default.  Thus, if you hit enter
at any time during the editing process, CES will automatically
accept those changes.  For operations that could negatively
affect your work (such as deleting standard items) the default
setting is to have the NO choice button highlighted.  In this
way, you are protected from deleting something by accident. 
The Print dialog default to the print condition.  Therefore,
hitting enter immediately will cause CES to print the default
report, without any specialization.
:esl.

   :h1 res=200005.Running CES  
      :p.From OS2, click on the CES menu choice.  The CES screen comes up
(blank).  You then choose whether to open a new program or job or
an existing one.  You do not have to open a job or program--many
CES features (such as accessing catalogs) are accessible from the
main menu screen

   :h1 res=200006.Password and Log-on  
      :p.Before you are able to open a new or existing job, you must
provided the system with your password and user name.  The
passwords for CES are case-sensitive as well as being sensitive
to spaces or other typed entries.  This means that if your
password is defined with some upper case and some lower case
letters, it will have to be typed in that way.  It will not be
visible as you type it, and therefore you will have to type it
twice.  The program will deny access until both of the entries
match.  
:p.In addition to the password, you must logon with a user name. 
Your logon request can be denied for the following reasons:  (1) 
you typed the username incorrectly,  (2) the username you typed
is not valid, (3) the password you typed was invalid, or (4) the 
System Manager is either installing or updating the system and
has denied access to all users.


   :h1 res=200007.Activating and Closing Windows  
      :p.Click on any window to make it the active window.  If the window
you wish to activate is not visible, you have four choices:  
:sl compact.
:li.(1) Begin closing windows until the desired window is exposed. 
You only need to see a portion small enough to identify the
window, to click on it.  
:li.(2) Use the minimize down arrow (in the top right-hand corner of
any active window) to shrink the window to icon size.  This will
expose the windows "hidden" beneath it.  
:li.(3)  Change the size of the windows.  If the window has a border,
you can position the cursor arrow just at the edge of the border
until the arrow becomes a double arrow that straddles the border
line.  You can then drag the mouse to shrink one side of the
window.  If the arrow is placed at a corner of a window, you can
resize two borders at once.  
:li.(4)  Move the window by clicking in the title bar and then
dragging the window (at its current size) to a new location on
the screen.  
:li.Closing Windows:
:li.Due to the tree structure of this program, there will be times
when many windows will be open at one time.  You may wish to
close all of them before exiting or you may simply close them to
keep system performance as high as possible.  In either case,
there are two ways to close all the open windows:  The obvious
way is to close each window in the opposite order in which they
were opened.  However, this can be frustrating and time-consuming
if more than three or four are open.
:li.The simpler way to do this is to click on the first window opened
(the one on the "bottom").  CES will then superimpose it over the
other windows (its children).  You then close this one (the
parent) and all its children close automatically with it.  This
demands some foresight in that some portion of the window must be
visible in order to be able to click on it.  In addition, it is
helpful if you can see the window title.  
:li.Therefore, a good method for using multiple windows in CES is to
move each new window just beneath its parent.  This is done by
clicking in the title bar and dragging it to its new location. 
Then the title bar of each window will be visible.  This gives
other benefits as well since it allows you to selectively go back
to an earlier window.  
:li.This method works for any two or more windows that have a
parent/child relationship.  Therefore, if you were five or six
windows deep in the job menu (for instance at the job item
window), you could close all the windows except the job window by
clicking on the immediate child of the job window.  This will
leave the job window open thus allowing you to take a different
path of windows.  
:li.This only works on windows that have a parent/child relationship. 
Therefore, if six layers of job windows and a couple of program
windows are open, closing the parent job window will clear the
screen of job windows, but will leave the program windows.   
Esl.

   :h1 res=200008.Creating New Programs or Jobs  
      :p.To create a new program or job, use the File command, then choose
[New program] or [New Job].  The screen will display a full
screen dialog box with entry fields and combination boxes. (See
the File section of Chapter 4 for a detailed description of the
data entry procedure.)  Enter the data in these fields and press
enter or click on OK.  The program will digest the entries and
provide you with another blank screen.  At this point, you can
choose the View command and begin to add things such as job
breakdowns, user lists, fund lists, alternate groups, etc.  This
will cause the program to begin accumulating costs.  You can go
back to the job window at any time to view the cost of the
program up to this point.

   :h1 res=200009.Saving and Exiting  
      :p.CES automatically stores all data entered into it.  Therefore you
do not have to perform a save procedure before you exit.  To exit
the program, choose the File, Exit command sequence or double
click on the control bar in the upper left hand corner of the
main screen.  This action will return you to the Program Manager
in OS/2.  To exit OS/2 you must first perform a shutdown from the
Program Manager.  (See your OS/2 support documentation for this
procedure.)


   :h1 res=200010.The Big Picture  
      :p.To make a detailed estimate, you go through these 7 or 8 menu
choices, providing all the detail you have available.  Each of
these choices will take you several layers or more in the CES
structure.  You can the use the "window map" in the Users Guide to help
keep track of your position in CES.  
:p.Windows Available from the Open Job Command
:sl compact.
:li.Each time you enter new information, CES adds it to the job and
computes its effect on total cost.
Click on File, then New Job.  CES will display the Add Job dialog
box. 
:li.The top three fields, Item Number, Description, and Unit, are
grayed.  This means you can only enter these values by selecting
from the list provided.  The program automatically fills in the
item number, description, and unit fields.  Once you have done
this, type in the quantity and unit price.  Choose the estimation
method by selecting from the list attached to this combination
box.  With a mouse you click on the down arrow to see the whole
list.  With a keyboard you use the arrow keys to scroll through
the list.  After adding this item, select OK or press ENTER to
finish adding the job item.  CES returns you to the Job Item List
window.  The item you just entered will occupy the place of the
item where your cursor was at the time of the addition. 
Note:  The Job Item List window must be full width in order for
you to see the scroll bars.  CES will truncate screens to some
extent, but if there is too much text in the window, the borders
simply will not be displayed.  Therefore, if you are not using a
full screen to work in CES, you may not be able to see the scroll
bar.  Simply drag the right border of CES out until you are able
to see the scroll bar.  
:li.Printing the job estimate report.
From the Job window, select File, Print.  CES will display a list
of the reports printed by the program.  CES requires you to print
a report from the applicable window.  Therefore, when you open
the print menu, most of the choices you see are grayed, meaning
they cannot be printed from this window.  In this case, three
reports are represented with black letters--the Job Estimate, the
Job Funding Report, and the Job Item Schedule Listing.   Select
the Job Estimate Report.  CES will display the dialog
box with two choices for report customization.  
:li.Part 2:  Completing the Job
:li.Adding the rest of the items.
:li.Reopen the job you created in the first section.  Add at least X
of the items listed below.  Since you will be adding multiple
items, choose the Add More option after each item.  This will
save you time and keystrokes by always presenting a blank dialog
box rather than putting you back to the Job List window.
Provide list of items to be added to the tutorial project here.
:li.Setting up users.
:li.Select the Job User List from the 8 choices provided.  You will
see a list of users.  Add several new team members by perfroming
the Edit, Add command sequence.  CES will display the dialog box:
:li.Fill in the data boxes to enter the first person on your team.  
Then, select Add More.  
:esl.


   :h1 res=200011.Default Unit Prices  
:p.Default Unit Prices are assigned to each Standard Item by some 
upper echelon.  They may be used in the absence, or in place of other
determination methods.

   :h1 res=200012.Cost-Based Estimation  
:lm margin=3.
:p.  Cost-Based estimation divides the work of a Standard Item into 
one or more tasks, with each task being a separate cost sheet.  Note the cost
sheets are not copies but rather references to one source cost sheet in the
catalog list.
:p. 
:p.:lines align=center.Cost-Based Estimate Procedure:elines.
:p.  To build a cost based estimate for a Standard Item, highlight the item in the
Standard Item Catalog list then from the View menu select Cost Based Estimates.
This will open the "Cost-Based Estimates for X9X9X9" windows.  From this window
select Edit/Add to name and describe the cost based estimate that is to be
attached to the standard item; note the Active check box is used to enable or
disable the use of this particular cost sheet in the cost calculations for 
the standard item.
:p.  After the add dialog box is exited, highlight the new Cost-Based Estimate 
in the list and from the View menu select Task List.  The tasks, ie. cost
sheets, are added to the item's cost based estimate task list from the Cost
Sheet Catalog list in the dialog box.  In the task dialog box (either Edit/Add or
Edit/Change) are where any deviations from the from the attached Cost Sheet
that are specific to the Standard Item are made.  This is done by changing
the percentages in the "Production Adj" and "Material Qty Adj" edit boxes.
  These percentage adjustments are applied only to the task for the current 
Standard Item and are NOT changes in the Cost Sheet itself.

:p.  Also note that a task's Cost Sheet may have a different Unit of measure from the
Standard Item to which it is being attached; it is up to the user to enter the
conversion factor in the "Quantity Per Item Unit" edit box.  For instance a 
Cost Sheet may be in units of square feet (SF) and the Standard Item may be in 
units of square yards (SY).  As an example, a cost sheet that determines the
expenses for 5 square feet of 8 inch reinforced concrete might be attached
to a standard item that is measured as 1 square yard of finished road surface;
then the value entered into the task's "Quantity Per Item Unit" edit field
would then be 1.8 (there are 9 square feet to 1 square yard so 1.8 x 5 SF
equals 9 SF or 1 SY).  A more interesting conversion case would be a cost
sheet in tons and a standard item in lane-miles. 


   :h1 res=200013.PEMETH Averages  
:p.The estimated price of an item using this method is currently calculated
using data loaded from the BAMS/DSS BRPEMETH analysis model.  Historical
information about quantity level, work type, and geographic area are used
to compute the price for the item.  Please refer to section 2.4 of the
BAMS/CES User's Guide.

   :h1 res=200014.PEMETH Regression  
      :p.Please refer to section 2.5 of the User's Guide.

   :h1 res=200015.Parametric Estimation  
:p.Parametric Estimation projects cost by considering the parameters of the
project and involves various statistical modeling techniques.  Please refer
to section 2.3 the User's Guide.

   :h1 res=200016.Scheduling Job items  
:p.From within one of a job's many windows, open the Utiliies menu and 
select the 'Schedule Job Items' option.  This will start a background
series of calculations that will update the affected windows.  Do not try
and add or delete any job items until the windows are done changing, usually
about a minute or so.

   :h1 res=200017. Entering Job Comments  
:p.These are notations made by the estimator that once the dialog box is exited
can NOT be edited.  Additional comments may be added at a later time to clarify 
or nullify previous comments.

   :h1 res=200018.Adding job alternates  
      :p.Please refer to the User's Guide.

   :h1 res=200019.Running Browsers  
      :p.Please refer to the User's Guide.    

:euserdoc.
