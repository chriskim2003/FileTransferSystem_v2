/////////////////////////////
// MDIFrameApp.cpp
//

#include "stdafx.h"
#include "MDIFrameSplitterApp.h"

////////////////////////////////////////////
// CMDIFrameSplitterApp function definitions
//

// Called when the application starts.
BOOL CMDIFrameSplitterApp::InitInstance()
{
    // Create the Window.
    m_mainMDIFrame.Create();    // throws a CWinException on failure

    return TRUE;
}

