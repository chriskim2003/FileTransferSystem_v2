/////////////////////////////
// MetaView.cpp
//

#include "MetaView.h"
#include "resource.h"


/////////////////////////////////
// CMetaView function definitions
//

// Called when the window is created.
int CMetaView::OnCreate(CREATESTRUCT&)
{
    // Set the window's icon
    SetIconSmall(IDW_MAIN);
    SetIconLarge(IDW_MAIN);

    // Set the window title
    SetWindowText(LoadString(IDW_MAIN));

    CEnhMetaFileDC metaDC;
    // Creates an enhanced MetaFile called "Pattern.emf", and also the EnhMetaFile device context
    metaDC.CreateEnhanced(nullptr, L"Pattern.emf", nullptr, nullptr);

    // Draw to the Metafile device context
    metaDC.Rectangle(0, 0, 100, 100);
    metaDC.MoveTo(0, 0);
    metaDC.LineTo(100, 100);
    metaDC.MoveTo(0, 100);
    metaDC.LineTo(100, 0);
    metaDC.CreateSolidBrush(RGB(0, 255, 0));
    metaDC.Ellipse(20, 20, 80, 80);

    // Close the metafile. The CEnhMetaFile is now ready for use.
    m_enhMetaFile = metaDC.CloseEnhanced();

    return 0;
}

// Called when part of the window needs to be redrawn.
void CMetaView::OnDraw(CDC& dc)
{
    CRect rcClient = GetClientRect();
    int cxClient = rcClient.Width();
    int cyClient = rcClient.Height();
    dc.SetMapMode(MM_ANISOTROPIC);
    dc.SetWindowExtEx(1000, 1000, nullptr);
    dc.SetViewportExtEx(cxClient, cyClient, nullptr);

    CRect rc(0, 0, 100, 100);

    // Display 10x10 images of the metafile in the window
    for (int x = 0; x < 10; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            dc.SetWindowOrgEx(-100 * x, -100 * y, nullptr);
            dc.PlayMetaFile(m_enhMetaFile, rc);
        }
    }
}

// Called when the window is destroyed.
void CMetaView::OnDestroy()
{
    PostQuitMessage(0);
}

// Process the meta view's window messages.
LRESULT CMetaView::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case WM_SIZE:
            RedrawWindow();
            return 0;
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

