/////////////////////////////
// App.h
//

#ifndef SDI_APP_H
#define SDI_APP_H

#include "Mainfrm.h"

///////////////////////////////////////////////////////////////
// CApp manages the application. It initializes the Win32++
// framework when it is constructed, and creates the main frame
// window when it runs.
class CApp : public CWinApp
{
public:
    CApp() = default;;
    virtual ~CApp() override = default;;

protected:
    virtual BOOL InitInstance() override;

private:
    CApp(const CApp&) = delete;
    CApp& operator=(const CApp&) = delete;

    CMainFrame m_frame;
};


#endif // define SDI_APP_H
