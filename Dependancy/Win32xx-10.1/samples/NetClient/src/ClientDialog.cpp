/////////////////////////////
// ClientDialog.cpp
//

#include "stdafx.h"
#include "ClientDialog.h"
#include "resource.h"

/////////////////////////////////////
// CClientDialog function definitions
//

// Constructor.
CClientDialog::CClientDialog(UINT resID) : CDialog(resID),
                   m_isClientConnected(false), m_socketType(SOCK_STREAM)
{
}

// Appends text to the specified edit control.
void CClientDialog::AppendText(const CEdit& edit, LPCWSTR text)
{
    // Append Line Feed
    int length = edit.GetWindowTextLength();
    if (length > 0)
        edit.AppendText(L"\r\n");

    edit.AppendText(text);
}

// Process the dialog's window messages.
INT_PTR CClientDialog::DialogProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case WM_ACTIVATE:       return OnActivate(msg, wparam, lparam);
        case WM_DPICHANGED:     return OnDpiChanged(msg, wparam, lparam);
        case USER_CONNECT:      return OnSocketConnect();
        case USER_DISCONNECT:   return OnSocketDisconnect();
        case USER_RECEIVE:      return OnSocketReceive();
        }

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

// Called when the dialog window is activated.
LRESULT CClientDialog::OnActivate(UINT, WPARAM, LPARAM)
{
    // Give focus to the Send Edit box
    if (m_editSend.IsWindow())
        GotoDlgCtrl(m_editSend);

    return TRUE;
}

// Called when the dialog window is about to be closed.
void CClientDialog::OnClose()
{
    // Disconnect before shutting down the dialog
    m_client.Disconnect();
    PostQuitMessage(0);
}

// Respond to the dialog buttons.
BOOL CClientDialog::OnCommand(WPARAM wparam, LPARAM)
{
    UINT id = LOWORD(wparam);

    switch (id)
    {
    case IDC_BUTTON_CONNECT:
        OnStartClient();
        return TRUE;
    case IDC_BUTTON_SEND:
        OnSend();
        // Give keyboard focus to the Send edit box
        GotoDlgCtrl(m_editSend);
        return TRUE;
    }

    return FALSE;
}

// Called when the effective dots per inch (dpi) for a window has changed.
// This occurs when:
//  - The window is moved to a new monitor that has a different DPI.
//  - The DPI of the monitor hosting the window changes.
LRESULT CClientDialog::OnDpiChanged(UINT, WPARAM, LPARAM)
{
    CFont font = GetFont();
    m_ip4Address.SetFont(font);

    return 0;
}

// Called before the dialog is displayed.
BOOL CClientDialog::OnInitDialog()
{
    // Set the Icon
    SetIconLarge(IDW_MAIN);
    SetIconSmall(IDW_MAIN);

    // reposition dialog
    CRect rc = GetWindowRect();
    MoveWindow(rc.left + 14, rc.top + 14, rc.Width(), rc.Height(), TRUE);

    // Attach CWnd objects to the dialog's children
    m_ip4Address.AttachDlgItem(IDC_IPADDRESS, *this);
    m_editIP6Address.AttachDlgItem(IDC_EDIT_IPV6ADDRESS, *this);
    m_editStatus.AttachDlgItem(IDC_EDIT_STATUS, *this);
    m_editPort.AttachDlgItem(IDC_EDIT_PORT, *this);
    m_editSend.AttachDlgItem(IDC_EDIT_SEND, *this);
    m_editReceive.AttachDlgItem(IDC_EDIT_RECEIVE, *this);
    m_buttonConnect.AttachDlgItem(IDC_BUTTON_CONNECT, *this);
    m_buttonSend.AttachDlgItem(IDC_BUTTON_SEND, *this);
    m_radioIP4.AttachDlgItem(IDC_RADIO_IPV4, *this);
    m_radioIP6.AttachDlgItem(IDC_RADIO_IPV6, *this);
    m_radioTCP.AttachDlgItem(IDC_RADIO_TCP, *this);
    m_radioUDP.AttachDlgItem(IDC_RADIO_UDP, *this);

    // Set the initial state of the dialog
    m_editIP6Address.SetWindowText(L"0000:0000:0000:0000:0000:0000:0000:0001");
    m_radioIP4.SetCheck(BST_CHECKED);
    AppendText(m_editStatus, L"Not Connected");
    m_editPort.SetWindowText(L"3000");
    m_radioTCP.SetCheck(BST_CHECKED);
    m_ip4Address.SetAddress(MAKEIPADDRESS(127, 0, 0, 1));

    if (!m_client.IsIPV6Supported())
    {
        m_radioIP6.EnableWindow(FALSE);
        m_editIP6Address.EnableWindow(FALSE);
    }

    return TRUE;
}

// Called when the connection to the server is established.
BOOL CClientDialog::OnSocketConnect()
{
    m_isClientConnected = true;
    m_buttonConnect.EnableWindow( TRUE );

    // Move focus to the Send Edit box
    GotoDlgCtrl(m_editSend);
    SetForegroundWindow();

    // Update the dialog
    m_ip4Address.EnableWindow( FALSE );
    m_editIP6Address.EnableWindow( FALSE );
    m_buttonSend.EnableWindow( TRUE );
    m_editSend.EnableWindow( TRUE );
    m_editPort.EnableWindow( FALSE );
    m_radioIP4.EnableWindow( FALSE );
    m_radioIP6.EnableWindow( FALSE );
    m_radioTCP.EnableWindow( FALSE );
    m_radioUDP.EnableWindow( FALSE );
    AppendText(m_editStatus, L"Connected to server");
    m_buttonConnect.SetWindowText(L"Disconnect");

    return TRUE;
}

// Called when the socket is disconnected from the server.
BOOL CClientDialog::OnSocketDisconnect()
{
    m_isClientConnected = false;
    m_buttonConnect.EnableWindow( TRUE );

    // Update the dialog
    AppendText(m_editStatus, L"Disconnected from server");
    m_buttonSend.EnableWindow( FALSE );
    m_editSend.EnableWindow( FALSE );
    m_editPort.EnableWindow( TRUE );
    m_ip4Address.EnableWindow( TRUE );
    m_radioTCP.EnableWindow( TRUE );
    m_radioUDP.EnableWindow( TRUE );
    m_radioIP4.EnableWindow( TRUE );
    m_buttonConnect.SetWindowText(L"Connect");
    if ( m_client.IsIPV6Supported() )
    {
        m_editIP6Address.EnableWindow( TRUE );
        m_radioIP6.EnableWindow( TRUE );
    }

    return TRUE;
}

// Called when the socket has data to receive.
BOOL CClientDialog::OnSocketReceive()
{
    std::vector<CHAR> bufVector( 1025, '\0' );
    CHAR* bufArray = bufVector.data(); // CHAR array with 1025 elements initialized to '\0'
    if (m_client.Receive(bufArray, 1024, 0 ) == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSAEWOULDBLOCK)
            AppendText(m_editStatus, L"Network error");

        return FALSE;
    }

    AppendText( m_editReceive, AtoW(bufArray) );
    TRACE("[Received:] "); TRACE(bufArray); TRACE("\n");

    return TRUE;
}

void CClientDialog::OnStartClient()
{
    TRACE("Connect/Disconnect Button Pressed\n");

    LRESULT check = m_radioTCP.GetCheck();
    m_socketType = (check == BST_CHECKED)? SOCK_STREAM : SOCK_DGRAM ;

    check = m_radioIP4.GetCheck();
    int IPfamily = (check == BST_CHECKED)? PF_INET : PF_INET6 ;

    if (!m_isClientConnected)
    {
        switch(m_socketType)
        {
        case SOCK_STREAM:
            {
                // Create the socket

                if (!m_client.Create(IPfamily, SOCK_STREAM))
                {
                    AppendText(m_editStatus, m_client.GetErrorString());
                    MessageBox(L"Failed to create Client socket", L"Connect Failed", MB_ICONWARNING);
                    return;
                }

                // Retrieve the IP Address
                CString strAddr;
                if (PF_INET6 == IPfamily)
                {
                    strAddr = m_editIP6Address.GetWindowText();
                }
                else
                {
                    strAddr = m_ip4Address.GetAddress();
                }

                // Retrieve the local port number
                UINT port = GetDlgItemInt(m_editPort.GetDlgCtrlID(), FALSE);

                // Temporarily disable the Connect/Disconnect button
                m_buttonConnect.EnableWindow(FALSE);

                // Connect to the server
                if (0 != m_client.Connect(strAddr, port) )
                {
                    AppendText(m_editStatus, m_client.GetErrorString());
                    MessageBox(L"Failed to connect to server. Is it started?",
                        L"Connect Failed", MB_ICONWARNING);
                    m_client.Disconnect();
                    m_buttonConnect.EnableWindow( TRUE );
                    return;
                }
                m_client.StartEvents();

            }
            break;

        case SOCK_DGRAM:
            {
                // Create the socket
                if (!m_client.Create(IPfamily, SOCK_DGRAM))
                {
                    AppendText(m_editStatus, m_client.GetErrorString());
                    MessageBox(L"Failed to create Client socket", L"Connect Failed", MB_ICONWARNING);
                    return;
                }

                m_client.StartEvents();

                //Update the dialog
                m_ip4Address.EnableWindow( FALSE );
                m_editIP6Address.EnableWindow( FALSE );
                m_buttonSend.EnableWindow( TRUE );
                m_editSend.EnableWindow( TRUE );
                m_editPort.EnableWindow( FALSE );
                m_radioIP4.EnableWindow( FALSE );
                m_radioIP6.EnableWindow( FALSE );
                m_radioTCP.EnableWindow( FALSE );
                m_radioUDP.EnableWindow( FALSE );
                m_buttonConnect.SetWindowText(L"Disconnect");
                AppendText(m_editStatus, L"Connected, ready to send");
                GotoDlgCtrl(m_editSend);
                m_isClientConnected = true;
            }
            break;
        }
    }
    else
    {
        // Shutdown and close the client socket
        m_client.Disconnect();

        // Update the dialog
        m_ip4Address.EnableWindow( TRUE );
        m_buttonSend.EnableWindow( FALSE );
        m_editSend.EnableWindow( FALSE );
        m_editPort.EnableWindow( TRUE );
        m_radioIP4.EnableWindow( TRUE );
        m_radioTCP.EnableWindow( TRUE );
        m_radioUDP.EnableWindow( TRUE );
        m_buttonConnect.SetWindowText(L"Connect");
        AppendText(m_editStatus, L"Not Connected");

        if (m_client.IsIPV6Supported())
        {
            m_radioIP6.EnableWindow(TRUE);
            m_editIP6Address.EnableWindow(TRUE);
        }
        m_isClientConnected = false;
    }
}

void CClientDialog::OnSend()
{
    switch (m_socketType)
    {
    case SOCK_STREAM:   // for TCP client
        {
            CStringA send(WtoA(GetDlgItemText(IDC_EDIT_SEND)));
            if (SOCKET_ERROR == m_client.Send(send, send.GetLength(), 0))
                if (WSAGetLastError() != WSAEWOULDBLOCK)
                    AppendText(m_editStatus, L"Send Failed");
        }
        break;
    case SOCK_DGRAM:    // for UDP client
        {
            LRESULT check = m_radioIP4.GetCheck();
            int IPfamily = (check == BST_CHECKED)? PF_INET : PF_INET6 ;

            UINT port = GetDlgItemInt(m_editPort.GetDlgCtrlID(), FALSE);
            CString strSend = m_editSend.GetWindowText();

            // Retrieve the IP Address
            CString strAddr;
            if (PF_INET6 == IPfamily)
            {
                strAddr = m_editIP6Address.GetWindowText();
            }
            else
            {
                strAddr = m_ip4Address.GetAddress();
            }

            if (SOCKET_ERROR == m_client.SendTo(WtoA(strSend), strSend.GetLength(), 0, strAddr, port ))
                if (WSAGetLastError() != WSAEWOULDBLOCK)
                    AppendText(m_editStatus, L"SendTo Failed");
        }
        break;
    }
}

