/////////////////////////////
// MyListView.cpp
//

#include "stdafx.h"
#include "ExplorerApp.h"
#include "MyListView.h"
#include "resource.h"

#ifndef HDF_SORTUP
#define HDF_SORTUP              0x0400
#define HDF_SORTDOWN            0x0200
#endif


//////////////////////////////////
// CMyListView function definitions
//

// Compares param1 and param2. Used to sort items in a list view.
int CALLBACK CMyListView::CompareFunction(LPARAM param1, LPARAM param2, LPARAM pSortViewItems)
{
    assert(param1);
    assert(param2);
    assert(pSortViewItems);

    if (param1 == 0 || param2 == 0 || pSortViewItems == 0)
        return 0;

    ListItemData* pItem1 = reinterpret_cast<ListItemData*>(param1);
    ListItemData* pItem2 = reinterpret_cast<ListItemData*>(param2);
    SortViewItems* pSort = reinterpret_cast<SortViewItems*>(pSortViewItems);

    int compare = 0;

    switch (pSort->m_column)
    {
    case 0:  // Sort by file name using IShellFolder::CompareIDs.
    {
        HRESULT result = pItem1->GetParentFolder().CompareIDs(0, pItem1->GetRelCpidl(), pItem2->GetRelCpidl());
        if (FAILED(result))
            break;

        short value = (short)SCODE_CODE(GetScode(result));
        compare = pSort->m_isSortDown ? value : -value;
        break;
    }
    case 1:  // Sort by file size.
    {
        if (pItem1->m_fileSize > pItem2->m_fileSize)
            compare = pSort->m_isSortDown ? 1 : -1;

        if (pItem1->m_fileSize < pItem2->m_fileSize)
            compare = pSort->m_isSortDown ? -1 : 1;

        if (pItem1->m_isFolder && !pItem2->m_isFolder)
            compare = pSort->m_isSortDown ? -1 : 1;

        if (!pItem1->m_isFolder && pItem2->m_isFolder)
            compare = pSort->m_isSortDown ? 1 : -1;

        break;
    }
    case 2:  // Sort by file type.
    {
        if (pItem1->m_fileType > pItem2->m_fileType)
            compare = pSort->m_isSortDown ? 1 : -1;

        if (pItem1->m_fileType < pItem2->m_fileType)
            compare = pSort->m_isSortDown ? -1 : 1;

        if (pItem1->m_isFolder && !pItem2->m_isFolder)
            compare = pSort->m_isSortDown ? -1 : 1;

        if (!pItem1->m_isFolder && pItem2->m_isFolder)
            compare = pSort->m_isSortDown ? 1 : -1;

        break;
    }
    case 3:  // Sort by modified time
    {
        ULONGLONG t1 = FileTimeToULL(pItem1->m_fileTime);
        ULONGLONG t2 = FileTimeToULL(pItem2->m_fileTime);

        if (t1 > t2)
            compare = pSort->m_isSortDown ? 1 : -1;

        if (t1 < t2)
            compare = pSort->m_isSortDown ? -1 : 1;

        if (pItem1->m_isFolder && !pItem2->m_isFolder)
            compare = pSort->m_isSortDown ? -1 : 1;

        if (!pItem1->m_isFolder && pItem2->m_isFolder)
            compare = pSort->m_isSortDown ? 1 : -1;

        break;
    }
    }

    return compare;
}

// Displays the contents of the specified folder.
void CMyListView::DisplayFolder(CShellFolder& parentFolder, Cpidl& cpidlFull, Cpidl& cpidlRel)
{
    m_cpidlCurFull = cpidlFull;
    if(parentFolder.GetIShellFolder())
        parentFolder.GetSubFolder(cpidlRel, m_csfCurFolder);
    else
        m_csfCurFolder.DesktopFolder();

    DoDisplay();
}

// Initiates the popup menu in response to a right mouse click on the list-view background.
void CMyListView::DoBackgroundMenu(CPoint& point)
{
    HRESULT result;
    if(m_csfCurFolder.GetIShellFolder())
    {
        CContextMenu ccm;
        result = m_csfCurFolder.GetContextMenu(*this, ccm);

        if(SUCCEEDED(result))
        {
            CMenu popup;
            popup.CreatePopupMenu();
            if(popup.GetHandle())
            {
                int   i = 0;
                UINT  idCmdFirst = 0;
                UINT  idCmd;

                // Find the largest ID in the menu.
                while((idCmd = popup.GetMenuItemID(i)) != static_cast<UINT>(-1))
                {
                    if(idCmd > idCmdFirst)
                        idCmdFirst = idCmd;
                    i++;
                }

                result = ccm.QueryContextMenu(popup, 0, ++idCmdFirst, static_cast<UINT>(-1), CMF_NORMAL | CMF_EXPLORE);

                if(SUCCEEDED(result))
                {
                    ccm.GetContextMenu2(m_ccm2);

                    idCmd = popup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON,
                        point.x, point.y, *this, nullptr);

                    if(idCmd)
                    {
                        CMINVOKECOMMANDINFO  cmi{};
                        cmi.cbSize = sizeof(cmi);
                        cmi.hwnd = GetParent();
                        cmi.lpVerb = (LPCSTR)(INT_PTR)(idCmd - idCmdFirst);
                        cmi.nShow = SW_SHOWNORMAL;
                        ccm.Invoke(cmi);

                        // The operation performed by the context menu may have
                        // changed the contents of the folder, so do a refresh.
                        DoDisplay();
                    }

                    m_ccm2.Release();
                }
            }
        }
    }
}

// Initiates the popup menu in response to a right mouse click on the list-view.
// Calls either DoItemMenu or DoBackgroundMenu.
void CMyListView::DoContextMenu(CPoint& point)
{
    LVHITTESTINFO  lvhti;
    lvhti.pt = point;
    ScreenToClient(lvhti.pt);
    lvhti.flags = LVHT_NOWHERE;
    HitTest(lvhti);

    if(LVHT_ONITEM & lvhti.flags)
    {
        // Get the selected items.
        UINT  items = GetSelectedCount();
        std::vector<int> vItems(items, 0);
        int* pItemArray = vItems.data();

        if(pItemArray)
        {
            UINT  i;
            int   curItem;

            // Put the item clicked on first in the list.
            pItemArray[0] = lvhti.iItem;

            for(i = 1, curItem = -1; i < items; ++i)
            {
                curItem = GetNextItem(curItem, LVNI_SELECTED);
                if(curItem != lvhti.iItem)
                    pItemArray[i] = curItem;
                else
                    --i;
            }

            DoItemMenu(pItemArray, items, point);
        }
    }
    else
        DoBackgroundMenu(point);
}

// Performs the default action on the list-view item.
// Typically that would open a folder or run a file.
void CMyListView::DoDefault(int item)
{
    LVITEM lvItem{};
    lvItem.mask = LVIF_PARAM;
    lvItem.iItem = item;

    if(GetItem(lvItem))
    {
        HRESULT        result;
        ListItemData*  pInfo = reinterpret_cast<ListItemData*>(lvItem.lParam);
        CShellFolder   folder;
        CContextMenu ccm;
        folder = pInfo->GetParentFolder();

        if (!folder.GetIShellFolder())
            folder.DesktopFolder();

        if(folder.GetIShellFolder())
        {
            LPCITEMIDLIST pidl = pInfo->GetRelCpidl();
            result = folder.GetContextMenu(*this, 1, &pidl, ccm);

            if(SUCCEEDED(result))
            {
                CMenu popup;
                popup.CreatePopupMenu();
                if(popup.GetHandle())
                {
                    result = ccm.QueryContextMenu(popup, 0, 1, 0x7fff, CMF_DEFAULTONLY | CMF_EXPLORE);

                    if(SUCCEEDED(result))
                    {
                        UINT idCmd = popup.GetMenuItemID(0);
                        if(idCmd && (idCmd != static_cast<UINT>(-1)))
                        {
                            //determine if the item is a folder
                            ULONG ulAttr = SFGAO_HASSUBFOLDER | SFGAO_FOLDER;
                            folder.GetAttributes(1, pInfo->GetRelCpidl(), ulAttr);

                            if ((ulAttr & SFGAO_HASSUBFOLDER) || (ulAttr &SFGAO_FOLDER))
                            {
                                CMainFrame& mainFrame = GetExplorerApp()->GetMainFrame();
                                mainFrame.GetTreeView().SelectFromListView(pInfo->GetFullCpidl());
                            }
                            else
                            {
                                CMINVOKECOMMANDINFO  cmi{};
                                cmi.cbSize = sizeof(CMINVOKECOMMANDINFO);
                                cmi.hwnd = GetParent();
                                cmi.lpVerb = (LPCSTR)(INT_PTR)(idCmd - 1);
                                cmi.nShow = SW_SHOWNORMAL;
                                ccm.Invoke(cmi);
                            }
                        }
                    }
                }
            }
        }
    }
}

// Enumerates the items in the currently selected folder and displays them.
void CMyListView::DoDisplay()
{
    DeleteAllItems();
    m_pItems.clear();

    if(m_csfCurFolder.GetIShellFolder())
    {
        HCURSOR  hCur = ::LoadCursor(nullptr, IDC_WAIT);
        hCur = ::SetCursor(hCur);

        // Turn redrawing off in the ListView.
        // This will speed things up as we add items.
        SetRedraw(FALSE);

        EnumObjects(m_csfCurFolder, m_cpidlCurFull);
        SortViewItems sort(0, TRUE);
        SortItems(CompareFunction, (LPARAM)&sort);

        // Turn redrawing back on.
        SetRedraw(TRUE);
        ::SetCursor(hCur);
    }
}

// Initiates the popup menu in response to a right mouse click on a list-view item.
void CMyListView::DoItemMenu(LPINT pItems, UINT items, CPoint& point)
{
    std::vector<LPCITEMIDLIST> vpidl(items);
    LPCITEMIDLIST* pidlArray = vpidl.data();

    for(UINT i = 0; i < items; ++i)
    {
        LVITEM lvItem{};
        lvItem.mask = LVIF_PARAM;
        lvItem.iItem = pItems[i];
        if(GetItem(lvItem))
        {
            ListItemData*  pInfo = reinterpret_cast<ListItemData*>(lvItem.lParam);
            pidlArray[i] = pInfo->GetRelCpidl().GetPidl();
        }
    }

    if(pidlArray[0])
    {
        HRESULT        result;
        CContextMenu ccm;

        if(m_csfCurFolder.GetIShellFolder())
        {
            result = m_csfCurFolder.GetContextMenu(*this, items, pidlArray, ccm);

            if(SUCCEEDED(result))
            {
                CMenu popup;
                popup.CreatePopupMenu();
                if(popup.GetHandle())
                {
                    result = ccm.QueryContextMenu(popup, 0, 1, 0x7fff, CMF_NORMAL | CMF_EXPLORE);
                    if(SUCCEEDED(result))
                    {
                        ccm.GetContextMenu2(m_ccm2);
                        UINT  idCmd;
                        idCmd = popup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON,
                                    point.x, point.y, *this, nullptr);

                        if(idCmd)
                        {
                            CMINVOKECOMMANDINFO  cmi{};
                            cmi.cbSize = sizeof(CMINVOKECOMMANDINFO);
                            cmi.hwnd = GetParent();
                            cmi.lpVerb = (LPCSTR)(INT_PTR)(idCmd - 1);
                            cmi.nShow = SW_SHOWNORMAL;
                            ccm.Invoke(cmi);

                        //  The operation performed by the context menu may have changed
                        //  the contents of the folder, so do a refresh.
                            DoDisplay();
                        }

                        m_ccm2.Release();
                    }
                }
            }
        }
    }
}

// Enumerates the items in the specified folder, and inserts each item into
// the list-view. The LVITEM param parameter holds a pointer to the
// ListItemData.
void CMyListView::EnumObjects(CShellFolder& folder, Cpidl& cpidlParent)
{
    CEnumIDList list;

    int flags = SHCONTF_FOLDERS | SHCONTF_NONFOLDERS;
    if (GetExplorerApp()->GetMainFrame().GetShowHidden())
    {
        flags |= SHCONTF_INCLUDEHIDDEN;
    }

    if (SUCCEEDED(folder.GetEnumIDList(0, flags, list)))
    {
        ULONG fetched = 1;
        Cpidl cpidlRel;

        // Enumerate the item's PIDLs.
        while (S_OK == (list.Next(1, cpidlRel, fetched)) && fetched)
        {
            LVITEM lvItem{};

            // Fill in the TV_ITEM structure for this item.
            lvItem.mask = LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;

            // Create the ListItemDataPtr unique_ptr.
            ListItemDataPtr pItem = std::make_unique<ListItemData>(cpidlParent, cpidlRel, folder);

            // Store a pointer to the ListItemData in the lParam.
            lvItem.lParam = reinterpret_cast<LPARAM>(pItem.get());

            wchar_t fileName[MAX_PATH];
            GetFullFileName(pItem->GetFullCpidl().GetPidl(), fileName);

            ULONG attr = SFGAO_CANDELETE | SFGAO_FOLDER;
            pItem->GetParentFolder().GetAttributes(1, pItem->GetRelCpidl(), attr);
            pItem->m_isFolder = (attr & SFGAO_FOLDER) != 0;

            // Retrieve the file find handle for an existing file.
            if (lstrcmp(fileName, L"") != 0)
            {
                CFileFind file;
                if (file.FindFirstFile(fileName))
                {
                    pItem->m_fileTime = file.GetLastWriteTime();
                    pItem->m_fileSize = file.GetLength();
                }
            }

            // Retrieve the file type.
            SHFILEINFO sfi{};
            if (pItem->GetFullCpidl().GetFileInfo(0, sfi, SHGFI_PIDL | SHGFI_TYPENAME))
            {
                pItem->m_fileType = sfi.szTypeName;
            }

            // m_pItems is a vector of unique pointers. The object is
            // automatically deleted when the vector goes out of scope.
            m_pItems.push_back(std::move(pItem));

            // Text and images are done on a callback basis.
            lvItem.pszText = LPSTR_TEXTCALLBACK;
            lvItem.iImage = I_IMAGECALLBACK;

            // Determine if the item's icon characteristics
            attr = SFGAO_SHARE | SFGAO_LINK | SFGAO_GHOSTED;
            folder.GetAttributes(1, cpidlRel, attr);

            if (attr & SFGAO_SHARE)
            {
                lvItem.mask |= LVIF_STATE;
                lvItem.stateMask |= LVIS_OVERLAYMASK;
                lvItem.state |= INDEXTOOVERLAYMASK(1); // 1 is the index for the shared overlay image
            }

            if (attr & SFGAO_LINK)
            {
                lvItem.mask |= LVIF_STATE;
                lvItem.stateMask |= LVIS_OVERLAYMASK;
                lvItem.state |= INDEXTOOVERLAYMASK(2); // 2 is the index for the link overlay image
            }

            if (attr & SFGAO_GHOSTED)
            {
                lvItem.mask |= LVIF_STATE;
                lvItem.stateMask |= LVIS_CUT;
                lvItem.state |= LVIS_CUT;
            }

            InsertItem(lvItem);
            fetched = 0;
        }

        // Sort by the first column, sorting down.
        SortColumn(0, TRUE);
    }
}

// Converts a filetime to ULONGLONG.
ULONGLONG CMyListView::FileTimeToULL(FILETIME ft)
{
    return static_cast<ULONGLONG>(ft.dwHighDateTime) << 32 | ft.dwLowDateTime;
}

// Retrieves the file's size and stores the text in string.
void CMyListView::GetFileSizeText(ULONGLONG fileSize, LPWSTR string)
{
    // Convert the fileSize to a string using Locale information.
    CString preFormat;
    preFormat.Format(L"%d", ((1023 + fileSize) >> 10));
    CString postFormat;
    const int maxSize = 31;
    ::GetNumberFormat(LOCALE_USER_DEFAULT, LOCALE_NOUSEROVERRIDE, preFormat, nullptr, postFormat.GetBuffer(maxSize), maxSize);
    postFormat.ReleaseBuffer();

    // Get our decimal point character from Locale information.
    int buffLen = ::GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, nullptr, 0 );
    assert(buffLen > 0);
    CString decimal;
    ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, decimal.GetBuffer(buffLen), buffLen);
    decimal.ReleaseBuffer();

    // Truncate at the "decimal" point.
    int pos = postFormat.Find(decimal);
    if (pos > 0)
        postFormat = postFormat.Left(pos);

    postFormat += L" KB";
    StrCopy(string, postFormat, maxSize);
}

// Retrieves the file's last write time and stores the text in string.
void CMyListView::GetLastWriteTime(FILETIME modified, LPWSTR string)
{
    SYSTEMTIME localSysTime{};
    SYSTEMTIME utcTime{};
    FILETIME localFileTime{};

    // Convert the last-write time to local time.
    if (GetWinVersion() > 2501)
    {
        // For Windows Vista and later.
        ::FileTimeToSystemTime(&modified, &utcTime);
        ::SystemTimeToTzSpecificLocalTime(nullptr, &utcTime, &localSysTime);
    }
    else
    {
        // For Windows XP and earlier.
        ::FileTimeToLocalFileTime(&modified, &localFileTime);
        ::FileTimeToSystemTime(&localFileTime, &localSysTime);
    }

    // Convert the localSysTime into date and time text strings with regional settings.
    const int maxChars = 32;
    wchar_t time[maxChars];
    wchar_t date[maxChars];
    ::GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &localSysTime, nullptr, date, maxChars-1);
    ::GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &localSysTime, nullptr, time, maxChars-1);

    // Assign the date and time text strings to the string variable.
    CString dateTime;
    dateTime << date << " " << time;
    StrCopy(string, dateTime, maxChars);
}

// Called when the window handle (HWND) is attached to CMyListView.
void CMyListView::OnAttach()
{
    // Set the image lists.
    SetImageLists();

    // Set up the columns for report mode.
    LVCOLUMN lvc{};
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    int columns = 4;   // Number of columns
    int colSizes[4] = {150, 70, 100, 120}; // Width of columns in pixels

    for (int i = 0; i < columns; i++)
    {
        CString str = LoadString(IDS_COLUMN1 + i);
        lvc.iSubItem = i;
        lvc.pszText = const_cast<LPWSTR>(str.c_str());
        lvc.cx = colSizes[i];

        if (i == 1) lvc.fmt = LVCFMT_RIGHT; // right-aligned column
        else lvc.fmt = LVCFMT_LEFT;         // left-aligned column

        InsertColumn(i, lvc);
    }

    // Set initial the view style as report
    ViewReport();

    // Disable double click on the list-view's header.
    m_header.Attach(GetHeader());
    LONG_PTR style = static_cast<LONG_PTR>(m_header.GetClassLongPtr(GCL_STYLE));
    style = style & ~CS_DBLCLKS;
    m_header.SetClassLongPtr(GCL_STYLE, style);
}

// Called when the window is destroyed.
void CMyListView::OnDestroy()
{
    // Cleanup required by Win2000
    m_pItems.clear();
    m_csfCurFolder.Release();
}

// Called when a list view column is clicked.
LRESULT CMyListView::OnLVColumnClick(LPNMITEMACTIVATE pnmitem)
{
    // Determine the required sort order.
    HDITEM  hdrItem{};
    hdrItem.mask = HDI_FORMAT;
    int column = pnmitem->iSubItem;
    VERIFY(Header_GetItem(GetHeader(), column, &hdrItem));
    bool isSortDown = (hdrItem.fmt & HDF_SORTUP) ? false : true;

    // Perform the sort.
    SortColumn(column, isSortDown);

    return 0;
}

// Called in response to a LVN_GETDISPINFO notification.
// Updates the list view item with the relevant file information.
LRESULT CMyListView::OnLVNDispInfo(NMLVDISPINFO* pdi)
{
    ListItemData*   pItem = reinterpret_cast<ListItemData*>(pdi->item.lParam);

    // Add text if available.
    if (pdi->item.mask & LVIF_TEXT)
    {
        ULONG attr = SFGAO_CANDELETE | SFGAO_FOLDER;
        pItem->GetParentFolder().GetAttributes(1, pItem->GetRelCpidl(), attr);

        const int maxLength = 32;
        wchar_t text[maxLength];
        SHFILEINFO sfi{};
        bool isTimeValid = (pItem->m_fileTime.dwHighDateTime != 0 && pItem->m_fileTime.dwLowDateTime != 0);

        switch (pdi->item.iSubItem)
        {
        case 0:  // Name
        {
            // Get the display name of the item.
            if (pItem->GetFullCpidl().GetFileInfo(0, sfi, SHGFI_PIDL | SHGFI_DISPLAYNAME))
                StrCopy(pdi->item.pszText, sfi.szDisplayName, pdi->item.cchTextMax);
        }
        break;
        case 1: // Size
        {
            // Report the size files and not folders.
            if ((~attr & SFGAO_FOLDER)  && (isTimeValid))
            {
                // Retrieve the file size.
                GetFileSizeText(pItem->m_fileSize, text);
                StrCopy(pdi->item.pszText, text, maxLength);
            }
            else
                StrCopy(pdi->item.pszText, L"", 1);
        }
        break;
        case 2: // Type
        {
            if (pItem->GetFullCpidl().GetFileInfo(0, sfi, SHGFI_PIDL | SHGFI_TYPENAME))
            {
                StrCopy(pdi->item.pszText, sfi.szTypeName, pdi->item.cchTextMax);
            }
        }
        break;
        case 3: // Modified
        {
            // Retrieve the modified file time for the file.
            if (isTimeValid)
            {
                GetLastWriteTime(pItem->m_fileTime, text);
                StrCopy(pdi->item.pszText, text, maxLength);
            }
            else
                StrCopy(pdi->item.pszText, L"", 1);
        }
        break;
        }
    }

    // Add the unselected image.
    if (pdi->item.mask & LVIF_IMAGE)
    {
        SHFILEINFO sfi{};

        // Get the unselected image for this item.
        UINT flags = SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON;
        if (pItem->GetFullCpidl().GetFileInfo(0, sfi, flags))
        {
            pdi->item.iImage = sfi.iIcon;
        }
    }

    return 0;
}

// Respond to a right mouse click on the window.
LRESULT CMyListView::OnNMRClick(LPNMHDR)
{
    CPoint ptScreen;
    ::GetCursorPos(&ptScreen);
    DoContextMenu(ptScreen);

    return 0;
}

// Called when the list-view has focus and the Enter key is pressed.
LRESULT CMyListView::OnNMReturn(LPNMHDR)
{
    // Get the item that has the focus.
    int item = GetNextItem(-1, LVNI_FOCUSED);

    if (item != -1)
        DoDefault(item);

    return 0;
}

// Process notification messages (WM_NOTIFY) reflected back from the parent window.
LRESULT CMyListView::OnNotifyReflect(WPARAM, LPARAM lparam)
{
    LPNMHDR pHeader = reinterpret_cast<LPNMHDR>(lparam);
    LPNMITEMACTIVATE pnmitem = reinterpret_cast<LPNMITEMACTIVATE>(lparam);
    NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(lparam);
    switch(pHeader->code)
    {
    case LVN_COLUMNCLICK:   return OnLVColumnClick(pnmitem);
    case LVN_GETDISPINFO:   return OnLVNDispInfo(pDispInfo);
    case NM_DBLCLK:         return OnNMReturn(pHeader);
    case NM_RCLICK:         return OnNMRClick(pHeader);
    case NM_RETURN:         return OnNMReturn(pHeader);
    }
    return 0;
}

// Sets the CREATESTRUCT parameters before the window is created.
void CMyListView::PreCreate(CREATESTRUCT& cs)
{
    CListView::PreCreate(cs);
    cs.style |= WS_TABSTOP | LVS_AUTOARRANGE | LVS_ICON | LVS_SHOWSELALWAYS;
    cs.dwExStyle |= WS_EX_CLIENTEDGE;
}

// Sets the up and down sort arrows in the listview's header.
BOOL CMyListView::SetHeaderSortImage(int  columnIndex, int showArrow)
{
    HWND    hHeader = 0;
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

// Sets the image lists for the list-view control.
void CMyListView::SetImageLists()
{
    SHFILEINFO  sfi{};

    // Get the system image list
    HIMAGELIST hLargeImages = reinterpret_cast<HIMAGELIST>(::SHGetFileInfo(L"C:\\", 0, &sfi,
                                sizeof(SHFILEINFO), SHGFI_SYSICONINDEX));

    HIMAGELIST hSmallImages = reinterpret_cast<HIMAGELIST>(::SHGetFileInfo(L"C:\\", 0, &sfi,
                                sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON));

    SetImageList(hLargeImages, LVSIL_NORMAL);
    SetImageList(hSmallImages, LVSIL_SMALL);
}

// Called when the user clicks on a column in the listview's header.
void CMyListView::SortColumn(int column, bool isSortDown)
{
    // Perform the sort.
    SortViewItems sort(column, isSortDown);
    SortItems(CompareFunction, (LPARAM)&sort);

    // Ensure the selected item is visible after sorting.
    int itemint = GetNextItem(-1, LVNI_SELECTED);
    EnsureVisible(itemint, FALSE);

    // Add an arrow to the column header.
    for (int col = 0; col < Header_GetItemCount(GetHeader()); col++)
        SetHeaderSortImage(col, SHOW_NO_ARROW);

    SetHeaderSortImage(column, isSortDown ? SHOW_UP_ARROW : SHOW_DOWN_ARROW);

    // Select the previously selected or first item
    if (GetSelectionMark() > 0)
        SetItemState(GetSelectionMark(), LVIS_FOCUSED | LVIS_SELECTED, 0x000F);
    else
        SetItemState(0, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);
}

// Set the view-list mode to large icons.
void CMyListView::ViewLargeIcons()
{
    DWORD dwStyle = GetStyle();
    SetStyle((dwStyle & ~LVS_TYPEMASK) | LVS_ICON );
}

// Set the view-list mode to small icons.
void CMyListView::ViewSmallIcons()
{
    DWORD dwStyle = GetStyle();
    SetStyle((dwStyle & ~LVS_TYPEMASK) | LVS_SMALLICON);
}

// Set the view-list mode to list.
void CMyListView::ViewList()
{
    DWORD dwStyle = GetStyle();
    SetStyle((dwStyle & ~LVS_TYPEMASK) | LVS_LIST);
}

// Set the view-list mode to report.
void CMyListView::ViewReport()
{
    DWORD dwStyle = GetStyle();
    SetStyle((dwStyle & ~LVS_TYPEMASK) | LVS_REPORT);
}

// Process the list-view's window messages.
LRESULT CMyListView::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case WM_DRAWITEM:
        case WM_MENUCHAR:
        case WM_MEASUREITEM:
        case WM_INITMENUPOPUP:
        {
            // Add features implemented via IContextMenu2 such as 'New'.
            if (m_ccm2.GetIContextMenu2())
                m_ccm2.HandleMenuMsg(msg, wparam, lparam);
        }
        break;
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

///////////////////////////////////
// ListItemData function definitions
// ListItemData is a nested class of CMyListView.
//

// Constructor.
CMyListView::ListItemData::ListItemData(Cpidl& cpidlParent, Cpidl& cpidlRel, CShellFolder& parentFolder)
{
    m_parentFolder = parentFolder;
    m_cpidlFull    = cpidlParent + cpidlRel;
    m_cpidlRel     = cpidlRel;

    m_fileSize = 0;
    m_fileTime = {};
    m_isFolder = false;
}

// Destructor.
CMyListView::ListItemData::~ListItemData()
{
}

