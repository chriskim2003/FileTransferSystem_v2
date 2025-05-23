/////////////////////////////
// MyDialog.cpp
//

#include "stdafx.h"
#include "MyDialog.h"
#include "resource.h"


/////////////////////////////////
// CMyDialog function definitions
//

// Constructor.
CMyDialog::CMyDialog(UINT resID) : CDialog(resID)
{
}

INT_PTR CMyDialog::DialogProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case WM_HSCROLL:   return OnHScroll(wparam, lparam);
        }

        // Pass unhandled messages on to parent DialogProc.
        return DialogProcDefault(msg, wparam, lparam);
    }

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

// Called when the dialog window is destroyed.
void CMyDialog::OnDestroy()
{
    // End the application
    ::PostQuitMessage(0);
}

BOOL CMyDialog::OnHScroll(WPARAM wparam, LPARAM lparam)
{
    HWND wnd = reinterpret_cast<HWND>(lparam);
    int pos = 0;
    bool isSlider = (wnd == m_slider.GetHwnd());

    if (isSlider)
        pos = m_slider.GetPos();
    else
        pos = m_scrollBar.GetPos(wparam);

    SetSliderPos(pos);
    SetScrollPos(pos);
    SetProgressPos(pos);
    SetStatic(isSlider, pos);
    return TRUE;
}

// Called before the dialog is displayed.
BOOL CMyDialog::OnInitDialog()
{
    // Set the Icon
    SetIconLarge(IDW_MAIN);
    SetIconSmall(IDW_MAIN);

    // Attach the dialog items to the CWnd objects
    AttachItem(IDC_PROGRESS1, m_progressBar);
    AttachItem(IDC_SCROLLBAR1, m_scrollBar);
    AttachItem(IDC_SLIDER1, m_slider);

    m_slider.SetTicFreq(10);

    return TRUE;
}

// Called when the OK button or Enter key is pressed.
void CMyDialog::OnOK()
{
    MessageBox(L"OK Button Pressed.  Program will exit now.", L"Button", MB_OK);
    CDialog::OnOK();
}

// Sets the progress bar's position.
void CMyDialog::SetProgressPos(int pos)
{
    m_progressBar.SetPos(pos);
}

// Sets the scroll bar's position.
void CMyDialog::SetScrollPos(int pos)
{
    m_scrollBar.SetPos(pos);
}

// Sets the slider control's position.
void CMyDialog::SetSliderPos(int pos)
{
    m_slider.SetPos(pos, TRUE);
}

// Sets the static control's text.
void CMyDialog::SetStatic(bool isSlider, int pos)
{
    CString str;
    if (isSlider)
        str.Format(L"Slider Position %d", pos);
    else
        str.Format(L"Scroll Position %d", pos);

    // Set the text in the static control
    SetDlgItemText(IDC_STATIC2, str);
}

