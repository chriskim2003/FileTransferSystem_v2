/////////////////////////////
// Hyperlink.h
//

#ifndef HYPERLINK_H
#define HYPERLINK_H


#ifndef IDC_HAND
  #define IDC_HAND  MAKEINTRESOURCE(32649)
#endif


//////////////////////////////////////////////////
// CHyperlink manages the hyperlink. The hyperlink
// is a modified static control.
class CHyperlink :  public CStatic
{
public:
    CHyperlink();
    virtual ~CHyperlink() override = default;

    void SetUrlFont(CFont urlFont) { m_urlFont = urlFont; }

protected:
    // Virtual functions that override base class functions
    virtual LRESULT OnMessageReflect(UINT msg, WPARAM wparam, LPARAM lparam) override;
    virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam) override;

private:
    CHyperlink(const CHyperlink&) = delete;
    CHyperlink& operator=(const CHyperlink&) = delete;
    void OpenUrl();

    // Message handlers
    LRESULT OnLButtonDown();
    LRESULT OnLButtonUp(LPARAM lparam);
    LRESULT OnSetCursor();

    // Member variables
    bool     m_isUrlVisited;
    bool     m_isClicked;
    COLORREF m_crVisited;
    COLORREF m_crNotVisited;
    HCURSOR  m_cursor;
    CFont    m_urlFont;
};

#endif // HYPERLINK_H

