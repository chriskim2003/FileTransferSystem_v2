/* (20-Oct-2024) [Tab/Indent: 8/8][Line/Box: 80/74]         (MyFontDialog.cpp) *
********************************************************************************
|                                                                              |
|                    Authors: Robert Tausworthe, David Nash                    |
|                                                                              |
===============================================================================*

    Contents Description: Implementation of the MyFontDialog class for
    applications using the Win32++ Windows interface classes. This class
    derives from the CFontDalog class found in the framework.

    Programming Notes: The programming style roughly follows that established
    by the 1995-1999 Jet Propulsion Laboratory Deep Space Network Planning and
    Preparation Subsystem project for C++ programming.

*******************************************************************************/

#include "stdafx.h"
#include "MyFontDlg.h"

/*============================================================================*/
    MyFontDialog::
MyFontDialog(DWORD dwFlags, HDC hdcPrinter /* = nullptr */)           /*

    Create a simple default font dialog object with the title "Font", a
    10pt Courier New typeface, and font, style, strikeout, underline, text
    color, as well as other options specified by dwFlags. The hdcPrinter
    parameter, if non-zero, is a handle to a printer device context for the
    printer on which the fonts are to be selected.
*-----------------------------------------------------------------------------*/
    : CFontDialog(dwFlags | CF_INITTOLOGFONTSTRUCT | CF_EFFECTS |
    CF_ENABLEHOOK, hdcPrinter)
{
    m_tm = {};
    SetBoxTitle(L"Font");
      // default font, 10pt Courier New
    m_font.CreatePointFont(10, L"Courier New");
    SetChoiceFont(m_font);
}

/*============================================================================*/
    BOOL MyFontDialog::
OnInitDialog()                                                          /*

    Perform any actions required for initialization of this object when
    the font choice is being initialized.
*-----------------------------------------------------------------------------*/
{
    SetWindowText(m_boxTitle);
    return TRUE;
}

/*============================================================================*/
    void MyFontDialog::
OnOK()                                                                  /*

    This member is caused to execute after the () member has
    terminated with the OK button activated. The action here is just to
    create the logfont form of the selected font.
*-----------------------------------------------------------------------------*/
{
    LOGFONT lf = GetLogFont();
    SetChoiceLogFont(lf);
}

/*============================================================================*/
    void MyFontDialog::
RecordFontMetrics()                                                     /*

    Record the current font TEXTMETRIC values and the average character
    width and height for the given parent window whose handle is
    m_cf.hwndOwner, if non-NUL, or for the entire screen if nullptr.
*-----------------------------------------------------------------------------*/{
      // get the handle to the hWnd's device context
    CWindowDC dc(GetParameters().hwndOwner);

      // select the current font into the device context:
    dc.SelectObject(m_font);

      // measure the font width and height
    dc.GetTextMetrics(m_tm);
    m_fontSize.cx = m_tm.tmAveCharWidth;
    m_fontSize.cy = m_tm.tmHeight + m_tm.tmExternalLeading;
}

/*============================================================================*/
    void MyFontDialog::
SetFontIndirect(const LOGFONT& lf)                                      /*

    Set the current font to have the characteristics contained in the
    supplied LOGFONT structure lf. Copy the face name into the font choice
    style name.
*-----------------------------------------------------------------------------*/
{
  // convert lf to a CFont
    try
    {
        m_font.CreateFontIndirect(lf);

        // if it worked, put it in this object
        m_logFont = lf;
        CHOOSEFONT cf = GetParameters();
        cf.lpLogFont = &m_logFont;
        SetParameters(cf);
    }

    catch (const CResourceException&)
    {
        ::MessageBox(nullptr, L"Font creation error.",
            L"Error", MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
    }
}
/*----------------------------------------------------------------------------*/
