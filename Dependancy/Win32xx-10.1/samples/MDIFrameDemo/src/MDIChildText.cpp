//////////////////////////////////////////////
// MDIChildText.cpp
//  Definitions for the CMDIChildText class

#include "stdafx.h"
#include "MDIChildText.h"
#include "resource.h"

/////////////////////////////////
// CViewText function definitions
//

// Called when a window handle (HWND) is attached to CViewText.
void CViewText::OnAttach()
{
    // Set the font.
    if (!m_font)
    {
        m_font.CreatePointFont(96, L"Courier New");
    }

    SetFont(m_font);
}

// Sets the CREATESTRUCT parameters before the window is created.
void CViewText::PreCreate(CREATESTRUCT& cs)
{
    cs.style = ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | WS_CHILD |
        WS_CLIPCHILDREN | WS_HSCROLL | WS_VISIBLE | WS_VSCROLL;
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


/////////////////////////////////////
// CMDIChildText function definitions
//

// Constructor.
CMDIChildText::CMDIChildText()
{
    m_menu.LoadMenu(L"MdiMenuText");
    SetHandles(m_menu, nullptr);
    SetView(m_textView);
}

// Process menu and toolbar input forwarded from the MDI frame.
BOOL CMDIChildText::OnCommand(WPARAM wparam, LPARAM)
{
    UINT id = LOWORD(wparam);

    switch(id)
    {
    case IDM_EDIT_COPY:
        GetView().SendMessage(WM_COPY, 0, 0);
        return TRUE;
    case IDM_EDIT_PASTE:
        GetView().SendMessage(WM_PASTE, 0, 0);
        return TRUE;
    }

    return FALSE;
}

// Called when the window is created.
int CMDIChildText::OnCreate(CREATESTRUCT& cs)
{
    SetWindowText(L"Text Window");
    SetIconLarge(IDI_TEXT);
    SetIconSmall(IDI_TEXT);

    return CMDIChild::OnCreate(cs);
}

// Handle the window's messages.
LRESULT CMDIChildText::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
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
