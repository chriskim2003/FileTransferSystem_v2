/* (06-May-2024) [Tab/Indent: 8/8][Line/Box: 80/74]              (AboutBox.cpp) *
********************************************************************************
|                                                                              |
|               Authors: Robert C. Tausworthe, David Nash, 2020                |
|                                                                              |
===============================================================================*

    Contents Description: Implementation of the AboutBox dialog for this
    application using the Win32++ framework.

    Programming Notes: The programming style roughly follows that established
    for the 1995-1999 Jet Propulsion Laboratory Deep Space Network Planning and
    Preparation Subsystem project for C++ programming.


*******************************************************************************/

#include "stdafx.h"
#include "AboutBox.h"
#include "App.h"
#include "resource.h"

/*============================================================================*/
    AboutBox::
AboutBox()                                                                  /*

    AboutBox constructor: uses the IDW_ABOUT dialog in resource.rc
*-----------------------------------------------------------------------------*/
    :   CDialog(IDW_ABOUT)
{
}

/*============================================================================*/
     BOOL AboutBox::
OnInitDialog()                                                              /*

     When the AboutBox is instantiated, enter the box contents.
*-----------------------------------------------------------------------------*/
{
     SetDlgItemText(IDC_CREDITS, m_status);
     return TRUE;
}
/*----------------------------------------------------------------------------*/

