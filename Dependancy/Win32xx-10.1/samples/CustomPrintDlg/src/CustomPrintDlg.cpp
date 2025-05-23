/////////////////////////////
// CustomPrintDialog.cpp
//

#include "stdafx.h"
#include "resource.h"
#include "CustomPrintDlg.h"
#include "UserMessages.h"


CCustomPrintDlg::CCustomPrintDlg()
    : CDialog(IDD_PRINTDLG), m_copies(1), m_fromPage(1), m_radio(0), m_toPage(9999),
                             m_collate(0), m_printToFile(0), m_maxPage(0),
                             m_isPropertiesDisplayed(false)
{
    m_hDevMode = nullptr;
    m_hDevNames = nullptr;
    m_owner = nullptr;
}

// Creates and assigns the hDevMode and hDevNames global memory for the specified printer.
bool CCustomPrintDlg::CreateGlobalHandles(LPCWSTR printerName, HGLOBAL* pHDevMode, HGLOBAL* pHDevNames)
{
    // HGLOBAL pHdevMode and pHDevNames are required.
    assert(pHDevMode);
    assert(pHDevNames);

    bool result = false;

    // Open printer
    HANDLE printer;
    if (::OpenPrinter(const_cast<LPWSTR>(printerName), &printer, nullptr))
    {
        // Create PRINTER_INFO_2 structure.
        DWORD bytesNeeded = 0;
        ::GetPrinter(printer, 2, nullptr, 0, &bytesNeeded);
        std::vector<BYTE> infoBuffer(bytesNeeded);
        PRINTER_INFO_2* p2 = reinterpret_cast<PRINTER_INFO_2*>(infoBuffer.data());

        // Fill the PRINTER_INFO_2 structure and close the printer handle.
        if (::GetPrinter(printer, 2, infoBuffer.data(), bytesNeeded, &bytesNeeded))
        {
            // Allocate a global handle for DEVMODE.
            size_t bufferSize = sizeof(DEVMODE) + p2->pDevMode->dmDriverExtra;
            HGLOBAL newDevMode = ::GlobalAlloc(GHND, bufferSize);
            assert(newDevMode);
            if (newDevMode != nullptr)
            {
                // copy DEVMODE data from PRINTER_INFO_2::pDevMode.
                CDevMode pNewDevMode(newDevMode);
                memcpy(pNewDevMode, p2->pDevMode, sizeof(*p2->pDevMode) +
                    p2->pDevMode->dmDriverExtra);

                // Compute size of DEVNAMES structure from PRINTER_INFO_2's data.
                size_t driverLength = lstrlen(p2->pDriverName) + 1;  // driver name
                size_t printerLength = lstrlen(p2->pPrinterName) + 1; // printer name
                size_t portLength = lstrlen(p2->pPortName) + 1;    // port name

                // Allocate a global handle big enough to hold DEVNAMES.
                bufferSize = sizeof(DEVNAMES) + (driverLength + printerLength + portLength) * sizeof(wchar_t);
                HGLOBAL newDevNames = ::GlobalAlloc(GHND, bufferSize);
                assert(newDevNames);
                if (newDevNames != nullptr)
                {
                    CDevNames pNewDevNames(newDevNames);

                    // Copy the DEVNAMES information from PRINTER_INFO_2.
                    // offset = wchar_t Offset into structure.
                    size_t offset = sizeof(DEVNAMES) / sizeof(wchar_t);

                    pNewDevNames->wDriverOffset = static_cast<WORD>(offset);
                    memcpy(pNewDevNames.GetString() + offset, p2->pDriverName,
                        driverLength * sizeof(wchar_t));
                    offset = offset + driverLength;

                    pNewDevNames->wDeviceOffset = static_cast<WORD>(offset);
                    memcpy(pNewDevNames.GetString() + offset, p2->pPrinterName,
                        printerLength * sizeof(wchar_t));
                    offset = offset + printerLength;

                    pNewDevNames->wOutputOffset = static_cast<WORD>(offset);
                    memcpy(pNewDevNames.GetString() + offset, p2->pPortName,
                        portLength * sizeof(wchar_t));
                    pNewDevNames->wDefault = 0;

                    // Set the new hDevMode and hDevNames.
                    *pHDevMode = newDevMode;
                    *pHDevNames = newDevNames;

                    result = true;
                }
            }
        }

        ::ClosePrinter(printer);
    }

    return result;
}

// Used by UpdateData to exchange data with the dialog.
void  CCustomPrintDlg::DoDataExchange(CDataExchange& DX)
{
    // Connect the dialog controls to member variables.
    DX.DDX_Control(IDC_COMBOBOX, m_comboBox);
    DX.DDX_Control(IDS_FROM, m_editFrom);
    DX.DDX_Control(IDS_TO, m_editTo);
    DX.DDX_Control(IDS_COPIES, m_editCopies);

    // Connect radio boxes value to int member variable.
    DX.DDX_Radio(IDB_RADIOALL, m_radio);

    // Connect check box values to int member variables.
    DX.DDX_Check(IDB_COLLATE, m_collate);
    DX.DDX_Check(IDB_PRINTTOFILE, m_printToFile);

    // Connect numeric edit boxes values to int member variables.
    DX.DDX_Text(IDS_FROM, m_fromPage);
    DX.DDX_Text(IDS_TO, m_toPage);
    DX.DDX_Text(IDS_COPIES, m_copies);

    // Connect text edit boxes values to CString member variables.
    DX.DDX_Text(IDS_STATUS, m_status);
    DX.DDX_Text(IDS_TYPE, m_type);
    DX.DDX_Text(IDS_WHERE, m_where);
    DX.DDX_Text(IDS_COMMENT, m_comment);
}

// Display the print dialog, and allow the user to select various options.
// An exception is thrown if the dialog isn't created.
// An exception is thrown if there is no default printer.
inline INT_PTR CCustomPrintDlg::DoModal(HWND owner)
{
    assert(!IsWindow());    // Only one window per CWnd instance allowed

    if (FindPrinters().size() == 0)
        throw CResourceException(GetApp()->MsgPrintFound());

    GetApp()->UpdateDefaultPrinter();

    // Assign values to member variables.
    m_hDevMode = GetApp()->GetHDevMode();
    m_hDevNames = GetApp()->GetHDevNames();
    m_owner = owner;

    // Display the modal dialog.
    INT_PTR result = CDialog::DoModal(owner);
    if (result == -1)
    {
        int error = static_cast<int>(CommDlgExtendedError());

        // Reset global memory
        m_hDevMode = nullptr;
        m_hDevNames = nullptr;
        GetApp()->ResetPrinterMemory();
        throw CWinException(GetApp()->MsgWndDialog(), error);
    }

    return result;
}

// Display the print setup dialog, and allow the user to select various options.
// An exception is thrown if there is no default printer.
INT_PTR CCustomPrintDlg::DoSetupModal(HWND owner)
{
    if (FindPrinters().size() == 0)
        throw CResourceException(GetApp()->MsgPrintFound());

    // Update the default printer
    GetApp()->UpdateDefaultPrinter();

    // Display the print setup dialog.
    PRINTDLG pd{};
    pd.lStructSize = sizeof(pd);
    pd.hDevMode = GetApp()->GetHDevMode();
    pd.hDevNames = GetApp()->GetHDevNames();
    pd.Flags = PD_PRINTSETUP;
    pd.hwndOwner = owner;

    INT_PTR result = ::PrintDlg(&pd);
    if (result != 0)
    {
        // User pressed OK
        GetApp()->UpdatePrinterMemory(pd.hDevMode, pd.hDevNames);
        SetPrinterFromDevMode(GetDeviceName(), GetDevMode());
        m_hDevMode = nullptr;
        m_hDevNames = nullptr;
    }
    else
    {
        m_hDevMode = nullptr;
        m_hDevNames = nullptr;
        int error = static_cast<int>(CommDlgExtendedError());
        if (error != 0)
            throw CWinException(GetApp()->MsgWndDialog(), error);
    }

    return result;
}

// Returns a vector of CString printer names.
std::vector<CString> CCustomPrintDlg::FindPrinters() const
{
    std::vector<CString> printerNames;

    DWORD bufferSize = 0;
    DWORD printerCount = 0;
    DWORD level = 2;        // for PRINTER_INFO_2
    ::SetLastError(0);

    // Identify size of PRINTER_INFO_2 buffer required.
    ::EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS,
        nullptr, level, nullptr, 0, &bufferSize, &printerCount);

    // Check for ERROR_INSUFFICIENT_BUFFER
    if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    {
        // Allocate the size of our vector.
        std::vector<BYTE> buffer(bufferSize);

        // Fill the buffer. The buffer is actually an array of PRINTER_INFO_2.
        VERIFY(::EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS,
            nullptr, level, buffer.data(), bufferSize, &bufferSize, &printerCount));

        // Do we have any printers?
        if (printerCount != 0)
        {
            // Get our array of PRINTER_INFO_2
            PRINTER_INFO_2* pInfo = reinterpret_cast<PRINTER_INFO_2*>(buffer.data());

            for (DWORD i = 0; i < printerCount; i++, pInfo++)
            {
                // Store the printer name in our vector of CString.
                printerNames.push_back(pInfo->pPrinterName);
            }
        }
    }

    return printerNames;
}

// Retrieves the number of copies requested.
int CCustomPrintDlg::GetCopies() const
{
    return m_copies;
}

CDevMode CCustomPrintDlg::GetDevMode() const
{
    if (GetApp()->GetHDevMode().Get() == nullptr)
        GetApp()->UpdateDefaultPrinter();
    if (GetApp()->GetHDevMode().Get() == nullptr)
        throw CResourceException(GetApp()->MsgPrintFound());
    return CDevMode(GetApp()->GetHDevMode());
}

CDevNames CCustomPrintDlg::GetDevNames() const
{
    if (GetApp()->GetHDevNames().Get() == nullptr)
        GetApp()->UpdateDefaultPrinter();
    if (GetApp()->GetHDevNames().Get() == nullptr)
        throw CResourceException(GetApp()->MsgPrintFound());
    return CDevNames(GetApp()->GetHDevNames());
}

CString CCustomPrintDlg::GetDriverName() const
{
    if (GetApp()->GetHDevNames().Get() == nullptr)
        GetApp()->UpdateDefaultPrinter();
    CString str;
    if (GetApp()->GetHDevNames().Get() != nullptr)
        str = GetDevNames().GetDriverName();
    return str;
}

CString CCustomPrintDlg::GetDeviceName() const
{
    if (GetApp()->GetHDevNames().Get() == nullptr)
        GetApp()->UpdateDefaultPrinter();
    CString str;
    if (GetApp()->GetHDevNames().Get() != nullptr)
        str = GetDevNames().GetDeviceName();
    return str;
}

CString CCustomPrintDlg::GetPortName() const
{
    if (GetApp()->GetHDevNames().Get() == nullptr)
        GetApp()->UpdateDefaultPrinter();
    CString str;
    if (GetApp()->GetHDevNames().Get() != nullptr)
        str = GetDevNames().GetPortName();
    return str;
}

// Returns the device context of the default or currently chosen printer.
// Throws on failure.
CDC CCustomPrintDlg::GetPrinterDC() const
{
    CDC dc;
    if (GetApp()->GetHDevNames().Get() == nullptr)
        GetApp()->UpdateDefaultPrinter();
    if ((GetApp()->GetHDevNames().Get() != nullptr) && (GetApp()->GetHDevMode().Get() != 0))
    {
        dc.CreateDC(GetDriverName(), GetDeviceName(),
            GetPortName(), GetDevMode());
    }
    else
        throw CResourceException(GetApp()->MsgPrintFound());

    return dc;
}

int CCustomPrintDlg::GetPrintToFile() const
{
    return m_printToFile;
}

// Retrieves the starting page of the print range.
int CCustomPrintDlg::GetFromPage() const
{
    return (IsPrintRange() ? m_fromPage : -1);
}

// Retrieves the ending page of the print range.
int CCustomPrintDlg::GetToPage() const
{
    return (IsPrintRange() ? m_toPage : -1);
}

// Retrieves the status from the printer's PRINTER_INFO_2.
DWORD CCustomPrintDlg::GetPrinterStatus(LPCWSTR printerName) const
{
    DWORD status = 0;
    HANDLE printer = 0;

    if (::OpenPrinter(const_cast<LPWSTR>(printerName), &printer, nullptr))
    {
        // Create PRINTER_INFO_2 structure.
        DWORD bufferSize = 0;
        ::GetPrinter(printer, 2, nullptr, 0, &bufferSize);
        assert(bufferSize);
        std::vector<BYTE> infoBuffer(bufferSize);

        // Fill the PRINTER_INFO_2 structure.
        if (::GetPrinter(printer, 2, infoBuffer.data(), bufferSize, &bufferSize))
        {
            PRINTER_INFO_2* printerInfo2 = reinterpret_cast<PRINTER_INFO_2*>(infoBuffer.data());
            status = printerInfo2->Status;
        }
        ::ClosePrinter(printer);
    }

    return status;
}

int CCustomPrintDlg::GetRadio() const
{
    return m_radio;
}

// Call this function after calling DoModal to determine whether to print
// all pages in the document.
bool CCustomPrintDlg::IsPrintAll() const
{
    return (!IsPrintRange() && !IsPrintSelection());
}

// Call this function after calling DoModal to determine whether the printer
// should collate all printed copies of the document.
bool CCustomPrintDlg::IsCollate() const
{
    return !!m_collate;
}

// Call this function after calling DoModal to determine whether to print
// only a range of pages in the document.
bool CCustomPrintDlg::IsPrintRange() const
{
    return (m_radio == IDB_RADIORANGE - IDB_RADIOALL);
}

// Call this function after calling DoModal to determine whether to print
// only the currently selected items.
bool CCustomPrintDlg::IsPrintSelection() const
{
    return (m_radio == IDB_RADIOSELECTION - IDB_RADIOALL);
}

bool CCustomPrintDlg::IsPrintToFile() const
{
    return m_printToFile != 0;
}

void CCustomPrintDlg::SetCollate(bool isCollate)
{
    m_collate = isCollate ? 1 : 0;
}

void CCustomPrintDlg::SetCopies(int copies)
{
    m_copies = copies;
}

void CCustomPrintDlg::SetFromPage(int fromPage)
{
    m_fromPage = fromPage;
}

void CCustomPrintDlg::SetPrintToFile(bool isPrintToFile)
{
    m_printToFile = isPrintToFile ? 1 : 0;
}

void CCustomPrintDlg::SetRadio(int radio)
{
    m_radio = radio;
}

void CCustomPrintDlg::SetToPage(int toPage)
{
    m_toPage = toPage;
}


BOOL CCustomPrintDlg::OnComboSelection()
{
    CString deviceName;
    int item = m_comboBox.GetCurSel();
    int length = m_comboBox.GetLBTextLen(item);
    m_comboBox.GetLBText(item, deviceName.GetBuffer(length));
    deviceName.ReleaseBuffer();
    if (deviceName != GetDeviceName())
    {
        HGLOBAL newHDevMode = nullptr;
        HGLOBAL newHDevNames = nullptr;
        CreateGlobalHandles(deviceName, &newHDevMode, &newHDevNames);
        GetApp()->ResetPrinterMemory();
        m_hDevNames = newHDevNames;
        m_hDevMode = newHDevMode;
        GetApp()->UpdatePrinterMemory(newHDevMode, newHDevNames);

        m_type = GetDriverName();
        m_where = GetPortName();
        UpdateStatusText();
    }

    ::SendMessage(m_owner, UWM_SETDEFAULTOPTIONS, 0, 0);
    UpdateData(m_dx, SENDTOCONTROL);
    return TRUE;
}

// Called when the Close button is pressed.
void CCustomPrintDlg::OnCancel()
{
    // Ignore Cancel button if the Properties dialog is displayed.
    if (!m_isPropertiesDisplayed)
        CDialog::OnCancel();
}

// Called when the dialog is closed.
void CCustomPrintDlg::OnClose()
{
    // Ignore close request if the Properties dialog is displayed.
    if (!m_isPropertiesDisplayed)
        CDialog::OnClose();
}

// Called when a button on the dialog is presseed, or
// a combox selection is made.
BOOL CCustomPrintDlg::OnCommand(WPARAM wparam, LPARAM)
{
    UINT id = LOWORD(wparam);

    switch (id)
    {
    case IDB_PRINTPROPERTIES:    return OnPrintProperties();

    case IDB_RADIOALL:           // Intentionally blank
    case IDB_RADIORANGE:         // Intentionally blank
    case IDB_RADIOSELECTION:     return OnRadioSelection(id);
    }

    UINT msg = HIWORD(wparam);
    switch (msg)
    {
    case CBN_SELCHANGE:    return OnComboSelection();
    }

    return FALSE;
}

// Called before the modal dialog is displayed.
BOOL CCustomPrintDlg::OnInitDialog()
{
    // Set the dialog's PortName and DriverName text.
    HGLOBAL hDevMode = nullptr;
    HGLOBAL hDevNames = nullptr;
    if (CreateGlobalHandles(GetDeviceName(), &hDevMode, &hDevNames))
    {
        CDevNames pDevNames(hDevNames);

        m_type = pDevNames.GetDriverName();
        m_where = pDevNames.GetPortName();
    }
    if (hDevMode != nullptr)
        ::GlobalFree(hDevMode);
    if (hDevNames != nullptr)
        ::GlobalFree(hDevNames);

    UpdateStatusText();

    // Update the dialog.
    UpdateData(m_dx, SENDTOCONTROL);

    std::vector<CString> names = FindPrinters();
    for (const CString& str : names)
    {
        m_comboBox.AddString(str);
    }

    int item = m_comboBox.FindString(0, GetDeviceName());
    m_comboBox.SetCurSel(item);

    return TRUE;
}

// Called when the OK button is pressed.
void CCustomPrintDlg::OnOK()
{
    // Ignore OK button if the Properties dialog is displayed.
    if (!m_isPropertiesDisplayed)
    {
        UpdateData(m_dx, READFROMCONTROL);
        if (m_fromPage >= 1 && m_fromPage <= m_maxPage &&
            m_toPage >= 1 && m_toPage <= m_maxPage)
        {
            GetApp()->UpdatePrinterMemory(m_hDevMode, m_hDevNames);
            m_hDevMode = nullptr;
            m_hDevNames = nullptr;

            CDialog::OnOK();
        }
        else
        {
            CString text = "Enter a page between 1 and ";
            text << ToCString(m_maxPage);
            MessageBox(text, L"Invalid Page", MB_OK);
        }
    }
}

// Called when the properties button on the print dialog is pressed.
BOOL CCustomPrintDlg::OnPrintProperties()
{
    assert(m_owner != 0);

    // Ignore Properties button if the Properties dialog is already displayed.
    if (!m_isPropertiesDisplayed)
    {
        m_isPropertiesDisplayed = true;
        // Get the printer name.
        CString dev = GetDeviceName();
        LPWSTR deviceName = const_cast<LPWSTR>(dev.c_str());

        // Retrieve the printer handle with PRINTER_ALL_ACCESS if we can.
        HANDLE printer;
        PRINTER_DEFAULTS printerDefaults{};
        printerDefaults.DesiredAccess = PRINTER_ALL_ACCESS;
        if (::OpenPrinter(deviceName, &printer, &printerDefaults) == FALSE)
            if (::OpenPrinter(deviceName, &printer, nullptr) == FALSE)
                return false;

        // Allocate the pDevMode buffer as an array of BYTE.
        size_t devModeSize = ::DocumentProperties(*this, printer, deviceName, nullptr, GetDevMode(), 0);
        std::vector<BYTE> buffer(devModeSize);
        LPDEVMODE pDevMode = reinterpret_cast<DEVMODE*>(buffer.data());

        // Display the printer property sheet, and retrieve the updated devMode data.
        if (IDOK == ::DocumentProperties(nullptr, printer, deviceName, pDevMode, 0, DM_IN_PROMPT | DM_OUT_BUFFER))
        {
            SetPrinterFromDevMode(deviceName, pDevMode);
            HGLOBAL newDevMode = nullptr;
            HGLOBAL newDevNames = nullptr;
            if (CreateGlobalHandles(deviceName, &newDevMode, &newDevNames))
            {
                // copy the updated devMode data to our global memory.
                CDevMode pNewDevMode(newDevMode);
                memcpy(pNewDevMode, pDevMode, devModeSize);

                // Save the new global memory handles.
                // CWinApp is assigned ownership of of the global memory.
                GetApp()->ResetPrinterMemory();
                m_hDevMode = newDevMode;
                m_hDevNames = newDevNames;
                GetApp()->UpdatePrinterMemory(newDevMode, newDevNames);
            }
            else
                return FALSE;
        }

        VERIFY(::ClosePrinter(printer));

        ::SendMessage(m_owner, UWM_SETDEFAULTOPTIONS, 0, 0);
        UpdateData(m_dx, SENDTOCONTROL);

        m_isPropertiesDisplayed = false;
    }
    return TRUE;
}

// Called when a radio button is selected
BOOL CCustomPrintDlg::OnRadioSelection(UINT id)
{
    CheckRadioButton(IDB_RADIOALL, IDB_RADIOSELECTION, id);
    UpdateData(m_dx, READFROMCONTROL);
    return TRUE;
}

void CCustomPrintDlg::SetMaxPage(int maxPage)
{
    m_maxPage = maxPage;
}

// Sets the DEVMODE parameters of the specified printer.
bool CCustomPrintDlg::SetPrinterFromDevMode(LPCWSTR deviceName, LPDEVMODE pDevMode)
{
    assert(deviceName);
    assert(pDevMode);

    // Retrieve the printer handle with PRINTER_ALL_ACCESS if we can.
    HANDLE printer;
    PRINTER_DEFAULTS printerDefaults{};
    printerDefaults.DesiredAccess = PRINTER_ALL_ACCESS;
    if (::OpenPrinter(const_cast<LPWSTR>(deviceName), &printer, &printerDefaults) == FALSE)
        if (::OpenPrinter(const_cast<LPWSTR>(deviceName), &printer, nullptr) == FALSE)
            throw CWinException(L"Failed to get printer handle.");;

    // Determine the size of the printerInfo buffer.
    DWORD bufferSize = 0;
    SetLastError(0);
    VERIFY(::GetPrinter(printer, 2, nullptr, 0, &bufferSize) == FALSE);
    if ((GetLastError() != ERROR_INSUFFICIENT_BUFFER) || (bufferSize == 0))
        throw CWinException(L"Failed to get printer info buffer size.");

    // Allocate a buffer for the PRINTER_INFO_2.
    std::vector<BYTE> infoBuffer(bufferSize);
    PRINTER_INFO_2* printerInfo2 = reinterpret_cast<PRINTER_INFO_2*>(infoBuffer.data());

    // Update the printer.
    if (::GetPrinter(printer, 2, infoBuffer.data(), bufferSize, &bufferSize))
    {
        printerInfo2->pDevMode = pDevMode;
        ::SetPrinter(printer, 2, infoBuffer.data(), 0);
        return true;
    }

    return false;
}

// Retrieves the printer status and stores a text result.
void CCustomPrintDlg::UpdateStatusText()
{
    // Refer to the description of PRINTER_INFO_2 in the
    // windows API documentation for possible status values.
    DWORD status = GetPrinterStatus(GetDeviceName());
    CString statusText;
    if (status == 0)                       statusText = L"Ready";
    if (status & PRINTER_STATUS_PAUSED)    statusText = L"Paused";
    if (status & PRINTER_STATUS_ERROR)     statusText = L"Error";
    if (status & PRINTER_STATUS_OFFLINE)   statusText = L"Offline";
    if (status & PRINTER_STATUS_BUSY)      statusText = L"Busy";
    if (status & PRINTER_STATUS_PRINTING)  statusText = L"Printing";
    if (status & PRINTER_STATUS_NOT_AVAILABLE) statusText = L"Not available";
    if (status & PRINTER_STATUS_TONER_LOW) statusText = L"Ink or toner low";
    if (status & PRINTER_STATUS_NO_TONER)  statusText = L"No ink or toner";
    if (status & PRINTER_STATUS_PAPER_OUT) statusText = L"Out of paper";
    if (status & PRINTER_STATUS_PAPER_JAM) statusText = L"Paper jam";
    m_status = statusText;
}
