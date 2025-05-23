//////////////////////////////////////////////
// MDIChildRect.cpp
//  Definitions for the CViewRect and CMDIChildRect class

#include "stdafx.h"
#include "MDIChildRect.h"
#include "resource.h"


/////////////////////////////////
// CViewRect function definitions
//

// Called when the window is created
int CViewRect::OnCreate(CREATESTRUCT&)
{
    SetTimer (1, 250, nullptr) ;
    return 0;
}

// Called when the window is destroyed.
void CViewRect::OnDestroy()
{
    KillTimer(1);
}

// Called when the window is resized.
LRESULT CViewRect::OnSize(UINT, WPARAM wparam, LPARAM lparam)
{
    if (wparam != SIZE_MINIMIZED)
    {
        m_cxClientMax = LOWORD (lparam) ;
        if (m_cxClientMax < 1)
            m_cxClientMax = 1;

        m_cyClientMax = HIWORD (lparam) ;
        if (m_cyClientMax < 1)
            m_cyClientMax = 1;
    }

    return 0;
}

// Called when a timer event occurs.
LRESULT CViewRect::OnTimer(UINT, WPARAM, LPARAM)
{
    int red, green, blue;
    int xLeft, xRight, yTop, yBottom;
    xLeft   = rand () % m_cxClientMax;
    xRight  = rand () % m_cxClientMax;
    yTop    = rand () % m_cyClientMax;
    yBottom = rand () % m_cyClientMax;
    red    = rand () & 255;
    green  = rand () & 255;
    blue   = rand () & 255;

    CClientDC dcRect(*this);
    dcRect.CreateSolidBrush (RGB (red, green, blue));

    int rcLeft   = (xLeft < xRight) ? xLeft : xRight;
    int rcTop    = (yTop < yBottom) ? yTop  : yBottom;
    int rcRight  = (xLeft > xRight) ? xLeft : xRight;
    int rcBottom = (yTop > yBottom) ? yTop  : yBottom;
    dcRect.Rectangle(rcLeft, rcTop, rcRight, rcBottom);

    return 0;
}

// Process the ViewRect's window messages.
LRESULT CViewRect::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case WM_TIMER:  return OnTimer(msg, wparam, lparam);       // Display a random rectangle
        case WM_SIZE:   return OnSize(msg, wparam, lparam);        // If not minimized, save the window size
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

/////////////////////////////////////
// CMDIChildRect function definitions
//

// Constructor.
CMDIChildRect::CMDIChildRect()
{
    m_menu.LoadMenu(L"MdiMenuRect");
    SetHandles(m_menu, nullptr);
    SetView(m_rectView);
}

// Called when the window is created.
int CMDIChildRect::OnCreate(CREATESTRUCT& cs)
{
    SetWindowText(L"Rectangle Window");
    SetIconLarge(IDI_RECT);
    SetIconSmall(IDI_RECT);

    return CMDIChild::OnCreate(cs);
}

// Handle the window's messages.
LRESULT CMDIChildRect::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
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