/////////////////////////////
// View.cpp
//

#include "stdafx.h"
#include "View.h"

/////////////////////////////
// CView function definitions
//

// Returns a reference to CDoc.
CDoc& CView::GetDoc()
{
    return m_doc;
}

// OnDraw is called when part or all of the window needs to be redrawn.
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
    HBRUSH white = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
    dc.FillRect(rc, white);
    dc.DrawText(L"View Window", -1, rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

// OnInitialUpdate is called immediately after the window is created.
void CView::OnInitialUpdate()
{
    TRACE("View window created\n");
}

// Sets the CREATESTRUCT parameters before the window is created.
// Preforming this is optional, but doing so allows us to
// take more precise control over the window we create.
void CView::PreCreate(CREATESTRUCT& cs)
{
    // Set the extended style.
    cs.dwExStyle = WS_EX_CLIENTEDGE;
}


// Sets the WNDCLASS parameters before the window is created.
// This is where we set the Window class parameters.
// Preforming this is optional, but doing so allows us to
// take more precise control over the type of window we create.
void CView::PreRegisterClass(WNDCLASS& wc)
{
    // Set the Window Class name.
    wc.lpszClassName = L"Win32++ View";

    // Set a background brush to white.
    wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));

    // Set the default cursor.
    wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);

    // Set the class style (not to be confused with the window styles set in PreCreate)
    wc.style = CS_DBLCLKS;  // Generate left button double click messages
}


// Prints the specified page to the specified device context.
// Here we copy (stretch) a bitmap image of the view window
// to the printed page.
void CView::PrintPage(CDC& dc, int)
{
    try
    {
        // Get the device context of the default or currently chosen printer.
        CPrintDialog printDlg;
        CDC printDC = printDlg.GetPrinterDC();

        int cxPage = printDC.GetDeviceCaps(HORZRES);        // Width  in pixels.
        int cyPage = printDC.GetDeviceCaps(VERTRES);        // Height in pixels.

        int cxView = GetClientRect().Width();
        int cyView = GetClientRect().Height();

        // Draw the view image to a memory DC.
        CClientDC viewDC(*this);
        CMemDC memDC(viewDC);
        memDC.CreateCompatibleBitmap(viewDC, cxView, cyView);
        OnDraw(memDC);

        // Now we convert the bitmap from DDB to DIB.
        CBitmap bmView = memDC.DetachBitmap();
        CBitmapInfoPtr pbmi(bmView);

        // Extract the device independent image data.
        BITMAPINFOHEADER* pBIH = reinterpret_cast<BITMAPINFOHEADER*>(pbmi.get());
        UINT scanLines = static_cast<UINT>(cyView);
        memDC.GetDIBits(bmView, 0, scanLines, nullptr, pbmi, DIB_RGB_COLORS);
        std::vector<byte> byteArray(pBIH->biSizeImage, 0);
        byte* pByteArray = byteArray.data();
        memDC.GetDIBits(bmView, 0, scanLines, pByteArray, pbmi, DIB_RGB_COLORS);

        // Copy the DI bits to the specified dc.
        dc.StretchDIBits(0, 0, cxPage, cyPage, 0, 0,
            cxView, cyView, pByteArray, pbmi, DIB_RGB_COLORS, SRCCOPY);
    }

    catch (const CException& e)
    {
        // An exception occurred. Display the relevant information.
        MessageBox(e.GetText(), L"Print Failed", MB_ICONWARNING);
    }
}

// Print to the default or previously chosen printer.
void CView::QuickPrint(LPCWSTR docName)
{
    try
    {
        // Create a DOCINFO structure.
        DOCINFO di;
        memset(&di, 0, sizeof(DOCINFO));
        di.cbSize = sizeof(DOCINFO);
        di.lpszDocName = docName;

        // Get the device context of the default or currently chosen printer.
        CPrintDialog printDlg;
        CDC printDC = printDlg.GetPrinterDC();

        // Begin a print job by calling the StartDoc function.
        printDC.StartDoc(&di);

        // Inform the driver that the application is about to begin sending data.
        printDC.StartPage();

        // Print the page on the printer DC.
        PrintPage(printDC);

        // Inform the driver that the page is finished.
        printDC.EndPage();

        // Inform the driver that document has ended.
        printDC.EndDoc();
    }

    catch (const CException& e)
    {
        // An exception occurred. Display the relevant information.
        MessageBox(e.GetText(), L"Print Failed", MB_ICONWARNING);
    }
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

