/////////////////////////////
// Mainfrm.h
//

#ifndef MAINFRM_H
#define MAINFRM_H

#include "View.h"


///////////////////////////////////////////////////////////
// CMainFrame manages the application's main window.
// The main window is a frame that has a menubar, toolbar,
// statusbar and view window.
class CMainFrame : public CFrame
{
public:
    CMainFrame() = default;
    virtual ~CMainFrame() override = default;
    virtual HWND Create(HWND parent = nullptr) override;
    void DpiScaleListView();

protected:
    // Virtual functions that override base class functions.
    virtual BOOL    OnCommand(WPARAM wparam, LPARAM lparam) override;
    virtual int     OnCreate(CREATESTRUCT& cs) override;
    virtual LRESULT OnDpiChanged(UINT msg, WPARAM wparam, LPARAM lparam) override;
    virtual void    OnInitialUpdate() override;
    virtual void    SetupMenuIcons() override;
    virtual void    SetupToolBar() override;
    virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam) override;

private:
    CMainFrame(const CMainFrame&) = delete;
    CMainFrame& operator=(const CMainFrame&) = delete;

    // Command Handlers.
    void OnFileExit();

    // Member variables.
    CView m_listView;
};

#endif //MAINFRM_H

