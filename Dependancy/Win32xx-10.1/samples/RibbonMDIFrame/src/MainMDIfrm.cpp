/////////////////////////////
// MainMDIfrm.cpp
//

#include "stdafx.h"

#include <UIRibbonPropertyHelpers.h>
#include "RibbonUI.h"
#include "MainMDIfrm.h"
#include "SimpleMDIChild.h"
#include "Resource.h"

//////////////////////////////////////
// CMainMDIFrame function definitions.
//

// Constructor for CMainMDIFrame.
CMainMDIFrame::CMainMDIFrame() : m_pIUIRibbon(nullptr)
{
}

// Create the MDI frame window.
HWND CMainMDIFrame::Create(HWND parent)
{
    // Set the registry key name, and load the initial window position
    // Use a registry key name like "CompanyName\\Application"
    LoadRegistrySettings(L"Win32++\\Ribbon MDI Frame");

    // Load the settings from the registry with 4 MRU entries
    LoadRegistryMRUSettings(4);

    return CRibbonMDIFrame::Create(parent);
}

// This function is called when a ribbon button is pressed.
// Refer to IUICommandHandler::Execute in the Windows 7 SDK documentation.
STDMETHODIMP CMainMDIFrame::Execute(UINT32 cmdID, UI_EXECUTIONVERB verb, const PROPERTYKEY*, const PROPVARIANT* ppropvarValue, IUISimplePropertySet* pCmdExProp)
{
    if (UI_EXECUTIONVERB_EXECUTE == verb)
    {
        switch(cmdID)
        {
        // Document Group
        case IDC_CMD_NEW:           OnFileNew();        break;
        case IDC_CMD_EXIT:          OnFileExit();       break;

        // View Group
        case IDC_PEN_COLOR:         OnPenColor(ppropvarValue, pCmdExProp);  break;
        case IDC_CMD_SHOWSTATUS:    ShowStatusBar(ppropvarValue->boolVal);  break;

        // Multiple Document Interface Group
        case IDC_CMD_MDICASCADE:    MDICascade();       break;
        case IDC_CMD_MDITILE:       MDITile();          break;
        case IDC_CMD_MDIARRANGE:    MDIIconArrange();   break;
        case IDC_CMD_MDICLOSE:      OnMDIClose();       break;
        case IDC_CMD_MDIMIN:        OnMDIMinimize();    break;
        case IDC_CMD_MDIRESTORE:    OnMDIRestore();     break;

        // Other
        case IDC_CMD_ABOUT:         OnHelp();           break;
        case IDC_RIBBONHELP:        OnHelp();           break;
        case IDC_CUSTOMIZE_QAT:     TRACE(L"Customize Quick Access ToolBar\n");  break;
        default:
            {
                CString str;
                str.Format(L"Unknown Button %d\n", cmdID);
                TRACE(str);
            }
            break;
        }
    }

    return S_OK;
}

COLORREF CMainMDIFrame::GetColorFromPicker() const
{
    PROPVARIANT var;
    PropVariantInit(&var);
    COLORREF color = RGB(0, 0, 0);
    HRESULT result = GetRibbonFramework()->GetUICommandProperty(IDC_PEN_COLOR, UI_PKEY_Color, &var);
    if (SUCCEEDED(result))
        color = PropVariantToUInt32WithDefault(var, 0);

    return color;
}

// The IUIRibbon interface provides the ability to specify settings and properties for thr ribbon.
IUIRibbon* CMainMDIFrame::GetIUIRibbon() const
{
    return m_pIUIRibbon;
}

// Process the messages from the (non-ribbon) Menu and Tool Bar.
BOOL CMainMDIFrame::OnCommand(WPARAM wparam, LPARAM)
{
    UINT id = LOWORD(wparam);

    switch (id)
    {
    case IDM_FILE_NEW:          OnFileNew();            return TRUE;
    case IDM_FILE_EXIT:         OnFileExit();           return TRUE;

    case IDM_COLOR_RED:     SetPenColor(RGB(255, 0, 0));    return TRUE;
    case IDM_COLOR_BLUE:    SetPenColor(RGB(0, 0, 255));    return TRUE;
    case IDM_COLOR_GREEN:   SetPenColor(RGB(0, 196, 0));    return TRUE;
    case IDM_COLOR_BLACK:   SetPenColor(RGB(0, 0, 0));      return TRUE;

    case IDW_MDI_ARRANGE:       MDIIconArrange();           return TRUE;
    case IDW_MDI_CASCADE:       MDICascade();               return TRUE;
    case IDW_MDI_CLOSEALL:      RemoveAllMDIChildren();     return TRUE;
    case IDW_MDI_TILE:          MDITile();                  return TRUE;

    case IDW_VIEW_STATUSBAR:    return OnViewStatusBar();
    case IDW_VIEW_TOOLBAR:      return OnViewToolBar();
    case IDM_HELP_ABOUT:        return OnHelp();
    }

    return FALSE;
}

int CMainMDIFrame::OnCreate(CREATESTRUCT &cs)
{
    int result = CRibbonMDIFrame::OnCreate(cs);

    if (GetRibbonFramework())
    {
        // Set the initial status bar check box
        InitValues values = GetInitValues();
        BOOL showStatusBar = values.showStatusBar;
        PROPVARIANT var;

        InitPropVariantFromBoolean(showStatusBar, &var);
        HRESULT hr = GetRibbonFramework()->SetUICommandProperty(IDC_CMD_SHOWSTATUS, UI_PKEY_BooleanValue, var);

        // Disable some Ribbon buttons.
        InitPropVariantFromBoolean(FALSE, &var);
        if (SUCCEEDED(hr))
            hr = GetRibbonFramework()->SetUICommandProperty(IDC_CMD_OPEN, UI_PKEY_Enabled, var);
        if (SUCCEEDED(hr))
            hr = GetRibbonFramework()->SetUICommandProperty(IDC_CMD_SAVE, UI_PKEY_Enabled, var);
        if (SUCCEEDED(hr))
            hr = GetRibbonFramework()->SetUICommandProperty(IDC_CMD_SAVEAS, UI_PKEY_Enabled, var);
        if (SUCCEEDED(hr))
            hr = GetRibbonFramework()->SetUICommandProperty(IDC_CMD_PRINT, UI_PKEY_Enabled, var);

        if (SUCCEEDED(hr))
            hr = GetRibbonFramework()->SetUICommandProperty(IDC_CMD_COPY, UI_PKEY_Enabled, var);
        if (SUCCEEDED(hr))
            hr = GetRibbonFramework()->SetUICommandProperty(IDC_CMD_CUT, UI_PKEY_Enabled, var);
        if (SUCCEEDED(hr))
            hr = GetRibbonFramework()->SetUICommandProperty(IDC_CMD_PASTE, UI_PKEY_Enabled, var);

        if (SUCCEEDED(hr))
            hr = GetRibbonFramework()->SetUICommandProperty(IDC_CMD_MDICLOSE, UI_PKEY_Enabled, var);
        if (SUCCEEDED(hr))
            hr = GetRibbonFramework()->SetUICommandProperty(IDC_CMD_MDIMIN, UI_PKEY_Enabled, var);
        if (SUCCEEDED(hr))
            hr = GetRibbonFramework()->SetUICommandProperty(IDC_CMD_MDIRESTORE, UI_PKEY_Enabled, var);
    }

    return result;
}

void CMainMDIFrame::OnFileExit()
{
    // Issue a close request to the frame.
    Close();
}

void CMainMDIFrame::OnFileNew()
{
    AddMDIChild(std::make_unique<CSimpleMDIChild>());
}

void CMainMDIFrame::OnMDIClose()
{
    CMDIChild* pChild = GetActiveMDIChild();
    if (pChild)
        pChild->Close();
}

void CMainMDIFrame::OnMDIMaximize()
{
    MDIMaximize();
}

void CMainMDIFrame::OnMDIMinimize()
{
    CMDIChild* pChild = GetActiveMDIChild();
    if (pChild)
        pChild->SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

void CMainMDIFrame::OnMDIMaximized(BOOL isMax)
{
    if (GetRibbonFramework())
    {
        // Enable MDI Ribbon when the MDI child is maximized.
        PROPVARIANT var;
        InitPropVariantFromBoolean(isMax, &var);

        HRESULT hr = GetRibbonFramework()->SetUICommandProperty(IDC_CMD_MDICLOSE, UI_PKEY_Enabled, var);
        if (SUCCEEDED(hr))
            hr = GetRibbonFramework()->SetUICommandProperty(IDC_CMD_MDIMIN, UI_PKEY_Enabled, var);
        if (SUCCEEDED(hr))
            hr = GetRibbonFramework()->SetUICommandProperty(IDC_CMD_MDIRESTORE, UI_PKEY_Enabled, var);
    }
}

void CMainMDIFrame::OnMDIRestore()
{
    MDIRestore();
}

// Called when the DropdownColorPicker button is pressed.
void CMainMDIFrame::OnPenColor(const PROPVARIANT* ppropvarValue, IUISimplePropertySet* pCmdExProp)
{
    if (ppropvarValue != nullptr)
    {
        // Retrieve color type.
        UINT type = ppropvarValue->uintVal;

        // The Ribbon framework passes color as additional property if the color type is RGB.
        if (type == UI_SWATCHCOLORTYPE_RGB && pCmdExProp != nullptr)
        {
            // Retrieve color.
            PROPVARIANT var;
            PropVariantInit(&var);
            if (pCmdExProp->GetValue(UI_PKEY_Color, &var) >= 0)
            {
                if (GetActiveMDIChild())
                {
                    CSimpleView& MDIView = (CSimpleView&)GetActiveMDIChild()->GetView();
                    MDIView.SetColor(GetColorFromPicker());
                    MDIView.Invalidate();
                }
            }
        }
    }
}

// OnViewChanged is called when the ribbon is changed.
STDMETHODIMP CMainMDIFrame::OnViewChanged(UINT32, UI_VIEWTYPE typeId, IUnknown* pView, UI_VIEWVERB verb, INT32)
{
    HRESULT result = E_NOTIMPL;

    // Checks to see if the view that was changed was a Ribbon view.
    if (UI_VIEWTYPE_RIBBON == typeId)
    {
        switch (verb)
        {
        case UI_VIEWVERB_CREATE:    // The ribbon has been created.
            m_pIUIRibbon = reinterpret_cast<IUIRibbon*>(pView);
            result = S_OK;
            break;
        case UI_VIEWVERB_SIZE:      // The ribbon's size has changed.
            RecalcLayout();
            result = S_OK;
            break;
        case UI_VIEWVERB_DESTROY:   // The ribbon has been destroyed.
            m_pIUIRibbon = nullptr;
            result = S_OK;
            break;
        case UI_VIEWVERB_ERROR:
            result = E_FAIL;
            break;
        }
    }

    return result;
}

// Set the Pen Color when a color toolbar button is pressed.
// Called by OnCommand when there isn't a ribbon.
void CMainMDIFrame::SetPenColor(COLORREF clr)
{
    if (GetActiveMDIChild())
    {
        CSimpleView& MDIView = (CSimpleView&)GetActiveMDIChild()->GetView();
        MDIView.SetColor((COLORREF)clr);
        MDIView.Invalidate();
    }
}

// Configures the ToolBar. Used when there isn't a ribbon.
void CMainMDIFrame::SetupToolBar()
{
    // Define our toolbar buttons. Used when there isn't a ribbon.
    AddToolBarButton( IDM_FILE_NEW   );
    AddToolBarButton( IDM_FILE_OPEN  );
    AddToolBarButton( IDM_FILE_SAVE  );
    AddToolBarButton( 0 );              // Separator
    AddToolBarButton( IDM_EDIT_CUT,   FALSE );
    AddToolBarButton( IDM_EDIT_COPY,  FALSE );
    AddToolBarButton( IDM_EDIT_PASTE, FALSE );
    AddToolBarButton( IDM_FILE_PRINT );
    AddToolBarButton( 0 );              // Separator
    AddToolBarButton( IDM_COLOR_RED );
    AddToolBarButton( IDM_COLOR_BLUE );
    AddToolBarButton( IDM_COLOR_GREEN );
    AddToolBarButton( IDM_COLOR_BLACK );
    AddToolBarButton( 0 );              // Separator
    AddToolBarButton( IDM_HELP_ABOUT );
}

// This function is called when a ribbon button is updated.
// Refer to IUICommandHandler::UpdateProperty in the Windows 7 SDK documentation.
STDMETHODIMP CMainMDIFrame::UpdateProperty(UINT32 cmdID, __in REFPROPERTYKEY key,  __in_opt  const PROPVARIANT*, __out PROPVARIANT* newValue)
{
    HRESULT result = E_NOTIMPL;

    if(UI_PKEY_Enabled == key)
    {
        return UIInitPropertyFromBoolean(UI_PKEY_Enabled, TRUE, newValue);
    }

    switch(cmdID)
    {
    case IDC_MRULIST:
        // Set up the most recently used (MRU) menu.
        if (UI_PKEY_Label == key)
        {
            WCHAR label[MAX_PATH] = L"Recent Files";
            result = UIInitPropertyFromString(UI_PKEY_Label, label, newValue);
        }
        else if (UI_PKEY_RecentItems == key)
        {
            result = PopulateRibbonRecentItems(newValue);
        }
        break;

    case IDC_PEN_COLOR:
        // Set the initial pen color
        result = UIInitPropertyFromUInt32(key, RGB(1, 1, 1), newValue);
        break;
    }

    return result;
}

// Called to handle the window's messages.
LRESULT CMainMDIFrame::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case UWM_SIMPLECREATED:
        {
            CSimpleView* pSimpleView = reinterpret_cast<CSimpleView*>(wparam);
            assert(pSimpleView);

            if (GetRibbonFramework())
                pSimpleView->SetColor(GetColorFromPicker());

            return 0;
        }
        }

        // Use the default message handling for remaining messages.
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
