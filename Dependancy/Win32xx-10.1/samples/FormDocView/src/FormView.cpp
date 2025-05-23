///////////////////////////////////////
// FormView.cpp

#include "stdafx.h"
#include "FormView.h"
#include "resource.h"


/////////////////////////////////
// CFormView function definitions
//

// Constructor.
CFormView::CFormView(UINT resID) : CDialog(resID)
{
}

// Process the dialog's window messages.
INT_PTR CFormView::DialogProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        m_resizer.HandleMessage(msg, wparam, lparam);

    //    switch (msg)
    //    {
    //
    //    }

        // Pass unhandled messages on to parent DialogProc.
        return DialogProcDefault(msg, wparam, lparam);
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

// Returns a reference to CDoc.
CFormDoc& CFormView::GetDoc()
{
    return m_doc;
}

// Called when the button is pressed.
BOOL CFormView::OnButton()
{
    SetDlgItemText(IDC_STATUS, L"Button Pressed");
    TRACE("Button Pressed\n");
    return TRUE;
}

// Suppress closing the dialog when the esc key is pressed.
void CFormView::OnCancel()
{
    SetDlgItemText(IDC_STATUS, L"Cancel Pressed.");
    TRACE("Cancel Pressed.\n");
}

// Suppress closing the dialog when the Esc button is pressed on a rich edit control.
void CFormView::OnClose()
{
}

// Process command messages (WM_COMMAND) from the dialog's controls.
BOOL CFormView::OnCommand(WPARAM wparam, LPARAM)
{
    UINT id = LOWORD(wparam);

    switch (id)
    {
    case IDC_BUTTON1:   return OnButton();
    case ID_CHECK_A:    return OnCheckA();
    case ID_CHECK_B:    return OnCheckB();
    case ID_CHECK_C:    return OnCheckC();

    case ID_RADIO_A:
    case ID_RADIO_B:    // intentionally blank
    case ID_RADIO_C:    return OnRangeOfIDs(ID_RADIO_A, ID_RADIO_C, id);
    }

    return FALSE;
}

// Called before the dialog is displayed.
BOOL CFormView::OnInitDialog()
{
    // Set the Icon
    SetIconLarge(IDW_MAIN);
    SetIconSmall(IDW_MAIN);

    // Attach CWnd objects to the dialog items
    AttachItem(IDC_BUTTON1, m_button);
    AttachItem(ID_CHECK_A,  m_checkA);
    AttachItem(ID_CHECK_B,  m_checkB);
    AttachItem(ID_CHECK_C,  m_checkC);
    AttachItem(IDC_EDIT1,   m_edit);
    AttachItem(IDC_LIST1,   m_listBox);
    AttachItem(IDOK,        m_ok);
    AttachItem(ID_RADIO_A,  m_radioA);
    AttachItem(ID_RADIO_B,  m_radioB);
    AttachItem(ID_RADIO_C,  m_radioC);
    AttachItem(IDC_RICHEDIT1, m_richEdit);
    AttachItem(IDC_GROUP1,  m_group);
    AttachItem(IDC_STATUS,  m_status);
    AttachItem(IDC_BITMAP1, m_picture);

    // Put some text in the edit boxes
    SetDlgItemText(IDC_EDIT1, L"Edit Control");
    SetDlgItemText(IDC_RICHEDIT1, L"Rich Edit Window");

    // Put some text in the list box
    for (int i = 0 ; i < 8 ; i++)
    {
        m_listBox.AddString(L"List Box");
    }

    // Set initial button states
    bool isChecked = GetDoc().GetCheckA();
    int checkState = isChecked ? BST_CHECKED : BST_UNCHECKED;
    m_checkA.SetCheck(checkState);

    isChecked = GetDoc().GetCheckB();
    checkState = isChecked ? BST_CHECKED : BST_UNCHECKED;
    m_checkB.SetCheck(checkState);

    isChecked = GetDoc().GetCheckC();
    checkState = isChecked ? BST_CHECKED : BST_UNCHECKED;
    m_checkC.SetCheck(checkState);

    UINT radioID = GetDoc().GetRadio();
    OnRangeOfIDs(ID_RADIO_A, ID_RADIO_C, radioID);

    // Initialize dialog resizing
    m_resizer.Initialize( *this, CRect(0, 0, 450, 350) );
    m_resizer.AddChild(m_radioA,   CResizer::topleft, 0);
    m_resizer.AddChild(m_radioB,   CResizer::topleft, 0);
    m_resizer.AddChild(m_radioC,   CResizer::topleft, 0);
    m_resizer.AddChild(m_checkA,   CResizer::topleft, 0);
    m_resizer.AddChild(m_checkB,   CResizer::topleft, 0);
    m_resizer.AddChild(m_checkC,   CResizer::topleft, 0);
    m_resizer.AddChild(m_button,   CResizer::topleft, 0);
    m_resizer.AddChild(m_edit,     CResizer::topleft, RD_STRETCH_WIDTH);
    m_resizer.AddChild(m_listBox,  CResizer::topleft, RD_STRETCH_WIDTH);
    m_resizer.AddChild(m_richEdit, CResizer::topleft, RD_STRETCH_WIDTH | RD_STRETCH_HEIGHT);
    m_resizer.AddChild(m_picture,  CResizer::topright, 0);
    m_resizer.AddChild(m_ok,       CResizer::bottomright, 0);
    m_resizer.AddChild(m_group,    CResizer::bottomright, RD_STRETCH_HEIGHT);
    m_resizer.AddChild(m_status,   CResizer::bottomright, RD_STRETCH_HEIGHT);

    return TRUE;
}

// Suppress closing the dialog when the return key is pressed.
void CFormView::OnOK()
{
    SetDlgItemText(IDC_STATUS, L"Button Pressed.");
    TRACE("Button Pressed.\n");
}

// Called when check box A is clicked.
BOOL CFormView::OnCheckA()
{
    TRACE("Check Box A\n");

    bool isChecked = !GetDoc().GetCheckA(); // Toggled
    UINT checkFlag = isChecked ? BST_CHECKED : BST_UNCHECKED;
    CheckDlgButton(ID_CHECK_A, checkFlag);
    GetDoc().SetCheckA(isChecked);

    CString str("Box A ");
    str += isChecked ? "checked" : "unchecked";
    SetDlgItemText(IDC_STATUS, str);
    return TRUE;
}

// Called when check box B is clicked.
BOOL CFormView::OnCheckB()
{
    TRACE("Check Box B\n");
    bool isChecked = !GetDoc().GetCheckB(); // Toggled
    UINT checkFlag = isChecked ? BST_CHECKED : BST_UNCHECKED;
    CheckDlgButton(ID_CHECK_B, checkFlag);
    GetDoc().SetCheckB(isChecked);

    CString str("Box B ");
    str += isChecked ? "checked" : "unchecked";
    SetDlgItemText(IDC_STATUS, str);
    return TRUE;
}

// Called when check box C is clicked.
BOOL CFormView::OnCheckC()
{
    TRACE("Check Box C\n");
    bool isChecked = !GetDoc().GetCheckC();  // Toggled
    UINT checkFlag = isChecked ? BST_CHECKED : BST_UNCHECKED;
    CheckDlgButton(ID_CHECK_C, checkFlag);
    GetDoc().SetCheckC(isChecked);

    CString str("Box C ");
    str += isChecked ? "checked" : "unchecked";
    SetDlgItemText(IDC_STATUS, str);
    return TRUE;
}

// Called when a radio button is selected.
BOOL CFormView::OnRangeOfIDs(UINT idFirst, UINT idLast, UINT idClicked)
{
    CheckRadioButton(idFirst, idLast, idClicked);
    GetDoc().SetRadio(idClicked);

    CString str;
    int button = idClicked - idFirst + 1;
    str.Format(L"Radio%d", button);
    TRACE(str); TRACE("\n");
    SetDlgItemText(IDC_STATUS, str);

    return TRUE;
}

