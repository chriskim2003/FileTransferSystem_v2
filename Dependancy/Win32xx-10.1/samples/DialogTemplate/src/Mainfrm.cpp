/////////////////////////////
// Mainfrm.cpp
//

#include "stdafx.h"
#include "Mainfrm.h"
#include "ResourceFinder.h"
#include "UserMessages.h"
#include "resource.h"


//////////////////////////////////
// CMainFrame function definitions
//

// Constructor.
CMainFrame::CMainFrame() : m_preview(m_richView), m_pDockDialogsTree(nullptr),
                           m_isTemplateShown(false)
{
}

// Create the frame window.
HWND CMainFrame::Create(HWND parent)
{
    // Set m_view as the view window of the frame.
    SetView(m_richView);

    // Set the registry key name, and load the initial window position.
    // Use a registry key name like "CompanyName\\Application".
    LoadRegistrySettings(L"Win32++\\DialogTemplate");

    return CDockFrame::Create(parent);
}

// Attempts to create the dialog from the template text
// that is displayed in the rich edit view.
void CMainFrame::DialogFromTemplateText()
{
    // Retrieve the text from the rich edit window.
    CString templateText = m_richView.GetWindowText();

    // Fill arrayText with the hex numbers from templateText.
    int start = templateText.FindOneOf(L"{");
    int end = templateText.FindOneOf(L"}");
    if (start > 0 && end > start)
    {
        CString arrayText = templateText.Mid(start + 1, end - start - 1);
        arrayText.Remove(L' ');
        arrayText.Remove(L'\n');
        arrayText.Remove(L'\t');
        arrayText.Remove(L'\r');

        // Fill array vector with values from arrayText.
        CString resToken;
        int curPos = 0;
        std::vector<unsigned char> array;
        resToken = arrayText.Tokenize(L",", curPos);
        while (resToken != L"")
        {
            long value = strtol(WtoA(resToken), nullptr, 0);
            array.push_back(static_cast<byte>(value));
            resToken = arrayText.Tokenize(L",", curPos);
        };

        m_holder.ShowDialog(this, array.data());
    }
}

// Returns the file name extracted from the path name.
inline CString CMainFrame::GetFileName() const
{
    CString fileName = m_pathName;
    int pos = fileName.ReverseFind(L'\\');
    if (pos >= 0)
        fileName = fileName.Mid(pos + 1);

    return fileName;
}

// Returns a pointer to the tree view.
CDialogsTree* CMainFrame::GetTree()
{
    assert(m_pDockDialogsTree);
    return m_pDockDialogsTree->GetTree();
}

// The stream out callback function. Used to writes the text to a file.
DWORD CALLBACK CMainFrame::MyStreamOutCallback(DWORD cookie, LPBYTE pBuffer, LONG cb, LONG* pcb)
{
    // Required for StreamOut
    if (!cb)
        return (1);

    HANDLE file = reinterpret_cast<HANDLE>(static_cast<DWORD_PTR>(cookie));
    LPDWORD bytesWritten = reinterpret_cast<LPDWORD>(pcb);
    *bytesWritten = 0;
    if (!::WriteFile(file, pBuffer, cb, bytesWritten, nullptr))
        ::MessageBox(nullptr, L"WriteFile Failed", L"", MB_OK);
    return 0;
}

// OnCommand responds to menu and and toolbar input.
BOOL CMainFrame::OnCommand(WPARAM wparam, LPARAM)
{
    UINT id = LOWORD(wparam);
    switch(id)
    {
    case IDM_EDIT_COPY:         return OnEditCopy();
    case IDM_FILE_EXIT:         return OnFileExit();
    case IDM_FILE_NEW:          return OnFileNew();
    case IDM_FILE_OPEN:         return OnFileOpen();
    case IDM_FILE_PREVIEW:      return OnFilePreview();
    case IDM_FILE_PRINT:        return OnFilePrint();
    case IDM_FILE_SAVE:         return OnFileSave();
    case IDW_VIEW_STATUSBAR:    return OnViewStatusBar();
    case IDW_VIEW_TOOLBAR:      return OnViewToolBar();
    case IDM_HELP_ABOUT:        return OnHelp();
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

    // UseIndicatorStatus(FALSE);    // Don't show keyboard indicators in the StatusBar
    // UseMenuStatus(FALSE);         // Don't show menu descriptions in the StatusBar
    // UseReBar(FALSE);              // Don't use a ReBar
    // UseStatusBar(FALSE);          // Don't use a StatusBar
    // UseThemes(FALSE);             // Don't use themes
    // UseToolBar(FALSE);            // Don't use a ToolBar

    // call the base class function
    return CDockFrame::OnCreate(cs);
}

// Called in response to the UWM_DROPFILE user defined message.
LRESULT CMainFrame::OnDropFile(WPARAM wparam)
{
    Reset();

    // wParam is a pointer (LPCWSTR) to the file name.
    LPCWSTR pathName = reinterpret_cast<LPCWSTR>(wparam);
    assert(pathName);
    m_pathName = pathName;

    // Fill the tree view with a list of dialogs.
    m_finder.FindResources(pathName);
    GetTree()->FillTree(m_finder.GetAllInfo(), GetFileName());

    return 0;
}

// Copy the current selection to the clipboard.
BOOL CMainFrame::OnEditCopy()
{
    m_richView.Copy();
    return TRUE;
}

// Issue a close request to the frame to end the program.
BOOL CMainFrame::OnFileExit()
{
    Close();
    return TRUE;
}

BOOL CMainFrame::OnFileNew()
{
    Reset();
    return TRUE;
}

// Called after the window is created.
void CMainFrame::OnInitialUpdate()
{
    // Add the docker for the tree view.
    DWORD style = DS_DOCKED_LEFT | DS_CLIENTEDGE | DS_NO_CLOSE | DS_NO_UNDOCK | DS_NO_CAPTION;
    const int width = DpiScaleInt(250);
    m_pDockDialogsTree = static_cast<CDockDialogsTree*>
                         (AddDockedChild(std::make_unique<CDockDialogsTree>(), style, width));

    Reset();
}

// Create the File Open dialog to choose the file to load.
BOOL CMainFrame::OnFileOpen()
{
    m_richView.SetWindowText(nullptr);

    CString filter = "Program Files (*.exe; *.dll)|*.exe; *.dll|All Files (*.*)|*.*|";
    CFileDialog fileDlg(TRUE);    // TRUE for file open
    fileDlg.SetFilter(filter);
    fileDlg.SetDefExt(L".exe");

    // Bring up the file open dialog retrieve the selected filename.
    if (fileDlg.DoModal(*this) == IDOK)
    {
        m_pathName = fileDlg.GetPathName();

        // Fill the tree view with a list of dialogs.
        m_finder.FindResources(m_pathName);
        GetTree()->FillTree(m_finder.GetAllInfo(), GetFileName());
    }

    return TRUE;
}

// Preview the print job before sending it to a printer.
BOOL CMainFrame::OnFilePreview()
{
    // Verify a print preview is possible
    try
    {
        // Retrieve the device context of the default or currently chosen printer.
        CPrintDialog printDlg;
        CDC printerDC = printDlg.GetPrinterDC();

        // Create the preview window if required.
        if (!m_preview.IsWindow())
            m_preview.Create(*this);

        // Set the preview's owner (for messages), and number of pages.
        UINT maxPage = m_richView.CollatePages();
        m_preview.DoPrintPreview(*this, maxPage);

        // Swap views
        SetView(m_preview);
        m_preview.SetFocus();
    }

    catch (const CException& e)
    {
        // An exception occurred. Display the relevant information.
        MessageBox(e.GetText(), L"Print Preview Failed", MB_ICONWARNING);
        SetView(m_richView);
    }

    return TRUE;
}

// Select the printer and print the document.
BOOL CMainFrame::OnFilePrint()
{
    try
    {
        m_richView.DoPrint(m_pathName);
    }

    catch (const CException& e)
    {
        // An exception occurred. Display the relevant information.
        MessageBox(e.GetText(), L"Print Failed", MB_ICONWARNING);
    }

    return TRUE;
}

// Create the File Save dialog to choose the file to save.
BOOL CMainFrame::OnFileSave()
{
    CString filter = "Program Files (*.h; *.cpp)|*.h; *.cpp|All Files (*.*)|*.*|";
    CFileDialog fileDlg(FALSE);    // FALSE for file save
    fileDlg.SetFilter(filter);
    fileDlg.SetDefExt(L".h");

    // Bring up the file save dialog retrieve the selected filename
    if (fileDlg.DoModal(*this) == IDOK)
    {
        CString pathName = fileDlg.GetPathName();
        WriteFile(pathName);
    }

    return TRUE;
}

// Called when a menu item is about to be displayed.
void CMainFrame::OnMenuUpdate(UINT id)
{
    // Update the check buttons before displaying the menu.
    switch (id)
    {
    case IDM_EDIT_COPY:
    case IDM_FILE_PRINT:
    case IDM_FILE_SAVE:
    {
        // Enable these menu items if the template is shown in the rich edit view.
        GetFrameMenu().EnableMenuItem(id, m_isTemplateShown? MF_ENABLED : MF_GRAYED);
    }
    break;
    }

    CDockFrame::OnMenuUpdate(id);
}

// Called when the Print Preview's "Close" button is pressed.
BOOL CMainFrame::OnPreviewClose()
{
    // Swap the view
    SetView(m_richView);
    UpdateSettings();

    return TRUE;
}

// Called when the Print Preview's "Print Now" button is pressed
BOOL CMainFrame::OnPreviewPrint()
{
    try
    {
        m_richView.QuickPrint(m_pathName);
    }

    catch (const CException& e)
    {
        // An exception occurred. Display the relevant information.
        MessageBox(e.GetText(), L"Print Failed", MB_ICONWARNING);
    }
    return TRUE;
}

// Called when the Print Preview's "Print Setup" button is pressed.
BOOL CMainFrame::OnPreviewSetup()
{
    // Call the print setup dialog.
    CPrintDialog printDlg(PD_PRINTSETUP);
    try
    {
        // Display the print dialog
        if (printDlg.DoModal(*this) == IDOK)
        {
            CString status = L"Printer: " + printDlg.GetDeviceName();
            SetStatusText(status);
        }

        // Initiate the print preview.
        UINT maxPage = m_richView.CollatePages();
        m_preview.DoPrintPreview(*this, maxPage);
    }

    catch (const CException& e)
    {
        // An exception occurred. Display the relevant information.
        MessageBox(e.GetErrorString(), e.GetText(), MB_ICONWARNING);
    }

    return TRUE;
}

// Called when a selection is made on the tree view.
LRESULT CMainFrame::OnSelectTreeItem()
{
    m_richView.SetWindowText(nullptr);
    m_holder.Destroy();
    SetView(m_richView);
    m_isTemplateShown = false;

    HTREEITEM item = GetTree()->GetSelection();
    HMODULE module = LoadLibraryEx(m_pathName, nullptr, LOAD_LIBRARY_AS_DATAFILE);
    ResourceInfo* info = reinterpret_cast<ResourceInfo*>
                         (GetTree()->GetItemData(item));

    if (module != 0)
    {
        if (info != nullptr)
        {
            // Display the dialog template in the rich edit view.
            if (info->resourceID != 0)
                ShowTemplateText(module, MAKEINTRESOURCE(info->resourceID));
            else
                ShowTemplateText(module, info->resourceName);
        }

        ::FreeLibrary(module);
    }

    DialogFromTemplateText();
    return 0;
}

// Called by CFrameApp::OnIdle to update toolbar buttons
void CMainFrame::UpdateToolbar()
{
    GetToolBar().EnableButton(IDM_EDIT_COPY, m_isTemplateShown);
    GetToolBar().EnableButton(IDM_FILE_PRINT, m_isTemplateShown);
    GetToolBar().EnableButton(IDM_FILE_SAVE, m_isTemplateShown);
}

// Sets the application back to default.
void CMainFrame::Reset()
{
    m_holder.Destroy();
    GetTree()->DeleteAllItems();
    m_pathName.Empty();
    m_isTemplateShown = false;
    SetView(m_richView);

    CString text = "Choose a file to load dialog resources from.\n";
    text << "Use the menu or toolbar to open the file, \n";
    text << "or drag and drop it onto this window.";
    m_richView.SetWindowText(text);
}

// Configure the menu icons.
void CMainFrame::SetupMenuIcons()
{
    // Set the bitmap used for menu icons
    std::vector<UINT> data = GetToolBarData();
    if (GetMenuIconHeight() >= 24)
        AddMenuIcons(data, RGB(192, 192, 192), IDW_MAIN, 0);
    else
        AddMenuIcons(data, RGB(192, 192, 192), IDB_MENUICONS, 0);
}

// Set the resource IDs and images for the toolbar buttons.
void CMainFrame::SetupToolBar()
{
    AddToolBarButton( IDM_FILE_NEW   );
    AddToolBarButton( IDM_FILE_OPEN  );
    AddToolBarButton( IDM_FILE_SAVE  );

    AddToolBarButton( 0 );                      // Separator
    AddToolBarButton( IDM_EDIT_COPY);
    AddToolBarButton( IDM_FILE_PRINT );

    AddToolBarButton( 0 );                      // Separator
    AddToolBarButton( IDM_HELP_ABOUT );
}

// Displays the dialog template in the rich edit view in a form
// that can be copied to a C++ header file.
void CMainFrame::ShowTemplateText(HMODULE module, LPCWSTR dialogRes)
{
    HRSRC dialog = ::FindResource(module, dialogRes, RT_DIALOG);

    if (dialog != nullptr)
    {
        HGLOBAL dialogResource = ::LoadResource(module, dialog);
        if (dialogResource != nullptr)
        {
            CString dialogName;
            if (IS_INTRESOURCE(dialogRes))
                dialogName = ToCString(reinterpret_cast<size_t>(dialogRes));
            else
                dialogName = dialogRes;

            unsigned char* buf = static_cast<unsigned char*>(::LockResource(dialogResource));
            if (buf != nullptr)
            {
                CString output;
                output << "// Template array for dialog: " << dialogName << "\n";
                output << "static unsigned char dialog" << "_" << dialogName << "[] = \n{\n\t";
                m_richView.AppendText(output);

                DWORD sz = ::SizeofResource(module, dialog);
                for (DWORD i = 0; i < sz; i++)
                {
                    output.Empty();
                    output.Format(L"0x%02x", buf[i]);

                    // Append a comma except for the last byte,
                    if (i != sz - 1)
                        output << ", ";

                    // Wrap the text output at 16 bytes.
                    if ((i != 0) && ((i + 1) % 16 == 0))
                        output << "\n\t";

                    m_richView.AppendText(output);
                }
                m_richView.AppendText(L"\n};\n");
                m_isTemplateShown = true;
            }
            ::FreeResource(dialogResource);
        }
    }
}

// Process the frame's window messages.
LRESULT CMainFrame::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case UWM_DROPFILE:             return OnDropFile(wparam);
        case UWM_ONSELECTTREEITEM:     return OnSelectTreeItem();
        case UWM_PREVIEWCLOSE:         OnPreviewClose();   break;
        case UWM_PREVIEWPRINT:         OnPreviewPrint();   break;
        case UWM_PREVIEWSETUP:         OnPreviewSetup();   break;
        }

        return WndProcDefault(msg, wparam, lparam);
    }

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

// Streams from the rich edit control to the specified file.
BOOL CMainFrame::WriteFile(LPCWSTR szFileName)
{
    try
    {
        // Open the file for writing
        CFile file;
        file.Open(szFileName, CREATE_ALWAYS);

        EDITSTREAM es;
        es.dwCookie = reinterpret_cast<DWORD_PTR>(file.GetHandle());
        es.dwError = 0;
        es.pfnCallback = reinterpret_cast<EDITSTREAMCALLBACK>(MyStreamOutCallback);
        m_richView.StreamOut(SF_TEXT, es);

        // Clear the modified text flag
        m_richView.SetModify(FALSE);
    }

    catch (const CFileException&)
    {
        CString str = L"Failed to write:  ";
        str += szFileName;
        ::MessageBox(0, str, L"Warning", MB_ICONWARNING);
        return FALSE;
    }

    return TRUE;
}

