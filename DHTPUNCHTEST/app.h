#pragma once
#define _WINSOCKAPI_
#define NOMINMAX
#include <algorithm>
#include <Windows.h>
#include <commdlg.h>
#include <commctrl.h>
#include <prsht.h>
#include <wxx_wincore.h>
#include <wxx_commondlg.h>
#include <string>
#include "hole_punch.h"
#include <thread>

extern std::string key;
extern std::string file_path;


class CView : public CWnd {
public:
    CView() {}
    virtual ~CView() {}
    virtual void OnDestroy() { PostQuitMessage(0); }
protected:
    HWND m_radio1 = nullptr;
    HWND m_radio2 = nullptr;
    HWND m_button = nullptr;
    HWND m_keyword = nullptr;

    virtual int OnCreate(CREATESTRUCT& cs) override {
        m_radio1 = ::CreateWindowEx(0, _T("BUTTON"), _T("Send"),
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
            20, 20, 150, 25, *this, (HMENU)101, GetApp()->GetInstanceHandle(), nullptr);
        m_radio2 = ::CreateWindowEx(0, _T("BUTTON"), _T("Receive"),
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            20, 50, 150, 25, *this, (HMENU)102, GetApp()->GetInstanceHandle(), nullptr);
        m_button = ::CreateWindowEx(0, _T("BUTTON"), _T("Confirm"),
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            20, 100, 80, 30, *this, (HMENU)103, GetApp()->GetInstanceHandle(), nullptr);

        m_keyword = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), NULL,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL,
            20, 80, 150, 20, *this, (HMENU)1001, GetApp()->GetInstanceHandle(), nullptr);

        ::SendMessage(m_radio1, BM_SETCHECK, BST_CHECKED, 0);
        return 0;
    }

    virtual int OnCommand(WPARAM wparam, LPARAM) override;
};

class CLog : public CWnd {
public:
    CLog() {}
    virtual ~CLog() {}
    virtual void OnDestroy() { PostQuitMessage(0); }
    void AppendLog(const CString& text);
protected:
    HWND m_log = nullptr;

    virtual int OnCreate(CREATESTRUCT& cs) override {
        m_log = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), NULL,
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL | WS_HSCROLL,
            20, 20, 400, 200, *this, (HMENU)1001, GetApp()->GetInstanceHandle(), nullptr);

        return 0;
    }
};

class CMainApp : public CWinApp {
public:
    CMainApp() {}
    virtual ~CMainApp() {}
    virtual BOOL InitInstance();
    void AppendLog(const CString& text);
    void ShowOpenFileDialog()
    {
        CFileDialog fileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, NULL);

        if (fileDlg.DoModal(GetMainWnd()) == IDOK)
        {
            CString filePath = fileDlg.GetPathName();
            file_path = filePath;
        }
    }
private:
    CView view;
    CLog log;
};

extern CMainApp app;

char* getDownloadPath();