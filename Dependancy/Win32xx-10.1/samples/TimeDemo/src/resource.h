/* (06-May-2024) [Tab/Indent: 8/8][Line/Box: 80/74]               (resource.h) *
********************************************************************************
|                                                                              |
|               Authors: Robert C. Tausworthe, David Nash, 2020                |
|                                                                              |
===============================================================================*

    Contents Description:  Resource definitions used by a minimal SDI
    application's resource.rc file.  Adapted to the Win32++ Windows
    interface classes.

    Special Conventions:  The specification of IDs for resource objects
    generally adhere to prefixes as shown in the following table. Note
    that different kinds of resource objects have different prefixes to
    distinguish the types of entities they belong to:

        Entity ID   Object
        IDP_        Message-box prompt
        IDD_        Dialog-box ID
        ID_             Standard toolbar and  menu command
        IDT_        Toolbar command
        IDM_        Menu command
        IDR_        Frame-related resource
        IDC_        Control resource
        IDW_            Win32++ default resources

    Notes about Resource IDs: In general, resource IDs can have values
    from 1 to 65535. Programs with resource IDs higher than 65535 may
    not be supported by some Windows versions.

    * Windows uses the icon with the lowest resource ID as the
    application icon. The Win32++ default_resource.h file defines this
    identifier as IDW_MAIN. The other default system identifiers it
    defines are all numbered below 100 and  are all prefixed IDW_. These
    include resource IDs for MENU, ICON, ToolBar Bitmap, Accelerator,
     and  Window Caption.

    * The Windows Platform SDK defines the WM_APP constant as a value
    to be used by applications to define private messages that do not
    conflict with system messages. The Win32++ suite is viewed as an
    application by the WINAPI and  preempts the first 24 (in version 7.6.1)
    of these, so this file defines its resource IDs beginning at a
    reasonable distance above this, to allow for their future expansion.
    This value is defined as 150 , below.

    * If multiple static controls appear in a dialog, these should be
    given unique identifiers if they are active. Otherwise, an ID of -1
    is used to designate these static controls do not actively participate
    in the application.

********************************************************************************

    Definitions of this application's resource identifiers

*******************************************************************************/

  // Include the Resource IDs defined by Win32++, numbered 51 - 99.
  // These are prefixed by IDW_.
#include "default_resource.h"

  // the base of this app's IDs:
#define THIS_APP                        150

  // resource IDs for Menu and  ToolBar commands
#define IDM_FILE_NEW                    151
#define IDM_FILE_OPEN                   152
#define IDM_FILE_SAVE                   153
#define IDM_FILE_SAVEAS                 154
#define IDM_FILE_CLOSE                  155
#define IDM_FILE_EXIT                   158

#define IDM_HELP_ABOUT                  167
#define IDM_COLOR_CHOICE                170
#define IDM_FONT_CHOICE                 171

#define IDB_MENUICONS                   175

  // resource IDs for controls
#define IDC_CREDITS                     180

/*-----------------------------------------------------------------------------*/

