///////////////////////////////////////////////////////
// Mainfrm.cpp  - definitions for the CMainFrame class.


#include "Mainfrm.h"
#include "resource.h"


// Create the frame window.
HWND CMainFrame::Create(HWND parent)
{
    // Set m_view as the view window of the frame.
    SetView(m_view);

    // Call the base class function.
    return CFrame::Create(parent);
}

// Process the messages from the menu and toolbar.
BOOL CMainFrame::OnCommand(WPARAM wparam, LPARAM)
{
    switch (LOWORD(wparam))
    {
    case IDM_FILE_NEW:        return OnFileNew();
    case IDM_FILE_OPEN:       return OnFileOpen();
    case IDM_FILE_SAVE:       return OnFileSave();
    case IDM_FILE_SAVEAS:     return OnFileSaveAs();
    case IDM_FILE_PRINT:      return OnFilePrint();

    case IDW_VIEW_STATUSBAR:  return OnViewStatusBar();
    case IDW_VIEW_TOOLBAR:    return OnViewToolBar();
    case IDM_HELP_ABOUT:      return OnHelp();
    case IDM_FILE_EXIT:       return OnFileExit();
    }

    return FALSE;
}

// Called during window creation.
int CMainFrame::OnCreate(CREATESTRUCT& cs)
{
    // OnCreate controls the way the frame is created.
    // Overriding CFrame::OnCreate is optional.

    // A menu is added if the IDW_MAIN menu resource is defined.
    // Frames have all options enabled by default.
    // Use the following functions to disable options.

    // UseIndicatorStatus(FALSE);    // Don't show keyboard indicators in the StatusBar.
    // UseMenuStatus(FALSE);         // Don't show menu descriptions in the StatusBar.
    // UseOwnerDrawnMenu(FALSE);     // Don't use owner draw for popup menu items.
    // UseReBar(FALSE);              // Don't use a ReBar.
    // UseStatusBar(FALSE);          // Don't use a StatusBar.
    UseThemes(FALSE);                // Don't use themes.
    UseToolBar(FALSE);               // Don't use a ToolBar.

    // call the base class function
    return CFrame::OnCreate(cs);
}

// Issue a close request to the frame.
BOOL CMainFrame::OnFileExit()
{
    Close();
    return TRUE;
}

BOOL CMainFrame::OnFileNew()
{
    ::MessageBox(nullptr, L"File New  ... Implemented later", L"Menu", MB_OK);
    return TRUE;
}

BOOL CMainFrame::OnFileOpen()
{
    ::MessageBox(nullptr, L"File Open  ... Implemented later", L"Menu", MB_OK);
    return TRUE;
}

BOOL CMainFrame::OnFilePrint()
{
    ::MessageBox(nullptr, L"File Print  ... Implemented later", L"Menu", MB_OK);
    return TRUE;
}

BOOL CMainFrame::OnFileSave()
{
    ::MessageBox(nullptr, L"File Save  ... Implemented later", L"Menu", MB_OK);
    return TRUE;
}

BOOL CMainFrame::OnFileSaveAs()
{
    ::MessageBox(nullptr, L"File SaveAs  ... Implemented later", L"Menu", MB_OK);
    return TRUE;
}


// Handle the frame's messages.
LRESULT CMainFrame::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{

//  switch (msg)
//  {
//  }

    // Use the default message handling for remaining messages.
    return WndProcDefault(msg, wparam, lparam);
}
