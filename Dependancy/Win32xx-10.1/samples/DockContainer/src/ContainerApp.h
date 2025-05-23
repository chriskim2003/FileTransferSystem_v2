/////////////////////////////
// ContainerApp.h
//

#ifndef CONTAINERAPP_H
#define CONTAINERAPP_H

#include "Mainfrm.h"


/////////////////////////////////////////////////////////////////
// CDockContainerApp manages the application. It initializes the
// Win32++ framework when it is constructed, and creates the main
// frame window when it runs.
class CDockContainerApp : public CWinApp
{
public:
    CDockContainerApp() = default;
    virtual ~CDockContainerApp() override = default;

protected:
    // Virtual functions that override base class functions
    virtual BOOL InitInstance() override;

private:
    CDockContainerApp(const CDockContainerApp&) = delete;
    CDockContainerApp& operator=(const CDockContainerApp&) = delete;

    CMainFrame m_frame;
};


#endif // CONTAINERAPP_H
