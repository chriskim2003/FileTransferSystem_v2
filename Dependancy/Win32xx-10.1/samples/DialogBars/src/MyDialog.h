/////////////////////////////
// MyDialog.h
//

#ifndef MYDIALOG_H
#define MYDIALOG_H

#include "Scrollbar.h"


///////////////////////////////////////////////////
// CMyDialog manages the application's main dialog.
class CMyDialog : public CDialog
{
public:
    CMyDialog(UINT resID);
    virtual ~CMyDialog() override = default;

protected:
    // Virtual functions that override base class functions
    virtual INT_PTR DialogProc(UINT msg, WPARAM wparam, LPARAM lparam) override;
    virtual void OnDestroy() override;
    virtual BOOL OnInitDialog() override;
    virtual void OnOK() override;

private:
    CMyDialog(const CMyDialog&) = delete;
    CMyDialog& operator=(const CMyDialog&) = delete;

    // Message Handlers
    BOOL OnHScroll(WPARAM wparam, LPARAM lparam);

    void SetProgressPos(int pos);
    void SetScrollPos(int pos);
    void SetSliderPos(int pos);
    void SetStatic(bool isSlider, int pos);

    // Member variables
    CMyScrollBar m_scrollBar;
    CSlider      m_slider;
    CProgressBar m_progressBar;
};

#endif //MYDIALOG_H
