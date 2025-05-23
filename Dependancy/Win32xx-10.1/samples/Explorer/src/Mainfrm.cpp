/////////////////////////////
// Mainfrm.cpp
//

#include "stdafx.h"
#include "Mainfrm.h"
#include "resource.h"

//////////////////////////////////
// CMainFrame function definitions
//

// Constructor.
CMainFrame::CMainFrame()  : m_pLeftPane(0), m_showHidden(FALSE)
{
}

// Create the frame window.
HWND CMainFrame::Create(HWND parent)
{
    // Set m_rightPane as the view window of the frame.
    SetView(m_rightPane);

    // Set the registry key name, and load the initial window position.
    // Use a registry key name like "CompanyName\\Application".
    LoadRegistrySettings(L"Win32++\\Explorer Sample");

    return CFrame::Create(parent);
}

// Creates the popup menu for the "View Menu" toolbar button.
void CMainFrame::DoPopupMenu()
{
    // Position the popup menu
    CToolBar& tb = GetToolBar();
    CRect rc = tb.GetItemRect(tb.CommandToIndex(IDM_VIEWMENU));
    tb.MapWindowPoints(nullptr, (LPPOINT)&rc, 2);

    TPMPARAMS tpm;
    tpm.cbSize = sizeof(tpm);
    tpm.rcExclude = rc;

    // Load the popup menu
    CMenu topMenu(IDM_VIEWMENU);
    CMenu popupMenu = topMenu.GetSubMenu(0);

    // Put a radio check in the currently checked item.
    MENUITEMINFO mii{};
    for (int i = 3 ; i < 7 ; i++)
    {
        mii = {};
        mii.cbSize = sizeof(mii);
        mii.fMask  = MIIM_STATE | MIIM_ID;
        CMenu subMenu = GetFrameMenu().GetSubMenu(1);
        subMenu.GetMenuItemInfo(i, mii, TRUE);
        if (mii.fState & MFS_CHECKED)
            topMenu.CheckMenuRadioItem(IDM_VIEW_SMALLICON, IDM_VIEW_REPORT, mii.wID, MF_BYCOMMAND);
    }

    // Start the popup menu.
    popupMenu.TrackPopupMenuEx(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, rc.left, rc.bottom, *this, &tpm);
}

// Load the default arrangement of the window panes.
void CMainFrame::LoadDefaultWindowPanes()
{
    // Add the right window pane
    int width = GetWindowRect().Width() / 3;
    DWORD dockStyle = DS_DOCKED_LEFT | DS_NO_UNDOCK | DS_NO_CAPTION;
    m_pLeftPane = m_rightPane.AddDockedChild(
        std::make_unique<CLeftPane>(), dockStyle, width, ID_DOCK_LEFTPANE);
}

// Process input from the menu and toolbar.
BOOL CMainFrame::OnCommand(WPARAM wparam, LPARAM)
{
    UINT id = LOWORD(wparam);
    switch (id)
    {
    case IDM_FILE_EXIT:         return OnFileExit();
    case IDM_HELP_ABOUT:        return OnHelp();
    case IDW_VIEW_STATUSBAR:    return OnViewStatusBar();
    case IDW_VIEW_TOOLBAR:      return OnViewToolBar();
    case IDM_VIEW_LARGEICON:    return OnViewLargeIcon();
    case IDM_VIEW_SMALLICON:    return OnViewSmallIcon();
    case IDM_VIEW_LIST:         return OnViewList();
    case IDM_VIEW_REPORT:       return OnViewReport();
    case IDM_SHOW_HIDDEN:       return OnShowHidden();
    case IDM_VIEWMENU:          return OnViewMenu();
    }

    return FALSE;
}

// OnCreate controls the way the frame is created.
// Overriding CFrame::OnCreate is optional.
int CMainFrame::OnCreate(CREATESTRUCT& cs)
{
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

    // call the base class function
    return CFrame::OnCreate(cs);
}

// Issue a close request to the frame to end the program.
BOOL CMainFrame::OnFileExit()
{
    Close();
    return TRUE;
}

void CMainFrame::LoadListViewRegistrySettings()
{
    CString appName = GetRegistryKeyName();

    if (!appName.IsEmpty())
    {
        CString listViewKeyName = L"\\ListView Settings";
        try
        {
            CString keyName = L"Software\\" + appName + listViewKeyName;
            CRegKey key;

            if (ERROR_SUCCESS != key.Create(HKEY_CURRENT_USER, keyName))
                throw CUserException(L"RegCreateKeyEx failed");
            if (ERROR_SUCCESS != key.Open(HKEY_CURRENT_USER, keyName))
                throw CUserException(L"RegCreateKeyEx failed");

            DWORD columns[4];

            if (ERROR_SUCCESS != key.QueryDWORDValue(L"Column0", columns[0]))
                throw CUserException();
            if (ERROR_SUCCESS != key.QueryDWORDValue(L"Column1", columns[1]))
                throw CUserException();
            if (ERROR_SUCCESS != key.QueryDWORDValue(L"Column2", columns[2]))
                throw CUserException();
            if (ERROR_SUCCESS != key.QueryDWORDValue(L"Column3", columns[3]))
                throw CUserException();

            for (int i = 0; i < 4; i++)
            {
                HDITEM headerItem{};
                headerItem.mask = HDI_WIDTH;
                headerItem.cxy = columns[i];
                GetListView().GetListHeader().SetItem(i, headerItem);
            }
        }

        catch (const  CUserException&)
        {
            CString keyName = L"Software\\" + appName;
            CRegKey key;

            if (ERROR_SUCCESS == key.Open(HKEY_CURRENT_USER, keyName))
            {
                // Roll back the registry changes by deleting this subkey.
                key.DeleteSubKey(listViewKeyName);
            }
        }
    }
}

// Limit the minimum size of the window.
LRESULT CMainFrame::OnGetMinMaxInfo(UINT msg, WPARAM wparam, LPARAM lparam)
{
    LPMINMAXINFO lpMMI = (LPMINMAXINFO)lparam;
    const CSize minimumSize(300, 250);
    lpMMI->ptMinTrackSize.x = DpiScaleInt(minimumSize.cx);
    lpMMI->ptMinTrackSize.y = DpiScaleInt(minimumSize.cy);
    return FinalWindowProc(msg, wparam, lparam);
}


// Called after the frame window is created.
void CMainFrame::OnInitialUpdate()
{
    if (m_rightPane.LoadDockRegistrySettings(GetRegistryKeyName()))
    {
        m_pLeftPane = m_rightPane.GetDockFromID(ID_DOCK_LEFTPANE);
        if (m_pLeftPane == 0)
        {
            m_rightPane.CloseAllDockers();
            LoadDefaultWindowPanes();
        }
    }
    else
        LoadDefaultWindowPanes();

    // Load the ListView column widths from the registry.
    LoadListViewRegistrySettings();

    // All windows are now created, so populate the treeview.
    GetTreeView().GetRootItems();

    // Uncheck the hidden menu item
    GetFrameMenu().CheckMenuItem(IDM_SHOW_HIDDEN, MF_UNCHECKED);

    // Place Radio button in view menu
    CMenu viewMenu = GetFrameMenu().GetSubMenu(1);
    viewMenu.CheckMenuRadioItem(IDM_VIEW_SMALLICON, IDM_VIEW_REPORT, IDM_VIEW_REPORT, MF_BYCOMMAND);
}

// Process notification messages (WM_NOTIFY) from child windows.
LRESULT CMainFrame::OnNotify(WPARAM wparam, LPARAM lparam)
{
    // Notification from our dropdown button is received if Comctl32.dll version
    // is 4.70 or later (IE v3 required).
    LPNMHDR pHeader = reinterpret_cast<LPNMHDR>(lparam);
    switch (pHeader->code)
    {
        //Menu for dropdown toolbar button
        case TBN_DROPDOWN:
        {
            if (pHeader->hwndFrom == GetToolBar())
                DoPopupMenu();
        }

    } //switch LPNMHDR

    return CFrame::OnNotify(wparam, lparam);
}

// Toggle the show state of hidden items.
BOOL CMainFrame::OnShowHidden()
{
    // Handle the the View submenu
    CMenu viewMenu = GetFrameMenu().GetSubMenu(1);

    m_showHidden = !m_showHidden;
    viewMenu.CheckMenuItem(IDM_SHOW_HIDDEN, (TRUE == m_showHidden)? MF_CHECKED : MF_UNCHECKED);

    // Refresh the Listview display
    GetListView().DoDisplay();
    return TRUE;
}

// Change the view mode to large icon.
BOOL CMainFrame::OnViewLargeIcon()
{
    // Handle the the View submenu
    CMenu ViewMenu = GetFrameMenu().GetSubMenu(1);

    GetListView().ViewLargeIcons();
    ViewMenu.CheckMenuRadioItem(IDM_VIEW_SMALLICON, IDM_VIEW_REPORT, IDM_VIEW_LARGEICON, MF_BYCOMMAND);
    return TRUE;
}

// Change the view mode to list files.
BOOL CMainFrame::OnViewList()
{
    // Handle the the View submenu
    CMenu viewMenu = GetFrameMenu().GetSubMenu(1);

    GetListView().ViewList();
    viewMenu.CheckMenuRadioItem(IDM_VIEW_SMALLICON, IDM_VIEW_REPORT, IDM_VIEW_LIST, MF_BYCOMMAND);
    return TRUE;
}

// Initiate a popup menu to select the view mode.
BOOL CMainFrame::OnViewMenu()
{
    // This Command is received if Comctl32.dll version is below 4.7
    DoPopupMenu();
    return TRUE;
}

// Change the view mode to report.
BOOL CMainFrame::OnViewReport()
{
    // Handle the the View submenu
    CMenu viewMenu = GetFrameMenu().GetSubMenu(1);

    GetListView().ViewReport();
    viewMenu.CheckMenuRadioItem(IDM_VIEW_SMALLICON, IDM_VIEW_REPORT, IDM_VIEW_REPORT, MF_BYCOMMAND);
    return TRUE;
}

// Change the view mode to small icon.
BOOL CMainFrame::OnViewSmallIcon()
{
    // Handle the the View submenu
    CMenu viewMenu = GetFrameMenu().GetSubMenu(1);

    GetListView().ViewSmallIcons();
    viewMenu.CheckMenuRadioItem(IDM_VIEW_SMALLICON, IDM_VIEW_REPORT, IDM_VIEW_SMALLICON, MF_BYCOMMAND);
    return TRUE;
}

BOOL CMainFrame::SaveRegistrySettings()
{
    if (CFrame::SaveRegistrySettings())
    {
        if (m_rightPane.SaveDockRegistrySettings(GetRegistryKeyName()))
        {
            CString appName = GetRegistryKeyName();

            if (!appName.IsEmpty())
            {
                CString listViewKeyName = L"\\ListView Settings";
                try
                {
                    CString keyName = L"Software\\" + appName + listViewKeyName;
                    CRegKey key;

                    if (ERROR_SUCCESS != key.Create(HKEY_CURRENT_USER, keyName))
                        throw CUserException(L"RegCreateKeyEx failed");
                    if (ERROR_SUCCESS != key.Open(HKEY_CURRENT_USER, keyName))
                        throw CUserException(L"RegCreateKeyEx failed");

                    DWORD columns[4];
                    for (int i = 0; i < 4; i++)
                    {
                        HDITEM headerItem{};
                        headerItem.mask = HDI_WIDTH;
                        GetListView().GetListHeader().GetItem(i, headerItem);
                        columns[i] = headerItem.cxy;
                    }

                    if (ERROR_SUCCESS != key.SetDWORDValue(L"Column0", columns[0]))
                        throw CUserException();
                    if (ERROR_SUCCESS != key.SetDWORDValue(L"Column1", columns[1]))
                        throw CUserException();
                    if (ERROR_SUCCESS != key.SetDWORDValue(L"Column2", columns[2]))
                        throw CUserException();
                    if (ERROR_SUCCESS != key.SetDWORDValue(L"Column3", columns[3]))
                        throw CUserException();

                    return TRUE;
                }

                catch (const  CUserException&)
                {
                    CString keyName = L"Software\\" + appName;
                    CRegKey key;

                    if (ERROR_SUCCESS == key.Open(HKEY_CURRENT_USER, keyName))
                    {
                        // Roll back the registry changes by deleting this subkey.
                        key.DeleteSubKey(listViewKeyName);
                    }
                }
            }
        }
    }

    return FALSE;
}

// Specifies the images for some of the menu items.
void CMainFrame::SetupMenuIcons()
{
    std::vector<UINT> data = GetToolBarData();
    if (GetMenuIconHeight() >= 24)
        SetMenuIcons(data, RGB(192, 192, 192), IDW_MAIN, IDB_TOOLBAR_DIS);
    else
        SetMenuIcons(data, RGB(192, 192, 192), IDB_TOOLBAR16);
}

// Define our toolbar.
void CMainFrame::SetupToolBar()
{
    AddToolBarButton( IDM_FILE_NEW  , FALSE, L"New" );
    AddToolBarButton( IDM_FILE_OPEN , FALSE, L"Open" );
    AddToolBarButton( IDM_FILE_SAVE , FALSE, L"Save" );

    AddToolBarButton( 0 );  // Separator
    AddToolBarButton( IDM_EDIT_CUT  , FALSE, L"Cut" );
    AddToolBarButton( IDM_EDIT_COPY , FALSE, L"Copy" );
    AddToolBarButton( IDM_EDIT_PASTE, FALSE, L"Paste" );
    AddToolBarButton( IDM_FILE_PRINT, FALSE, L"Print" );

    AddToolBarButton( 0 );  // Separator
    AddToolBarButton( IDM_VIEWMENU,   TRUE,  L"View" );

    AddToolBarButton( 0 );  // Separator
    AddToolBarButton( IDM_HELP_ABOUT, TRUE,  L" About " );

    // Use larger buttons
    SetToolBarImages(RGB(192,192,192), IDW_MAIN, IDB_TOOLBAR_HOT, IDB_TOOLBAR_DIS);

    // Configure the ViewMenu button to bring up a menu
    GetToolBar().SetButtonStyle(IDM_VIEWMENU, BTNS_WHOLEDROPDOWN);
}

// Process the window messages for the frame.
LRESULT CMainFrame::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case WM_GETMINMAXINFO:    return OnGetMinMaxInfo(msg, wparam, lparam);
        }

        // pass any unhandled messages on for default processing
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

