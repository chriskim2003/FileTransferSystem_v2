///////////////////////////////////////
// FormView.h

#ifndef SDI_VIEW_H
#define SDI_VIEW_H

#include "FormDoc.h"
#include "StaticImage.h"
#include "resource.h"

//////////////////////////////////////////////
// CFormView manages CMainFrame's view window.
// The view window is a modeless dialog.
class CFormView : public CDialog
{
public:
    CFormView(UINT resID);
    virtual ~CFormView() override = default;
    CFormDoc& GetDoc();

    // Command handlers
    BOOL OnButton();
    BOOL OnCheckA();
    BOOL OnCheckB();
    BOOL OnCheckC();
    BOOL OnRangeOfIDs(UINT idFirst, UINT idLast, UINT idClicked);

protected:
    // Virtual functions that override base class functions
    virtual INT_PTR DialogProc(UINT msg, WPARAM wparam, LPARAM lparam) override;
    virtual void OnCancel() override;
    virtual void OnClose() override;
    virtual BOOL OnCommand(WPARAM wparam, LPARAM lparam) override;
    virtual BOOL OnInitDialog() override;
    virtual void OnOK() override;

private:
    CFormView(const CFormView&) = delete;
    CFormView& operator=(const CFormView&) = delete;

    // Member variables for dialog controls.
    CButton      m_radioA;
    CButton      m_radioB;
    CButton      m_radioC;
    CButton      m_checkA;
    CButton      m_checkB;
    CButton      m_checkC;
    CButton      m_button;
    CListBox     m_listBox;
    CEdit        m_edit;
    CRichEdit    m_richEdit;
    CStaticImage m_picture;
    CStatic      m_status;
    CButton      m_ok;
    CWnd         m_group;
    CBitmap      m_patternImage;

    // Member variables
    CFormDoc m_doc;
    CResizer m_resizer;
};

#endif //SDI_VIEW_H
