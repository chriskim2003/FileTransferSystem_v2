/////////////////////////////
// MovieShowApp.h
//

#ifndef _MOVIESHOWAPP_H_
#define _MOVIESHOWAPP_H_

#include "Mainfrm.h"


////////////////////////////////////////////////////////////////////
// CMovieShowApp manages the application. It initializes the Win32++
// framework when it is constructed, and creates the main frame
// window when it runs.
class CMovieShowApp : public CWinApp
{
public:
    CMovieShowApp() = default;
    virtual ~CMovieShowApp() override = default;
    CMovieShowApp(const CMovieShowApp&) = delete;
    CMovieShowApp& operator=(const CMovieShowApp&) = delete;

protected:
    virtual BOOL InitInstance() override;

private:
    CMainFrame m_frame;
};


#endif // _MOVIESHOWAPP_H_
