/* (20-Oct-2024) [Tab/Indent: 4/4][Line/Box: 80/74]                   (View.h) *
********************************************************************************
|                                                                              |
|               Authors: Robert C. Tausworthe, David Nash, 2020                |
|                                                                              |
===============================================================================*

    Contents Description: Declaration of the CView class derived from the
    CScrollView base class for this application using the Win32++ Windows
    framework.

    Programming Notes: The programming style roughly follows that established
    for the 1995-1999 Jet Propulsion Laboratory Deep Space Network Planning and
    Preparation Subsystem project for C++ programming.

*******************************************************************************/

#ifndef CAPP_VIEW_H
#define CAPP_VIEW_H

#include "Doc.h"

/*============================================================================*/
    class
CView   : public CScrollView                                            /*

    The application scrolling window serializable class.
*-----------------------------------------------------------------------------*/
{
public:
    CView();
    virtual ~CView() override = default;

    // Attributes
    COLORREF  GetBkgndColor() const      { return m_bkgndColor; }
    COLORREF* GetColorsArray()           { return m_colors; }
    COLORREF  GetTextColor()  const      { return m_textColor; }
    CFont     GetTextFont()   const      { return m_font; }
    void      SetBkgndColor(COLORREF bk) { m_bkgndColor = bk; }
    void      SetTextFont(CFont font)    { m_font = font; }
    void      SetTextColor(COLORREF col) { m_textColor = col; }
    CDoc&     TheDoc()                   { return m_doc; }

    // Operations
    CSize   GetFontSize() const;
    void    SetAppSize(BOOL keepPos = FALSE);
    void    SetDefaults();
    void    TextLineOut(CDC&, UINT, UINT, LPCWSTR) const;

    // public data members
    static const CString m_compiledOn;  // compilation date

protected:
    virtual void    OnDraw(CDC& dc) override;
    virtual LRESULT OnPaint(UINT msg, WPARAM wparam, LPARAM lparam) override;
    virtual void    PreCreate(CREATESTRUCT &cs) override;
    virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam) override;

private:
    CView(const CView&) = delete;
    CView& operator=(const CView&) = delete;

    // data members
    COLORREF    m_bkgndColor;   // background color
    CDoc        m_doc;          // the document
    CFont       m_font;         // the font used to display view
    COLORREF    m_colors[16];   // custom colors defined
    COLORREF    m_textColor;    // text foreground color
};
/*----------------------------------------------------------------------------*/
#endif  //CAPP_VIEW_H
