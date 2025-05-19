#include "app.h"
#include "hole_punch.h"


CMainApp app;

int CView::OnCommand(WPARAM wparam, LPARAM) {
    int id = LOWORD(wparam);
    if (id == 103) {
        BOOL checked = ::SendMessage(m_radio1, BM_GETCHECK, 0, 0);
        TCHAR tmp[32];
        ::GetWindowText(m_keyword, tmp, 32);
        key = tmp;
        if (checked) {
            app.ShowOpenFileDialog();
            std::thread sender_tr(sender);
            sender_tr.detach();
        }
        else {
            std::thread receiver_tr(receiver);
            receiver_tr.detach();
        }
    }
    return 0;
}

void CLog::AppendLog(const CString& text)
{
    int length = ::GetWindowTextLength(m_log);

    if (length > 500) {
        length = 0;
        ::SetWindowText(m_log, _T(""));
    }

    SendMessage(m_log, EM_SETSEL, (WPARAM)length, (LPARAM)length);
    SendMessage(m_log, EM_REPLACESEL, 0, (LPARAM)(LPCTSTR)text);
}

void CMainApp::AppendLog(const CString& text) {
    log.AppendLog(text);
}

BOOL CMainApp::InitInstance() {
    view.CreateEx(0, NULL, _T("Main View"), WS_OVERLAPPEDWINDOW,
        100, 100, 250, 200, NULL, NULL);
    view.ShowWindow();
    view.UpdateWindow();

    log.CreateEx(0, NULL, _T("Log Window"), WS_OVERLAPPEDWINDOW,
        360, 100, 480, 300, NULL, NULL);
    log.ShowWindow();
    log.UpdateWindow();

    return TRUE;
}

char* getDownloadPath() {
    PWSTR wide_path = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Downloads, 0, NULL, &wide_path);
    if (FAILED(hr)) return nullptr;

    int len = WideCharToMultiByte(CP_UTF8, 0, wide_path, -1, NULL, 0, NULL, NULL);
    if (len <= 0) {
        CoTaskMemFree(wide_path);
        return nullptr;
    }

    char* utf8_path = (char*)malloc(len);
    WideCharToMultiByte(CP_UTF8, 0, wide_path, -1, utf8_path, len, NULL, NULL);
    CoTaskMemFree(wide_path);

    return utf8_path;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    app.Run();

    return 0;
}