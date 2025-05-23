/* (06-May-2024) [Tab/Indent: 8/8][Line/Box: 80/74]                    (Doc.h) *
********************************************************************************
|                                                                              |
|               Authors: Robert C. Tausworthe, David Nash, 2020                |
|                                                                              |
===============================================================================*

    Contents Description: Declaration of the the CDoc class for this application
    using the Win32++ framework, Copyright (c) 2005-2020 David Nash, under
    permissions granted therein.

    Programming Notes: The programming style roughly follows that established
    for the 1995-1999 Jet Propulsion Laboratory Deep Space Network Planning and
    Preparation Subsystem project for C++ programming.

*******************************************************************************/

#ifndef SDI_DOC_H
#define SDI_DOC_H

/*============================================================================*/
    class
CDoc : public CObject                           /*
    Application document interface class
*-----------------------------------------------------------------------------*/
{
    public:

        CDoc();
        virtual ~CDoc() override = default;

        int     GetDocLength() const;
        CString GetDocOpenFileName(const CString&) const;
        const CString&
                GetDocPath() const      { return m_docPath; }
        CString GetDocRecord(int, int left = 0, int length = -1) const;
        CString GetDocSaveFileName(const CString&) const;
        int     GetDocWidth() const;
        CTime   GetTimeFromStr(LPCWSTR szTime, int nDST = -1 ) const;
        bool    OpenDoc(const CString &);
        void    NewDocument();
        void    PushContent(const CString&);
        bool    SaveDoc();
        bool    SaveDocAs();
        CString Truth(bool b)           {return b ? L"true" : L"false";}

      // public data members
        static const    CString m_compiledOn;  // date, mmm dd yyyy

    protected:
        virtual void    Serialize(CArchive& ar) override;

    private:
        CString     m_docPath;
        int         m_docWidth;             // length, in characters
        LPCWSTR     m_fileDlgFilter;        // file dialog filter
        std::vector<CString> m_docContent;  // array of document lines
};
/*----------------------------------------------------------------------------*/
#endif //SDI_DOC_H
