/////////////////////////////
//FrameApp.cpp
//

#include "stdafx.h"
#include "ReBarApp.h"

/////////////////////////////////
// CReBarApp function definitions
//

// Called when the application starts.
BOOL CReBarApp::InitInstance()
{
    // Create the frame window.
    m_frame.Create();   // throws a CWinException on failure

    return TRUE;
}

