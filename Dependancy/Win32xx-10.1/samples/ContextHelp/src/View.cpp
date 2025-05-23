/////////////////////////////
// View.cpp
//

#include "stdafx.h"
#include "View.h"
#include "resource.h"

//////////////////////////////////
// Definitions for the CView class
//

// Constructor.
CView::CView(UINT resID) : CDialog(resID), m_parent(nullptr)
{
}

// Creates the view window. Its a modeless dialog.
HWND CView::Create(HWND parent = nullptr)
{
    m_parent = parent;
    return DoModeless(parent);
}

// Process the dialog's window messages.
INT_PTR CView::DialogProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        // Pass messages on to the resizer.
        m_resizer.HandleMessage(msg, wparam, lparam);

      switch (msg)
      {
      case WM_SIZE:  return OnSize(msg, wparam, lparam);
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

void CView::DpiScaleImage()
{
    m_patternImage.LoadBitmap(IDB_BITMAP1);
    m_patternImage = DpiScaleUpBitmap(m_patternImage);
    LPARAM lparam = reinterpret_cast<LPARAM>(m_patternImage.GetHandle());
    m_picture.SendMessage(STM_SETIMAGE, IMAGE_BITMAP, lparam);
}

// Retrieves a reference to CDoc.
CDoc& CView::GetDoc()
{
    return m_doc;
}

// Called when the button is pressed.
BOOL CView::OnButton()
{
    SetDlgItemText(IDC_STATUS, L"Button Pressed");
    TRACE("Button Pressed\n");
    return TRUE;
}

// Called when the esc key is pressed.
void CView::OnCancel()
{
    // Discard these messages
    SetDlgItemText(IDC_STATUS, L"Cancel Pressed.");
    TRACE("Cancel Pressed.\n");
}

// Called when a request is made to close the dialog.
void CView::OnClose()
{
    // Suppress handling of WM_CLOSE for the dialog.
    // An edit control will send WM_CLOSE to the dialog if the Esc button is pressed.
}

// Process command notifications from the dialog's controls.
BOOL CView::OnCommand(WPARAM wparam, LPARAM)
{
    WORD id = LOWORD(wparam);

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

// Called before the dialog (view window) is displayed.
BOOL CView::OnInitDialog()
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
    AttachItem(IDOK,    m_ok);
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

    UINT curRadio = GetDoc().GetRadio();
    OnRangeOfIDs(ID_RADIO_A, ID_RADIO_C, curRadio);

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

// Called when the OK button is pressed.
void CView::OnOK()
{
    SetDlgItemText(IDC_STATUS, L"OK Button Pressed.");
    TRACE("OK Button Pressed.\n");
}

// Called when Check Box A is toggled.
BOOL CView::OnCheckA()
{
    TRACE("Check Box A\n");
    bool isCheckA = GetDoc().GetCheckA();
    isCheckA = !isCheckA;  // Toggle
    UINT checkFlag = isCheckA ? BST_CHECKED : BST_UNCHECKED;
    CheckDlgButton(ID_CHECK_A, checkFlag);
    GetDoc().SetCheckA(isCheckA);

    SetDlgItemText(IDC_STATUS, L"Check Box A toggled");
    return TRUE;
}

// Called when Check Box B is toggled.
BOOL CView::OnCheckB()
{
    TRACE("Check Box B\n");
    bool isCheckB = GetDoc().GetCheckB();
    isCheckB = !isCheckB;  // Toggle
    UINT checkFlag = isCheckB ? BST_CHECKED : BST_UNCHECKED;
    CheckDlgButton(ID_CHECK_B, checkFlag);
    GetDoc().SetCheckB(isCheckB);

    SetDlgItemText(IDC_STATUS, L"Check Box B toggled");
    return TRUE;
}

// Called when Check Box C is toggled.
BOOL CView::OnCheckC()
{
    TRACE("Check Box C\n");
    bool isCheckC = GetDoc().GetCheckC();
    isCheckC = !isCheckC;  // Toggle
    UINT checkFlag = isCheckC ? BST_CHECKED : BST_UNCHECKED;
    CheckDlgButton(ID_CHECK_C, checkFlag);
    GetDoc().SetCheckC(isCheckC);

    SetDlgItemText(IDC_STATUS, L"Check Box C toggled");
    return TRUE;
}

// Called when the radio button is selected.
BOOL CView::OnRangeOfIDs(UINT firstID, UINT lastID, UINT clickedID)
{
    CheckRadioButton(firstID, lastID, clickedID);

    GetDoc().SetRadio(clickedID);
    SetDlgItemText(IDC_STATUS, L"Radio changed");
    TRACE("Radio changed\n");

    return TRUE;
}

INT_PTR CView::OnSize(UINT msg, WPARAM wparam, LPARAM lparam)
{
    // Perform default processing first.
    FinalWindowProc(msg, wparam, lparam);

    // Set the image size.
    DpiScaleImage();
    return 0;
}
