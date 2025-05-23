/////////////////////////////
// View.cpp
//

#include "stdafx.h"
#include "View.h"

/////////////////////////////
// CView function definitions
//

// Called when part of the view window needs to be redrawn.
void CView::OnDraw(CDC& dc)
{
    // Use the message font for Windows 7 and higher.
    if (GetWinVersion() >= 2601)
    {
        NONCLIENTMETRICS info = GetNonClientMetrics();
        LOGFONT lf = DpiScaleLogfont(info.lfMessageFont, 10);
        dc.CreateFontIndirect(lf);
    }

    // Centre some text in our view window.
    CRect rc = GetClientRect();
    dc.DrawText(L"Choose a Property Sheet from the menu",
        -1, rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

// Called after the view window is created.
void CView::OnInitialUpdate()
{
    TRACE("View Window Created\n");
}

// Sets the CREATESTRUCT parameters before the window is created.
void CView::PreCreate(CREATESTRUCT& cs)
{
    // Here we set the defaults used by the create function for the view window
    // Preforming this is optional, but doing so allows us to
    // take more precise control over the window we create.

    // Set the Window Class name
    cs.lpszClass = L"View";

    // Set the extended style
    cs.dwExStyle = WS_EX_CLIENTEDGE;
}

// Process the view's window messages.
LRESULT CView::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case WM_SIZE:
            Invalidate();
            break;  // Also do default processing.
        }

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
