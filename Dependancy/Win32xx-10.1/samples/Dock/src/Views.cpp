/////////////////////////////
// Views.cpp
//

#include "stdafx.h"
#include "Views.h"
#include "resource.h"


/////////////////////////////////
// CViewClasses function definitions
//

// Destructor.
CViewClasses::~CViewClasses()
{
    if (IsWindow())
        DeleteAllItems();
}

// Called when a window handle (HWND) is attached to this object.
void CViewClasses::OnAttach()
{
    // Set the image lists.
    SetDPIImages();

    // Adjust style to show lines and [+] button.
    DWORD dwStyle = GetStyle();
    dwStyle |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
    SetStyle(dwStyle);

    DeleteAllItems();

    // Add some tree-view items.
    HTREEITEM htiRoot = InsertItem(L"TreeView", 0, 0);
    HTREEITEM htiCTreeViewApp = InsertItem(L"CTreeViewApp", 1, 1, htiRoot);
    InsertItem(L"CTreeViewApp()", 3, 3, htiCTreeViewApp);
    InsertItem(L"GetMainFrame()", 3, 3, htiCTreeViewApp);
    InsertItem(L"InitInstance()", 3, 3, htiCTreeViewApp);
    HTREEITEM htiMainFrame = InsertItem(L"CMainFrame", 1, 1, htiRoot);
    InsertItem(L"CMainFrame()", 3, 3, htiMainFrame);
    InsertItem(L"OnCommand()", 4, 4, htiMainFrame);
    InsertItem(L"OnInitialUpdate()", 4, 4, htiMainFrame);
    HTREEITEM htiView = InsertItem(L"CView", 1, 1, htiRoot);
    InsertItem(L"CView()", 3, 3, htiView);
    InsertItem(L"OnInitialUpdate()", 4, 4, htiView);
    InsertItem(L"WndProc()", 4, 4, htiView);

    // Expand some tree-view items.
    Expand(htiRoot, TVE_EXPAND);
    Expand(htiCTreeViewApp, TVE_EXPAND);
}

// Called in response to a WM_DPICHANGED_BEFOREPARENT message which is sent to child
// windows after a DPI change. A WM_DPICHANGED_BEFOREPARENT is only received when the
// application is DPI_AWARENESS_PER_MONITOR_AWARE.
LRESULT CViewClasses::OnDpiChangedBeforeParent(UINT msg, WPARAM wparam, LPARAM lparam)
{
    SetDPIImages();
    return FinalWindowProc(msg, wparam, lparam);
}

// Called when the mouse is clicked on the window.
LRESULT CViewClasses::OnMouseActivate(UINT msg, WPARAM wparam, LPARAM lparam)
{
    // Set window focus. The docker will now report this as active.
    SetFocus();
    return FinalWindowProc(msg, wparam, lparam);
}

// Set the CREATESTURCT parameters before the window is created.
void CViewClasses::PreCreate(CREATESTRUCT& cs)
{
    // Call base clase to set defaults.
    CTreeView::PreCreate(cs);

    if (GetWinVersion() >= 3000)  // Windows 10 or later.
        cs.dwExStyle |= WS_EX_COMPOSITED;
}

// Adjusts the listview image sizes in response to window DPI changes.
void CViewClasses::SetDPIImages()
{
    // Resize the image list.
    CBitmap bmImage(IDB_CLASSVIEW);
    bmImage = DpiScaleUpBitmap(bmImage);
    int scale = bmImage.GetSize().cy / 15;
    CImageList normalImages;
    normalImages.Create(scale * 16, scale * 15, ILC_COLOR32 | ILC_MASK, 1, 0);
    normalImages.Add(bmImage, RGB(255, 0, 0));
    SetImageList(normalImages, TVSIL_NORMAL);

    // Reset the item indentation.
    int imageWidth = normalImages.GetIconSize().cx;
    SetIndent(imageWidth);
}

// Process window messages for the tree-view control.
LRESULT CViewClasses::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case WM_DPICHANGED_BEFOREPARENT: return OnDpiChangedBeforeParent(msg, wparam, lparam);
        case WM_MOUSEACTIVATE:           return OnMouseActivate(msg, wparam, lparam);
        }

        return WndProcDefault(msg, wparam, lparam);
    }

    // Catch all CException types.
    catch (const CException& e)
    {
        // Display the exception and continue.
        CString str;
        str << e.GetText() << L'\n' << e.GetErrorString();
        ::MessageBox(nullptr, str, L"An exception occurred", MB_ICONERROR);

        return 0;
    }
}


/////////////////////////////////
// CViewFiles function definitions
//

// Destructor.
CViewFiles::~CViewFiles()
{
    if (IsWindow()) DeleteAllItems();
}

// Insert 4 list view items.
void CViewFiles::InsertItems()
{
    // Add 4th item.
    int item = InsertItem(0, L"ListViewApp.h", 2);
    SetItemText(item, 1, L"1 KB");
    SetItemText(item, 2, L"C Header file");

    // add 3rd item.
    item = InsertItem(item, L"ListViewApp.cpp", 1);
    SetItemText(item, 1, L"3 KB");
    SetItemText(item, 2, L"C++ Source file");

    // add 2nd item.
    item = InsertItem(item, L"main.cpp", 1);
    SetItemText(item, 1, L"1 KB");
    SetItemText(item, 2, L"C++ Source file");

    // add 1st item.
    item = InsertItem(item, L"ListView", 0);
    SetItemText(item, 2, L"Folder");
}

// Called when a window handle (HWND) is attached to the List-View.
void CViewFiles::OnAttach()
{
    // Call the base class function.
    CListView::OnAttach();

    // Set the image lists.
    SetDPIImages();

    // Set the report style.
    DWORD dwStyle = GetStyle();
    SetStyle((dwStyle & ~LVS_TYPEMASK) | LVS_REPORT);

    SetColumns();
    InsertItems();
    SetDPIColumnWidths();

    // Set the extended style to double buffer.
    SetExtendedStyle(LVS_EX_DOUBLEBUFFER);
}

// Called in response to a WM_DPICHANGED_BEFOREPARENT message which is sent to child
// windows after a DPI change. A WM_DPICHANGED_BEFOREPARENT is only received when the
// application is DPI_AWARENESS_PER_MONITOR_AWARE.
LRESULT CViewFiles::OnDpiChangedBeforeParent(UINT msg, WPARAM wparam, LPARAM lparam)
{
    SetDPIImages();
    SetDPIColumnWidths();
    return FinalWindowProc(msg, wparam, lparam);
}

// Called when the mouse is clicked on the window.
LRESULT CViewFiles::OnMouseActivate(UINT msg, WPARAM wparam, LPARAM lparam)
{
    // Set window focus. The docker will now report this as active.
    SetFocus();
    return FinalWindowProc(msg, wparam, lparam);
}

// Configure the List-view's columns.
void CViewFiles::SetColumns()
{
    // Empty the list.
    DeleteAllItems();

    // Add the column items.
    InsertColumn(0, L"Name");
    InsertColumn(1, L"Size");
    InsertColumn(2, L"Type");
    SetDPIColumnWidths();
}

// Adjusts the listview column widths in response to window DPI changes.
void CViewFiles::SetDPIColumnWidths()
{
    SetColumnWidth(0, DpiScaleInt(120));
    SetColumnWidth(1, DpiScaleInt(50));
    SetColumnWidth(2, DpiScaleInt(100));
}

// Adjusts the listview image sizes in response to window DPI changes.
void CViewFiles::SetDPIImages()
{
    // Set the image lists
    CBitmap bmImage(IDB_FILEVIEW);
    bmImage = DpiScaleUpBitmap(bmImage);
    int scale = bmImage.GetSize().cy / 15;
    CImageList smallImages;
    smallImages.Create(scale * 16, scale * 15, ILC_COLOR32 | ILC_MASK, 1, 0);
    smallImages.Add(bmImage, RGB(255, 0, 255));
    SetImageList(smallImages, LVSIL_SMALL);
}

// Process the listview window messages.
LRESULT CViewFiles::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case WM_DPICHANGED_BEFOREPARENT:  return OnDpiChangedBeforeParent(msg, wparam, lparam);
        case WM_MOUSEACTIVATE:           return OnMouseActivate(msg, wparam, lparam);
        }

        return WndProcDefault(msg, wparam, lparam);
    }

    // Catch all CException types.
    catch (const CException& e)
    {
        // Display the exception and continue.
        CString str;
        str << e.GetText() << L'\n' << e.GetErrorString();
        ::MessageBox(nullptr, str, L"An exception occurred", MB_ICONERROR);

        return 0;
    }
}


///////////////////////////////////
// CViewSimple function definitions
//

// Called when part of the window needs to be redrawn.
void CViewSimple::OnDraw(CDC& dc)
{
    // Use the message font for Windows 7 and higher.
    if (GetWinVersion() >= 2601)
    {
        NONCLIENTMETRICS info = GetNonClientMetrics();
        LOGFONT lf = DpiScaleLogfont(info.lfMessageFont, 10);
        dc.CreateFontIndirect(lf);
    }

    // Centre some text in the window.
    CRect rc = GetClientRect();
    dc.DrawText(L"Simple View", -1, rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

// Respond to a mouse click on the window.
LRESULT CViewSimple::OnMouseActivate(UINT msg, WPARAM wparam, LPARAM lparam)
{
    // Set window focus. The docker will now report this as active.
    SetFocus();
    return FinalWindowProc(msg, wparam, lparam);
}

// Called when the window is resized.
LRESULT CViewSimple::OnSize(UINT, WPARAM, LPARAM)
{
    Invalidate();
    return 0;
}

// Set the CREATESTURCT parameters before the window is created.
void CViewSimple::PreCreate(CREATESTRUCT& cs)
{
    // Call base clase to set defaults.
    CWnd::PreCreate(cs);

    if (GetWinVersion() >= 3000)  // Windows 10 or later.
        cs.dwExStyle |= WS_EX_COMPOSITED;
}

// Process the window's messages.
LRESULT CViewSimple::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case WM_MOUSEACTIVATE:      return OnMouseActivate(msg, wparam, lparam);
        case WM_SIZE:               return OnSize(msg, wparam, lparam);
        }

        return WndProcDefault(msg, wparam, lparam);
    }

    // Catch all CException types.
    catch (const CException& e)
    {
        // Display the exception and continue.
        CString str;
        str << e.GetText() << L'\n' << e.GetErrorString();
        ::MessageBox(nullptr, str, L"An exception occurred", MB_ICONERROR);

        return 0;
    }
}


/////////////////////////////////
// CViewText function definitions
//

// Called when a window handle (HWND) is attached to this object.
void CViewText::OnAttach()
{
    SetDPIFont();
    SetWindowText(L"Text Edit Window\r\n\r\n You can type some text here ...");
}

// Adjusts the font size in response to window DPI changes.
void CViewText::SetDPIFont()
{
    m_font.CreatePointFont(100, L"Courier New");
    m_font = DpiScaleFont(m_font, 9);
    SetFont(m_font);
}

// Handle the window's messages.
LRESULT CViewText::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
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
