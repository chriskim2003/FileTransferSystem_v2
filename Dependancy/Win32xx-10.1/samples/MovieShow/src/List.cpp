///////////////////////////////////////////////////
// List.cpp - Definitions for the CViewList and CDockList classes


#include "stdafx.h"
#include <Uxtheme.h>
#include "List.h"
#include "resource.h"
#include "MovieInfo.h"
#include "UserMessages.h"


//////////////////////////////////
// CViewList function definitions.
//

// Constructor
CViewList::CViewList() : m_oldDPI(0)
{
}

// Destructor.
CViewList::~CViewList()
{
    if (IsWindow()) DeleteAllItems();
}

// Adds an item to the list view.
void CViewList::AddItem(const MovieInfo& mi)
{
    bool isFound = false;
    for (int i = 0; i < GetItemCount(); i++)
        if (GetItemData(i) == (DWORD_PTR)&mi)
            isFound = true;

    if (!isFound)
    {
        int nImage = 0;
        if (mi.videoType == L"Live Performances")
            nImage = 1;
        if (!mi.boxset.IsEmpty())
            nImage = 2;
        if (mi.flags & 0x0001)
            nImage = 3;
        if (mi.flags & 0x0002)
            nImage = 4;
        int item = GetItemCount();

        UINT mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
        LPWSTR text = const_cast<LPWSTR>(mi.movieName.c_str());
        LPARAM lparam = reinterpret_cast<LPARAM>(&mi);
        InsertItem(mask, item, text, 0, 0, nImage, lparam);

        SetItemText(item, 1, mi.releaseDate);
        SetItemText(item, 2, mi.genre);
        SetItemText(item, 3, mi.fileName);
        SetItemText(item, 4, GetFileTime(mi.lastModifiedTime));
    }
}

// Compares lp1 and lp2. Used for sorting.
int CALLBACK CViewList::CompareFunction(LPARAM lp1, LPARAM lp2, LPARAM pSortViewItems)
{
    //  The comparison function must return a negative value if the first
    //  item should precede the second, a positive value if the first item
    //  should follow the second, or zero if the two items are equivalent.

    assert(lp1);
    assert(lp2);
    assert(pSortViewItems);

    if (lp1 == 0 || lp2 == 0 || pSortViewItems == 0)
        return 0;

    SortViewItems* pSort = reinterpret_cast<SortViewItems*>(pSortViewItems);
    MovieInfo* pmi1 = (MovieInfo*)lp1;
    MovieInfo* pmi2 = (MovieInfo*)lp2;

    int compare = 0;

    switch (pSort->m_column)
    {
    case 0:
        {
            if (pmi1->movieName > pmi2->movieName)
                compare = pSort->m_isSortDown ? -1 : 1;

            if (pmi1->movieName < pmi2->movieName)
                compare = pSort->m_isSortDown ? 1 : -1;
            break;
        }
    case 1:
        {
            if (pmi1->releaseDate > pmi2->releaseDate)
                compare = pSort->m_isSortDown ? -1 : 1;

            if (pmi1->releaseDate < pmi2->releaseDate)
                compare = pSort->m_isSortDown ? 1 : -1;
            break;
        }
    case 2:
        {
            if (pmi1->genre > pmi2->genre)
                compare = pSort->m_isSortDown ? -1 : 1;

            if (pmi1->genre < pmi2->genre)
                compare = pSort->m_isSortDown ? 1 : -1;
            break;
        }
    case 3:
        {
            if (pmi1->fileName > pmi2->fileName)
                compare = pSort->m_isSortDown ? -1 : 1;

            if (pmi1->fileName < pmi2->fileName)
                compare = pSort->m_isSortDown ? 1 : -1;
            break;
        }
    case 4:
    {
        if (CTime(pmi1->lastModifiedTime) > CTime(pmi2->lastModifiedTime))
            compare = pSort->m_isSortDown ? -1 : 1;

        if (CTime(pmi1->lastModifiedTime) < CTime(pmi2->lastModifiedTime))
            compare = pSort->m_isSortDown ? 1 : -1;
        break;
    }
    }

    return compare;
}

// Returns a CString containing the file's date and time.
CString CViewList::GetFileTime(FILETIME fileTime)
{
    // Convert the file time to local time.
    FILETIME localFileTime;
    SYSTEMTIME localSysTime;
    ::FileTimeToLocalFileTime(&fileTime, &localFileTime);
    ::FileTimeToSystemTime(&localFileTime, &localSysTime);

    // Build strings with the date and time with regional settings.
    const int maxChars = 32;
    WCHAR time[maxChars];
    WCHAR date[maxChars];
    ::GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &localSysTime, nullptr, date, maxChars - 1);
    ::GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &localSysTime, nullptr, time, maxChars - 1);

    // Combine the date and time in a CString.
    CString str = date;
    str += L" ";
    str += time;
    return str;
}

// Called when the listview window is attached to CViewList during Create.
void CViewList::OnAttach()
{
    // Call the base class function.
    CListView::OnAttach();

    SetWindowTheme(L"Explorer", nullptr);

    // Set the report style.
    DWORD dwStyle = GetStyle();
    SetStyle((dwStyle & ~LVS_TYPEMASK) | LVS_REPORT);

    SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
                      | LVS_EX_HEADERDRAGDROP | LVS_EX_DOUBLEBUFFER );

    SetColumn();

    // Disable double click on the list-view's header.
    m_header.Attach(GetHeader());
    LONG_PTR style = static_cast<LONG_PTR>(m_header.GetClassLongPtr(GCL_STYLE));
    style = style & ~CS_DBLCLKS;
    m_header.SetClassLongPtr(GCL_STYLE, style);
}

// Call to perform custom drawing.
LRESULT CViewList::OnCustomDraw(LPNMCUSTOMDRAW pCustomDraw)
{
    assert(pCustomDraw);

    switch (pCustomDraw->dwDrawStage)
    {
    case CDDS_PREPAINT: // Before the paint cycle begins.
        // Request notifications for individual header items.
        return CDRF_NOTIFYITEMDRAW;

    case CDDS_ITEMPREPAINT: // Before an item is drawn
    {
        // Get an appropriate color for the header
        COLORREF color = GetSysColor(COLOR_BTNFACE);
        HWND hFrame = GetAncestor();
        ReBarTheme* pTheme = reinterpret_cast<ReBarTheme*>(::SendMessage(hFrame, UWM_GETRBTHEME, 0, 0));
        if (pTheme && pTheme->UseThemes && pTheme->clrBand2 != 0)
            color = pTheme->clrBkgnd2;

        // Set the background color of the header.
        CBrush br(color);
        ::FillRect(pCustomDraw->hdc, &pCustomDraw->rc, br);

        // Also set the text background color.
        ::SetBkColor(pCustomDraw->hdc, color);

        return CDRF_DODEFAULT;
    }
    }

    return CDRF_DODEFAULT;
}

// Called in response to a WM_DPICHANGED_BEFOREPARENT message that is sent to child
// windows after a DPI change. A WM_DPICHANGED_BEFOREPARENT is only received when the
// application is DPI_AWARENESS_PER_MONITOR_AWARE.
LRESULT CViewList::OnDpiChangedBeforeParent(UINT msg, WPARAM wparam, LPARAM lparam)
{
    // Adjust the column width in response to window DPI changes.
    SetRedraw(FALSE);
    int lastCol = Header_GetItemCount(GetHeader()) - 1;
    int dpi = GetWindowDpi(*this);
    for (int i = 0; i <= lastCol; ++i)
    {
        int width = GetColumnWidth(i) * dpi / m_oldDPI;
        SetColumnWidth(i, width);
    }

    m_oldDPI = GetWindowDpi(*this);

    // Adjust the images in response to window DPI changes.
    SetDPIImages();

    SetRedraw(TRUE);
    RedrawWindow();

    return FinalWindowProc(msg, wparam, lparam);
}

// Called after the listview window is created.
void CViewList::OnInitialUpdate()
{
    m_oldDPI = GetWindowDpi(*this);
    SetDPIImages();
}

// Called with a double click left mouse button or press the Enter key.
// on a list view item.
LRESULT CViewList::OnItemActivate(LPNMLISTVIEW pListView)
{
    assert(pListView);

    int item = pListView->iItem;
    MovieInfo* pmi = reinterpret_cast<MovieInfo*>(GetItemData(item));
    assert(pmi);
    if (pmi)
    {
        LPCWSTR movie = pmi->fileName.c_str();
        GetAncestor().SendMessage(UWM_PLAYMOVIE, (WPARAM)movie, 0);
    }

    return 0;
}

// Called when a list view item is selected.
LRESULT CViewList::OnItemChanged(LPNMLISTVIEW pListView)
{
    assert(pListView);

    int item = pListView->iItem;
    MovieInfo* pmi = reinterpret_cast<MovieInfo*>(GetItemData(item));
    GetAncestor().SendMessage(UWM_ONSELECTLISTITEM, (WPARAM)pmi, 0);

    return 0;
}

// Called when a list view column is clicked.
LRESULT CViewList::OnLVColumnClick(LPNMLISTVIEW pListView)
{
    assert(pListView);

    // Determine the required sort order.
    HDITEM  hdrItem{};
    hdrItem.mask = HDI_FORMAT;
    int column = pListView->iSubItem;
    VERIFY(Header_GetItem(GetHeader(), column, &hdrItem));
    bool isSortDown = (hdrItem.fmt & HDF_SORTUP) ? true : false;

    // Perform the sort.
    SortColumn(column, isSortDown);

    return 0;
}

// Respond to notifications from child windows,
LRESULT CViewList::OnNotify(WPARAM, LPARAM lparam)
{
    LPNMCUSTOMDRAW  pCustomDraw = (LPNMCUSTOMDRAW)lparam;
    assert(pCustomDraw);

    switch (pCustomDraw->hdr.code)
    {
    case NM_CUSTOMDRAW:          return OnCustomDraw(pCustomDraw);
    }

    return 0;
}

// Called when the framework reflects the WM_NOTIFY message
// back to CViewList.
LRESULT CViewList::OnNotifyReflect(WPARAM, LPARAM lparam)
{
    LPNMLISTVIEW pListView = (LPNMLISTVIEW)lparam;
    assert(pListView);

    switch (pListView->hdr.code)
    {
    case LVN_COLUMNCLICK:          return OnLVColumnClick(pListView);
    case LVN_ITEMACTIVATE:         return OnItemActivate(pListView);
    case LVN_ITEMCHANGED:          return OnItemChanged(pListView);
    case NM_RCLICK:                return OnRClick();
    }

    return 0;
}

// Called with a right mouse click on one or more list view items.
LRESULT CViewList::OnRClick()
{
    if (GetNextItem(-1, LVNI_SELECTED) >= 0)
        GetAncestor().SendMessage(UWM_ONRCLICKLISTITEM, 0, 0);

    return 0;
}

// Called when the window is resized.
LRESULT CViewList::OnWindowPosChanged(UINT msg, WPARAM wparam, LPARAM lparam)
{
    // Adjust the list view's columns when the window is resized.
    SetLastColumnWidth();

    return FinalWindowProc(msg, wparam, lparam);
}

// Sets the CREATESTRUCT parameters prior to the window's creation.
void CViewList::PreCreate(CREATESTRUCT& cs)
{
    CListView::PreCreate(cs);
    cs.dwExStyle |= WS_EX_CLIENTEDGE;
    cs.style |= LVS_SHOWSELALWAYS | LVS_ALIGNLEFT;
}

// Configures the columns in the list view's header.
void CViewList::SetColumn()
{
    // Initialize the columns
    LV_COLUMN lvColumn{};
    lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvColumn.fmt = LVCFMT_LEFT;

    lvColumn.cx = DpiScaleInt(200);
    lvColumn.pszText = const_cast<LPWSTR>(L"Movie Title");
    InsertColumn(0, lvColumn);

    lvColumn.cx = DpiScaleInt(100);
    lvColumn.pszText = const_cast<LPWSTR>(L"Release Date");
    InsertColumn(1, lvColumn);

    lvColumn.cx = DpiScaleInt(120);
    lvColumn.pszText = const_cast<LPWSTR>(L"Genre");
    InsertColumn(2, lvColumn);

    lvColumn.cx = DpiScaleInt(150);
    lvColumn.pszText = const_cast<LPWSTR>(L"File Name");
    InsertColumn(3, lvColumn);

    lvColumn.cx = DpiScaleInt(150);
    lvColumn.pszText = const_cast<LPWSTR>(L"File Date");
    InsertColumn(4, lvColumn);
}

// Adjusts the listview image sizes in response to window DPI changes.
void CViewList::SetDPIImages()
{
    // Set the image lists
    int size = DpiScaleInt(16);
    CImageList smallImages;
    smallImages.Create(size, size, ILC_COLOR32, 1, 0);
    smallImages.AddIcon(IDI_MOVIES);
    smallImages.AddIcon(IDI_VIOLIN);
    smallImages.AddIcon(IDI_BOXSET);
    smallImages.AddIcon(IDI_FAVOURITES);
    smallImages.AddIcon(IDI_EYE);
    SetImageList(smallImages, LVSIL_SMALL);
}

// Sets the up and down sort arrows in the listview's header.
BOOL CViewList::SetHeaderSortImage(int  columnIndex, int showArrow)
{
    HWND    hHeader = nullptr;
    HDITEM  hdrItem{};

    hHeader = GetHeader();
    if (hHeader)
    {
        hdrItem.mask = HDI_FORMAT;

        if (Header_GetItem(hHeader, columnIndex, &hdrItem))
        {
            if (showArrow == SHOW_UP_ARROW)
            {
                hdrItem.fmt = (hdrItem.fmt & ~HDF_SORTDOWN) | HDF_SORTUP;
            }
            else if (showArrow == SHOW_DOWN_ARROW)
            {
                hdrItem.fmt = (hdrItem.fmt & ~HDF_SORTUP) | HDF_SORTDOWN;
            }
            else
            {
                hdrItem.fmt = hdrItem.fmt & ~(HDF_SORTDOWN | HDF_SORTUP);
            }

            return Header_SetItem(hHeader, columnIndex, &hdrItem);
        }
    }

    return FALSE;
}

// Sets the width of the last column to match the window width.
void CViewList::SetLastColumnWidth()
{
    if (IsWindow())
    {
        int remainingWidth = GetClientRect().Width();
        int lastCol = Header_GetItemCount(GetHeader()) - 1;
        for (int i = 0; i < lastCol; i++)
        {
            remainingWidth -= GetColumnWidth(i);
        }

        SetColumnWidth(lastCol, std::max(remainingWidth, 100));
    }
}

// Called when the user clicks on a column in the listview's header.
void CViewList::SortColumn(int column, bool isSortDown)
{
    // Perform the sort.
    SortViewItems sort{column, isSortDown};
    SortItems(CompareFunction, (LPARAM)&sort);

    // Ensure the selected item is visible after sorting.
    int itemint = GetNextItem(-1, LVNI_SELECTED);
    EnsureVisible(itemint, FALSE);

    // Add an arrow to the column header.
    for (int col = 0; col < Header_GetItemCount(GetHeader()); col++)
        SetHeaderSortImage(col, SHOW_NO_ARROW);

    SetHeaderSortImage(column, isSortDown ? SHOW_DOWN_ARROW : SHOW_UP_ARROW);

    // Select the previously selected or first item
    if (GetSelectionMark() > 0)
        SetItemState(GetSelectionMark(), LVIS_FOCUSED | LVIS_SELECTED, 0x000F);
    else
        SetItemState(0, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);
}

// Sets the image associated with the listview item.
void CViewList::UpdateItemImage(int item)
{
    MovieInfo* mi = (MovieInfo*)GetItemData(item);

    if (mi)
    {
        int nImage = 0;
        if (mi->videoType == L"Live Performances")
            nImage = 1;
        if (!mi->boxset.IsEmpty())
            nImage = 2;
        if (mi->flags & 0x0001)
            nImage = 3;
        if (mi->flags & 0x0002)
            nImage = 4;

        LVITEM lvi{};
        lvi.mask = LVIF_IMAGE;
        lvi.iImage = nImage;
        lvi.iItem = item;

        SetItem(lvi);
    }
}

LRESULT CViewList::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case WM_WINDOWPOSCHANGED:        return OnWindowPosChanged(msg, wparam, lparam);
        case WM_DPICHANGED_BEFOREPARENT: return OnDpiChangedBeforeParent(msg, wparam, lparam);
        }

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

