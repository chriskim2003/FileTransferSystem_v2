/////////////////////////////
// View.cpp
//

#include "stdafx.h"
#include "View.h"
#include "resource.h"

/////////////////////////////
// CView function definitions
//

// OnDraw is called when part or all of the window needs to be redrawn.
void CView::OnDraw(CDC& dc)
{
    // Centre the resource string text in our view window.
    // We use a memory device context for double buffering.

    // Load the text from the string resource in resource.rc
    CString text = LoadString(IDW_MAIN);

    // Create a memory cevice context and bitmap
    CRect rect = GetClientRect();
    CMemDC memDC(dc);
    memDC.CreateCompatibleBitmap(dc, rect.Width(), rect.Height());
    CBrush whiteBrush(RGB(255, 255, 255));
    memDC.FillRect(rect, whiteBrush);

    // Draw the text to the memory device context.
    NONCLIENTMETRICS info = GetNonClientMetrics();
    LOGFONT lf = DpiScaleLogfont(info.lfMessageFont, 10);
    memDC.CreateFontIndirect(lf);
    memDC.DrawText(text, -1, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Copy the memDC's bitmap to our device context.
    dc.BitBlt(0, 0, rect.Width(), rect.Height(), memDC, 0, 0, SRCCOPY);
}

// OnInitialUpdate is called immediately after the window is created.
void CView::OnInitialUpdate()
{
    TRACE("View window created\n");
}

// All window messages for this window pass through WndProc.
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

