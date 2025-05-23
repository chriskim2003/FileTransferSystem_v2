/////////////////////////////
// View.cpp
//

#include "stdafx.h"
#include "View.h"
#include "UserMessages.h"
#include "resource.h"

#define HIMETRIC_INCH   2540

/////////////////////////////
// CView function definitions
//



// Constructor.
CView::CView() : m_pPicture(nullptr)
{
    // Initializes the COM library on the current thread.
    if FAILED(::CoInitialize(nullptr))
        throw CWinException(L"Failed to initialize COM");
}

// Destructor.
CView::~CView()
{
    if (m_pPicture)
        m_pPicture->Release();

    ::CoUninitialize();
}

// Retrieves the width and height of picture.
CRect CView::GetImageRect()
{
    long width = 0;
    long height = 0;

    if (m_pPicture)
    {
        m_pPicture->get_Width(&width);
        m_pPicture->get_Height(&height);
    }

    // convert himetric to pixels
    CClientDC dc(*this);
    int widthInPixels  = MulDiv(width, dc.GetDeviceCaps(LOGPIXELSX), HIMETRIC_INCH);
    int heightInPixels = MulDiv(height, dc.GetDeviceCaps(LOGPIXELSY), HIMETRIC_INCH);

    CRect imageRect;
    imageRect.right = std::max(widthInPixels, 200);
    imageRect.bottom = std::max(heightInPixels, 200);
    return imageRect;
}

// Displays a blank (black) image.
void CView::NewPictureFile()
{
    if (m_pPicture)
    {
        m_pPicture->Release();
        m_pPicture = nullptr;

        // Turn scrolling off
        SetScrollSizes();
    }

    GetAncestor().SetWindowText(LoadString(IDW_MAIN).c_str());
    Invalidate();
}

// Loads an image from the specified file.
BOOL CView::LoadPictureFile(LPCWSTR fileName)
{
    if (m_pPicture)
    {
        m_pPicture->Release();
        m_pPicture = nullptr;
    }

    BOOL IsPictureLoaded;

    // Create IPicture from image file
    if (S_OK == ::OleLoadPicturePath(WtoOLE(fileName), nullptr, 0, 0, IID_IPicture, (LPVOID*)&m_pPicture))
    {
        GetAncestor().SendMessage(UWM_FILELOADED, 0, (LPARAM)fileName);
        Invalidate();
        CSize size = CSize(GetImageRect().Width(), GetImageRect().Height());
        SetScrollSizes(size);
        IsPictureLoaded = TRUE;

        TRACE("Successfully loaded: "); TRACE(fileName); TRACE("\n");
    }
    else
    {
        CString str("Failed to load: ");
        str += fileName;
        MessageBox(str, L"Load Picture Failed", MB_ICONWARNING);
        TRACE(str); TRACE("\n");

        // Set Frame title back to default
        GetAncestor().SetWindowText(LoadString(IDW_MAIN).c_str());
        SetScrollSizes();
        IsPictureLoaded = FALSE;
    }

    return IsPictureLoaded;
}

// Called when the window is created.
int CView::OnCreate(CREATESTRUCT& cs)
{
    // Set the window background to black
    m_brush.CreateSolidBrush(RGB(0,0,0));
    SetClassLongPtr(GCLP_HBRBACKGROUND, (LONG_PTR)m_brush.GetHandle());

    // Set a black background brush for scrolling.
    SetScrollBkgnd(m_brush);

    // Support Drag and Drop on this window
    DragAcceptFiles(TRUE);

    return CWnd::OnCreate(cs);
}

// Called when part of the window needs to be redrawn.
void CView::OnDraw(CDC& dc)
{
    if (m_pPicture)
    {
        // get width and height of picture
        long width;
        long height;
        m_pPicture->get_Width(&width);
        m_pPicture->get_Height(&height);

        // convert himetric to pixels
        int widthInPixels = MulDiv(width, GetDeviceCaps(dc, LOGPIXELSX), HIMETRIC_INCH);
        int HeightInPixels = MulDiv(height, GetDeviceCaps(dc, LOGPIXELSY), HIMETRIC_INCH);

        // Render the picture to the DC
        CRect rc;
        m_pPicture->Render(dc, 0, 0, widthInPixels, HeightInPixels, 0, height, width, -height, &rc);
    }
}

// Called when a file is dropped on the window.
LRESULT CView::OnDropFiles(UINT, WPARAM wparam, LPARAM)
{
    HDROP hDrop = (HDROP)wparam;
    UINT length = DragQueryFile(hDrop, 0, nullptr, 0);

    if (length > 0)
    {
        CString FileName;
        DragQueryFile(hDrop, 0, FileName.GetBuffer(length), length +1);
        FileName.ReleaseBuffer();

        if ( !LoadPictureFile(FileName) )
            NewPictureFile();
    }

    DragFinish(hDrop);
    return 0;
}

// Sets the CREATESTRUCT parameters before the window is created.
void CView::PreCreate(CREATESTRUCT& cs)
{
    // Set the Window Class name
    cs.lpszClass = L"PictureView";

    cs.style = WS_CHILD | WS_HSCROLL | WS_VSCROLL ;

    // Set the extended style
    cs.dwExStyle = WS_EX_CLIENTEDGE;
}

// Saves the image to the specified file.
void CView::SavePicture(LPCWSTR fileName)
{
    // get a IPictureDisp interface from your IPicture pointer
    IPictureDisp *pDisp = nullptr;

    if (SUCCEEDED(m_pPicture->QueryInterface(IID_IPictureDisp,  (void**) &pDisp)))
    {
        // Save the IPicture image as a bitmap
        OleSavePictureFile(pDisp,  WtoBSTR(fileName));
        pDisp->Release();
    }
}

// Process the view's window messages.
LRESULT CView::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case WM_DROPFILES:          return OnDropFiles(msg, wparam, lparam);
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

