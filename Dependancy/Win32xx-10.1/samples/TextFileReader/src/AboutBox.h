/* (10-05-2024) [Tab/Indent: 4/4][Line/Box: 80/74]                (AboutBox.h) *
********************************************************************************
|                                                                              |
|                Authors: Robert Tausworthe, David Nash, 2020                  |
|                                                                              |
===============================================================================*

    Contents Description: Declaration of the AboutBox dialog for this
    sample program using the Win32++ Windows interface classes.

     Programming Notes: The programming style roughly follows that established
     got the 1995-1999 Jet Propulsion Laboratory Network Planning and
     Preparation Subsystem project for C++ programming.

*******************************************************************************/

#ifndef ABOUT_BOX_H_DEFINED
#define ABOUT_BOX_H_DEFINED

/*============================================================================*/
    class
AboutBox    : public CDialog                                            /*

*-----------------------------------------------------------------------------*/
{
    public:
        AboutBox();
        virtual ~AboutBox() override = default;

        void    SetAboutBoxInfo(LPCWSTR info) { m_aboutBoxInfo = info;}

    protected:
        virtual BOOL OnInitDialog() override;

    private:
        AboutBox(const AboutBox&) = delete;
        AboutBox& operator=(const AboutBox&) = delete;

        CString m_aboutBoxInfo; // about the program, credits, etc.
};
/*----------------------------------------------------------------------------*/
#endif // ABOUT_BOX_H_DEFINED
