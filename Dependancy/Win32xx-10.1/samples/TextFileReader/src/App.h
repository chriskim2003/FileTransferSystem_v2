/* (22-Oct-2024) [Tab/Indent: 4/4][Line/Box: 80/74]                    (App.h) *
********************************************************************************
|                                                                              |
|                Authors: Robert Tausworthe, David Nash, 2020                  |
|                                                                              |
===============================================================================*

    Contents Description: Declaration of the CApp class for this sample program
    using the Win32++ Windows interface classes.

     Programming Notes: The programming style roughly follows that established
     got the 1995-1999 Jet Propulsion Laboratory Network Planning and
     Preparation Subsystem project for C++ programming.

*******************************************************************************/

#ifndef SDI_APP_H
#define SDI_APP_H

/*============================================================================*/
    class
CApp : public CWinApp                                                       /*

    This class defines the top-level architecture of the program.
*-----------------------------------------------------------------------------*/
{
    public:
        CApp();
        virtual ~CApp() override = default;

        CMainFrame&  GetFrame() { return m_frame;}

    protected:
        virtual BOOL InitInstance() override;

    private:
        CApp(const CApp&) = delete;
        CApp& operator=(const CApp&) = delete;

        CMainFrame  m_frame;
        CString     m_months;
};

/*============================================================================*/
    inline
CApp* TheApp()                                                              /*

    Handy global method for communicating with the application.
*----------------------------------------------------------------------------*/
{
    return (CApp*)GetApp();
}
/*-----------------------------------------------------------------------------*/
#endif // define SDI_APP_H
