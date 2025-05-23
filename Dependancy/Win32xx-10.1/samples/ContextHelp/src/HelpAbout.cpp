/////////////////////////////
// HelpAbout.cpp
//

#include "stdafx.h"
#include "HelpAbout.h"
#include "resource.h"

//////////////////////////////////
// CAboutHelp function definitions
//

// Constructor.
CAboutHelp::CAboutHelp() : CDialog(IDW_ABOUT)
{
    m_credits = "Use the SetCredits function to specify this dialog's text";
}

// Called before the dialog is displayed.
BOOL CAboutHelp::OnInitDialog()
{
    SetDlgItemText(IDC_ABOUT_CREDITS, m_credits);
    return TRUE;
}
