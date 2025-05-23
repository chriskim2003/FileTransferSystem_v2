//////////////////////////////////////////////
// SplitterMDIChild.cpp
//  Definitions for the CSimpleView and CSplitterMDIChild classes

#include "stdafx.h"
#include "SplitterMDIChild.h"
#include "Files.h"
#include "Output.h"
#include "resource.h"

using namespace std;

///////////////////////////////////
// CDockSimple function definitions
//

CDockSimple::CDockSimple()
{
    SetView(m_view);
}

// Handle the window's messages.
LRESULT CDockSimple::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
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


///////////////////////////////////
// CSimpleView function definitions
//

// Constructor.
CSimpleView::CSimpleView() : m_color(RGB(0,0,255))
{
}

// Called when part of the window needs to be redrawn.
void CSimpleView::OnDraw(CDC& dc)
{
    // Use the message font for Windows 7 and higher.
    if (GetWinVersion() >= 2601)
    {
        NONCLIENTMETRICS info = GetNonClientMetrics();
        LOGFONT lf = info.lfMessageFont;
        dc.CreateFontIndirect(lf);
    }

    // Centre some text in our view window.
    CRect rc = GetClientRect();
    dc.SetTextColor(m_color);
    dc.DrawText(L"View Window", -1, rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}

// Respond to a mouse click on the window
LRESULT CSimpleView::OnMouseActivate(UINT msg, WPARAM wparam, LPARAM lparam)
{
    // Set window focus. The docker will now report this as active.
    SetFocus();
    return FinalWindowProc(msg, wparam, lparam);
}

// Called when the window is resized.
LRESULT CSimpleView::OnSize(UINT, WPARAM, LPARAM)
{
    Invalidate();   // Force the window to be repainted
    return 0;
}

// Process the simple view's window messages.
LRESULT CSimpleView::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case WM_MOUSEACTIVATE:      return OnMouseActivate(msg, wparam, lparam);
        case WM_SIZE:               return OnSize(msg, wparam, lparam);
        }

        // Do default processing for other messages.
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

/////////////////////////////////////////
// CSplitterMDIChild function definitions
//

// Constructor.
CSplitterMDIChild::CSplitterMDIChild()
{
    // Set m_view as the view window of the MDI child.
    SetView(m_view);

    // Set the menu for this MDI child.
    SetHandles(LoadMenu(GetApp()->GetResourceHandle(), L"MdiMenuView"), nullptr);
}

// Called when the window is created.
int CSplitterMDIChild::OnCreate(CREATESTRUCT& cs)
{
    // Set the window caption.
    SetWindowText(L"Splitter Window");

    // Set the window icons.
    SetIconLarge(IDI_VIEW);
    SetIconSmall(IDI_VIEW);

    return CMDIChild::OnCreate(cs);
}

// Called after the window is created.
// Called after OnCreate.
void CSplitterMDIChild::OnInitialUpdate()
{
    // Add Child dockers.
    DWORD style = DS_CLIENTEDGE;// | DS_NO_UNDOCK;
    m_view.SetDockStyle(style);
    int width = DpiScaleInt(200);
    CDocker* pDockLeft  = m_view.AddDockedChild(make_unique<CDockOutput>(),
        DS_DOCKED_LEFT  | style, width, 0);
    CDocker* pDockRight = m_view.AddDockedChild(make_unique<CDockOutput>(),
        DS_DOCKED_RIGHT | style, width, 0);
    pDockLeft->AddDockedChild(make_unique<CDockFiles>(), DS_DOCKED_CONTAINER | style, 0, 0);
    pDockRight->AddDockedChild(make_unique<CDockFiles>(), DS_DOCKED_CONTAINER | style, 0, 0);
}

// Respond to menu and toolbar input.
BOOL CSplitterMDIChild::OnCommand(WPARAM wparam, LPARAM)
{
    // Respond to menu and toolbar input.

    UINT id = LOWORD(wparam);

    switch (id)
    {
    case IDM_COLOR_BLACK:   return OnColor(RGB(0,0,0));
    case IDM_COLOR_RED:     return OnColor(RGB(255,0,0));
    case IDM_COLOR_GREEN:   return OnColor(RGB(0,255,0));
    case IDM_COLOR_BLUE:    return OnColor(RGB(0,0,255));
    case IDM_COLOR_WHITE:   return OnColor(RGB(255,255,255));
    }

    return FALSE;
}

// Sets the text color in the simple view.
BOOL CSplitterMDIChild::OnColor(COLORREF rgb)
{
    m_view.GetSimpleView().SetColor(rgb);
    m_view.GetSimpleView().Invalidate();
    return TRUE;
}

// Called when the window is resized.
LRESULT CSplitterMDIChild::OnSize(UINT msg, WPARAM wparam, LPARAM lparam)
{
    m_view.RecalcDockLayout();

    // Pass the message on for default processing.
    return FinalWindowProc(msg, wparam, lparam);
}

// Process the MDI child's window messages.
LRESULT CSplitterMDIChild::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case WM_SIZE:   return OnSize(msg, wparam, lparam);
        }

        // Do default processing for other messages.
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
