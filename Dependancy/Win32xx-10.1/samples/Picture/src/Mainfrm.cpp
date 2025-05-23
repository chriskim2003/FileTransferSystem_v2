/////////////////////////////
// Mainfrm.cpp
//

#include "stdafx.h"
#include "Mainfrm.h"
#include "UserMessages.h"
#include "resource.h"

//////////////////////////////////
// CMainFrame function definitions
//

// Constructor.
CMainFrame::CMainFrame() : m_isDPIChanging(false)
{
}

// Create the frame window.
HWND CMainFrame::Create(HWND parent)
{
    // Set m_view as the view window of the frame.
    SetView(m_view);

    // Set the registry key name, and load the initial window position.
    // Use a registry key name like "CompanyName\\Application".
    LoadRegistrySettings(L"Win32++\\Picture Sample");

    // Load the settings from the registry with 4 MRU entries.
    LoadRegistryMRUSettings(4);

    return CFrame::Create(parent);
}

// Assigns the appropriately sized toolbar icons.
// Required for per-monitor DPI-aware.
void CMainFrame::DpiScaleToolBar()
{
    if (GetToolBar().IsWindow())
    {
        // Reset the toolbar images.
        SetToolBarImages(RGB(192, 192, 192), IDW_MAIN, IDB_TOOLBAR24_HOT, IDB_TOOLBAR24_DIS);
    }
}

// OnCommand responds to menu and and toolbar input.
BOOL CMainFrame::OnCommand(WPARAM wparam, LPARAM)
{
    UINT id = LOWORD(wparam);

    switch(id)
    {
    case IDM_FILE_NEW:          return OnFileNew();
    case IDM_FILE_OPEN:         return OnFileOpen();
    case IDM_FILE_SAVE:         return OnFileSave();
    case IDM_FILE_EXIT:         return OnFileExit();
    case IDW_VIEW_STATUSBAR:    return OnViewStatusBar();
    case IDW_VIEW_TOOLBAR:      return OnViewToolBar();
    case IDM_HELP_ABOUT:        return OnHelp();

    case IDW_FILE_MRU_FILE1:
    case IDW_FILE_MRU_FILE2:
    case IDW_FILE_MRU_FILE3:
    case IDW_FILE_MRU_FILE4:
    case IDW_FILE_MRU_FILE5:    return OnFileMRU(wparam);
    }

    return FALSE;
}

// Called when the window is created.
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
    // UseThemes(FALSE);             // Don't use themes.
    // UseToolBar(FALSE);            // Don't use a ToolBar.

    // Call the base class function.
    return CFrame::OnCreate(cs);
}

// Called when the effective dots per inch (dpi) for a window has changed.
// This occurs when:
//  - The window is moved to a new monitor that has a different DPI.
//  - The DPI of the monitor hosting the window changes.
LRESULT CMainFrame::OnDpiChanged(UINT, WPARAM, LPARAM)
{
    // Save the view's rectangle and disable scrolling.
    m_scrollPos = m_view.GetScrollPosition();
    m_view.SetScrollSizes();
    m_viewRect = m_view.GetClientRect();

    // Update the frame.
    m_isDPIChanging = true;
    ResetMenuMetrics();
    UpdateSettings();
    DpiScaleToolBar();
    ClearMenuIcons();
    SetupMenuIcons();
    RecalcLayout();

    return 0;
}

// Issue a close request to the frame to end the application.
BOOL CMainFrame::OnFileExit()
{
    Close();
    return TRUE;
}

// Called when a MRU file selection is made.
BOOL CMainFrame::OnFileMRU(WPARAM wparam)
{
    UINT mruIndex = LOWORD(wparam) - IDW_FILE_MRU_FILE1;
    CString smruText = GetMRUEntry(mruIndex);

    if (!m_view.LoadPictureFile(smruText))
    {
        RemoveMRUEntry(smruText);
        m_view.NewPictureFile();
    }

    return TRUE;
}

// Displays a blank (black) image.
BOOL CMainFrame::OnFileNew()
{
    m_view.NewPictureFile();
    return TRUE;
}

// Displays the file choose dialog an loads an image file.
BOOL CMainFrame::OnFileOpen()
{
    LPCWSTR filters = L"Supported Files Types(*.bmp;*.gif;*.jpg;*.ico;*.emf;*.wmf)\0*.bmp;*.gif;*.jpg;*.ico;*.emf;*.wmf\0Bitmaps (*.bmp)\0*.bmp\0GIF Files (*.gif)\0*.gif\0JPEG Files (*.jpg)\0*.jpg\0Icons (*.ico)\0*.ico\0Enhanced Metafiles (*.emf)\0*.emf\0Windows Metafiles (*.wmf)\0*.wmf\0\0";

    DWORD flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    CFileDialog fileDlg(TRUE, nullptr, nullptr, flags, filters);

    if (fileDlg.DoModal(*this) == IDOK)
    {
        CString str = fileDlg.GetPathName();
        m_view.LoadPictureFile(str);
    }

    return TRUE;
}

// Called when an image has been loaded from a file.
LRESULT CMainFrame::OnFileLoaded(LPCWSTR fileName)
{
    SetWindowText(fileName);
    AdjustFrameRect(m_view.GetImageRect());
    AddMRUEntry(fileName);
    return 0;
}

// Displays the file choose dialog and saves the image to the file.
BOOL CMainFrame::OnFileSave()
{
    if (m_view.GetPicture())
    {
        SHORT Type;
        m_view.GetPicture()->get_Type(&Type);
        LPCWSTR filter = nullptr;
        LPCWSTR ext    = nullptr;

        // Assign the default file extension and filter.
        // Note: iPicture doesn't convert between file types
        switch(Type)
        {
        case PICTYPE_BITMAP:
            filter = L"Supported Files Type(*.bmp)\0*.bmp;\0Bitmap (*.bmp)\0*.bmp\0\0";
            ext = L"bmp";
            break;
        case PICTYPE_METAFILE:
            filter = L"Supported Files Type(*.wmf)\0*.bmp;\0Metafile (*.wmf)\0*.wmf\0\0";
            ext = L"wmf";
            break;
        case PICTYPE_ICON:
            filter = L"Supported Files Type(*.ico)\0*.ico;\0Icon File (*.ico)\0*.ico\0\0";
            ext = L"ico";
            break;
        case PICTYPE_ENHMETAFILE:
            filter = L"Supported Files Type(*.emf)\0*.emf;\0Enhanced Metafile (*.emf)\0*.emf\0\0";
            ext = L"emf";
            break;
        }

        DWORD flags = OFN_OVERWRITEPROMPT;
        CFileDialog fileDlg(FALSE, ext, nullptr, flags, filter);

        if (fileDlg.DoModal(*this) == IDOK)
        {
            CString str = fileDlg.GetPathName();
            m_view.SavePicture(str);
            AddMRUEntry(str);
        }
    }

    return TRUE;
}

// Called before the dropdown menu is displayed.
void CMainFrame::OnMenuUpdate(UINT id)
{
    switch (id)
    {
    case IDM_FILE_SAVE:
    {
        // Enable FileSave menu item if a picture is loaded.
        UINT enabled = m_view.GetPicture() != nullptr ? MF_ENABLED : MF_GRAYED;
        GetFrameMenu().EnableMenuItem(id, enabled);
        break;
    }
    }

    CFrame::OnMenuUpdate(id);
}

// Called when the frame's position has changed.
LRESULT CMainFrame::OnWindowPosChanged(UINT msg, WPARAM wparam, LPARAM lparam)
{
    // The DPI can change when the window is moved to a different monitor.
    if (m_isDPIChanging)
    {
        // Adjust the frame size to fit the view.
        AdjustFrameRect(m_viewRect);

        // Restore the scrollbars and scroll position.
        CSize size = CSize(m_view.GetImageRect().Width(), m_view.GetImageRect().Height());
        m_view.SetScrollSizes(size);
        m_view.SetScrollPosition(m_scrollPos);
        m_isDPIChanging = false;
    }

    return FinalWindowProc(msg, wparam, lparam);
}

// Adds images to the popup menu items.
void CMainFrame::SetupMenuIcons()
{
    std::vector<UINT> data = GetToolBarData();
    if (GetMenuIconHeight() >= 24)
        SetMenuIcons(data, RGB(192, 192, 192), IDW_MAIN);
    else
        SetMenuIcons(data, RGB(192, 192, 192), IDB_TOOLBAR16);
}

// Sets the resource identifiers and images for the toolbar buttons
void CMainFrame::SetupToolBar()
{
    // Set the resource identifiers for the toolbar buttons
    AddToolBarButton( IDM_FILE_NEW   );
    AddToolBarButton( IDM_FILE_OPEN  );
    AddToolBarButton( IDM_FILE_SAVE  );

    AddToolBarButton( 0 );  // Separator
    AddToolBarButton( IDM_EDIT_CUT,   FALSE );
    AddToolBarButton( IDM_EDIT_COPY,  FALSE );
    AddToolBarButton( IDM_EDIT_PASTE, FALSE );

    AddToolBarButton( 0 );  // Separator
    AddToolBarButton( IDM_FILE_PRINT, FALSE );

    AddToolBarButton( 0 );  // Separator
    AddToolBarButton( IDM_HELP_ABOUT );

    // Set the image lists for normal, hot and disabled buttons
    SetToolBarImages(RGB(192,192,192), IDW_MAIN, IDB_TOOLBAR24_HOT, IDB_TOOLBAR24_DIS);
}

// Called by CPictureApp::OnIdle to update toolbar buttons
void CMainFrame::UpdateToolbar()
{
    // Enable the FileSave toolbar button if a picture is loaded.
    BOOL enabled = (m_view.GetPicture() != nullptr);
    GetToolBar().EnableButton(IDM_FILE_SAVE, enabled);
}

// Process the frame's window messages.
LRESULT CMainFrame::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case UWM_FILELOADED:       return OnFileLoaded((LPCWSTR)lparam);
        case WM_WINDOWPOSCHANGED:  return OnWindowPosChanged(msg, wparam, lparam);
        }

        // Pass unhandled messages on for default processing.
        return WndProcDefault(msg, wparam, lparam);
    }

    // Catch all unhandled CException types.
    catch (const CException& e)
    {
        // Display the exception and continue.
        CString str1;
        str1 << e.GetText() << L'\n' << e.GetErrorString();
        CString str2;
        str2 << "Error: " << e.what();
        ::MessageBox(nullptr, str1, str2, MB_ICONERROR);
    }

    // Catch all unhandled std::exception types.
    catch (const std::exception& e)
    {
        // Display the exception and continue.
        CString str1 = e.what();
        ::MessageBox(nullptr, str1, L"Error: std::exception", MB_ICONERROR);
    }

    return 0;
}
