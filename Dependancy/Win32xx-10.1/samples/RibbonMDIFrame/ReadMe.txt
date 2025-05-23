RibbonMDIFrame Example
===================
This project demonstrates how to add a ribbon to a MDI Frame window. The ribbon
buttons are defined in an Ribbon.xml. This application adds a ribbon to the
Scribble sample. The ribbon is only supported on Windows 7 OS (and above). If
the ribbon isn't supported by the operating system, the traditional menu and 
toolbar are displayed instead.

One of the Ribbon groups is titled Multiple Document Interface. This group
has the MDI Cascade, MDI Tile and Icon Arrange buttons.

The View group has a check button to show or hide the status bar, and a
color picker to select the color for the MDI child window.

The Ribbon contains a Multiple Document Interface group that has the buttons
for MDI Tile, MDI cascade etc. This group also has other buttons that are
enabled when the MDI child is maximized.


How the ribbon works
====================
Microsoft's RibbonUI framework uses COM to implement the ribbon. The COM 
interfaces involved are IUIApplication and IUICommandHandler. The Win32++ 
CRibbon class inherits from both IUIApplication and IUICommandHandler. 

To add a ribbon to your frame, inherit CMainFrame from CRibbonFrame, and add
the Ribbon.xml file containing the definitions for your ribbon to the project. 
CRibbonFrame inherits from CRibbonFrameT<CFrame>. To create and
interact with the ribbon, we override the relevant functions from CRibbon.

The functions you may wish to override are:
OnCreateUICommand 
  Called for each Command specified in the Ribbon markup to bind the Command 
  to an IUICommandHandler.  
OnDestroyUICommand 
  Called for each Command specified in the Ribbon markup when the Ribbon host 
  application window is destroyed.  
OnViewChanged 
  Called when the state of a View changes.
Execute 
  Executes or previews the Commands bound to the Command handler.  
UpdateProperty
  Sets a property value for a bound Command, for example, setting a Command to 
  enabled or disabled depending on the state of a View. 
  
Prerequisites:
==============
1. Windows 7 operating system
2. Microsoft Windows SDK v7.0 
3. Visual Studio 2005, 2008 or 2010 (full or Express Edition)

Note:
Microsoft Visual Community 2013 and 2015 include the Windows 7 SDK.

Compiling Instructions
======================
To compile Ribbon.xml, specify it's custom build properties as follows:

Command Line  uicc.exe ..\src\Ribbon.xml ..\src\Ribbon.bml /header:..\src\RibbonUI.h /res:..\src\RibbonUI.rc
Outputs       Ribbon.bml;RibbonUI.rc;RibbonUI.h 

Note: Ensure that the following VC++ Directories are configured:
Executables directory:  C:\Program Files\Microsoft SDKs\Windows\v7.0\Bin
Include directory:      C:\Program Files\Microsoft SDKs\Windows\v7.0\Include
