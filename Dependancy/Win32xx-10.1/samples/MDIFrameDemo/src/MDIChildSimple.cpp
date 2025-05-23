//////////////////////////////////////////////
// MDIChildSimple.cpp
//  Definitions for the CViewSimple and CMDIChildSimple classes

#include "stdafx.h"
#include "MDIChildSimple.h"
#include "resource.h"

///////////////////////////////////
// CViewSimple function definitions
//

// Constructor
CViewSimple::CViewSimple() : m_color(RGB(0,0,255))
{
}

// Called when part of the window needs to be redrawn.
void CViewSimple::OnDraw(CDC& dc)
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

// Process the simple view's window messages.
LRESULT CViewSimple::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case WM_SIZE:
            // Force the window to be repainted.
            Invalidate();
            break;      // Also do default processing
        }

        // Do default processing for other messages
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

///////////////////////////////////////
// CMDIChildSimple function definitions
//

// Constructor.
CMDIChildSimple::CMDIChildSimple()
{
    m_menu.LoadMenu(L"MdiMenuView");
    SetHandles(m_menu, nullptr);
    SetView(m_view);
}

// Called when the window is created.
int CMDIChildSimple::OnCreate(CREATESTRUCT& cs)
{
    SetWindowText(L"Simple Window");
    SetIconLarge(IDI_VIEW);
    SetIconSmall(IDI_VIEW);

    return CMDIChild::OnCreate(cs);
}

// Called when the window is asked to close.
void CMDIChildSimple::OnClose()
{
    int nResult = MessageBox(L"OK to close Window?", L"File Close", MB_YESNO);

    if (nResult == IDYES)
        MDIDestroy();
}

// Respond to menu and toolbar input forwarded from the MDI frame.
BOOL CMDIChildSimple::OnCommand(WPARAM wparam, LPARAM)
{
    UINT id = LOWORD(wparam);

    switch (id)
    {
    case IDM_COLOR_BLACK:
        m_view.SetColor(RGB(0,0,0));
        Invalidate();
        return TRUE;
    case IDM_COLOR_RED:
        m_view.SetColor(RGB(255, 0, 0));
        Invalidate();
        return TRUE;
    case IDM_COLOR_GREEN:
        m_view.SetColor(RGB(0, 255, 0));
        Invalidate();
        return TRUE;
    case IDM_COLOR_BLUE:
        m_view.SetColor(RGB(0, 0, 255));
        Invalidate();
        return TRUE;
    case IDM_COLOR_WHITE:
        m_view.SetColor(RGB(255, 255, 255));
        Invalidate();
        return TRUE;
    }

    return FALSE;
}

// Process the simple MDI child's window messages.
LRESULT CMDIChildSimple::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {

    //  switch (msg)
    //  {
    //  Add case statements for each messages to be handled here.
    //  }

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
