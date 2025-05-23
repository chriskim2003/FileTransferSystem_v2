/////////////////////////////
// Mainfrm.h
//

#ifndef MAINFRM_H
#define MAINFRM_H

#include "MyCombo.h"
#include "EventSink.h"


///////////////////////////////////////////////////////////
// CMainFrame manages the application's main window.
// The main window is a frame that has a menubar, toolbar,
// statusbar and view window.
class CMainFrame : public CFrame
{
# if defined(__GNUC__)
    // Required by some TDM GCC compilers.
    enum CommandStateChangeConstants
    {
        CSC_UPDATECOMMANDS = (int)0xffffffff,
        CSC_NAVIGATEFORWARD = 0x1,
        CSC_NAVIGATEBACK = 0x2
    }     CommandStateChangeConstants;
#endif

public:
    CMainFrame();
    virtual ~CMainFrame() override = default;
    virtual HWND Create(HWND parent = nullptr) override;
    IWebBrowser2* GetIWebBrowser2() const { return m_browser.GetIWebBrowser2(); }
    const CEdit* GetCBEdit() const { return m_combo.GetCBEdit(); }

protected:
    // Virtual functions that override base class functions
    virtual void OnClose() override;
    virtual BOOL OnCommand(WPARAM wparam, LPARAM lparam) override;
    virtual int  OnCreate(CREATESTRUCT& cs) override;
    virtual void OnInitialUpdate() override;
    virtual LRESULT OnNotify(WPARAM wparam, LPARAM lparam) override;
    virtual void SetupMenuIcons() override;
    virtual void SetupToolBar() override;
    virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam) override;

private:
    CMainFrame(const CMainFrame&) = delete;
    CMainFrame& operator=(const CMainFrame&) = delete;

    void AddComboBoxBand();
    void ConnectEvents();
    void DisconnectEvents();
    IConnectionPoint* GetConnectionPoint(REFIID riid) const;

    // Command Handlers
    BOOL OnBack();
    BOOL OnFileExit();
    BOOL OnEditCut();
    BOOL OnEditCopy();
    BOOL OnEditPaste();
    BOOL OnEditDelete();
    BOOL OnForward();
    BOOL OnHelpAbout();
    BOOL OnHome();
    BOOL OnPrint();
    BOOL OnPrintPreview();
    BOOL OnRefresh();
    BOOL OnStop();

    // Message handlers
    LRESULT OnGetMinMaxInfo(UINT msg, WPARAM wparam, LPARAM lparam);

    // User defined message handlers
    void OnBeforeNavigate2(DISPPARAMS* pDispParams);
    void OnCommandStateChange(DISPPARAMS* pDispParams);
    void OnDocumentComplete(DISPPARAMS* pDispParams);
    void OnDownloadBegin(DISPPARAMS* pDispParams);
    void OnDownloadComplete(DISPPARAMS* pDispParams);
    void OnNavigateComplete2(DISPPARAMS* pDispParams);
    void OnNewWindow2(DISPPARAMS* pDispParams);
    void OnProgressChange(DISPPARAMS* pDispParams);
    void OnPropertyChange(DISPPARAMS* pDispParams);
    void OnStatusTextChange(DISPPARAMS* pDispParams);
    void OnTitleChange(DISPPARAMS* pDispParams);

    // Member variables
    CWebBrowser m_browser;          // Win32++'s web browser defined in wxx_webbrowser.h
    CMyCombo    m_combo;            // ComboBoxEx control used in the toolbar.
    CEventSink  m_eventSink;        // Routes event notifications from IWebBrowser.
    DWORD       m_eventCookie;      // Token that uniquely identifies this connection.
};


#endif //MAINFRM_H

