/* (20-Oct-2024) [Tab/Indent: 8/8][Line/Box: 80/74]                    (Doc.h) *
********************************************************************************
|                                                                              |
|                    Authors: Robert Tausworthe, David Nash                    |
|                                                                              |
===============================================================================*

    Contents Description: Declaration of the CDoc class for the CommonDialogs
    sample program using the Win32++ Windows interface classes. This class
    provides the document management functions for the application.

    Programming Notes: The programming style roughly follows that established
    by the 1995-1999 Jet Propulsion Laboratory Deep Space Network Planning and
    Preparation Subsystem project for C++ programming.

*******************************************************************************/

#ifndef SDI_DOC_H
#define SDI_DOC_H

class CMainFrame;
class CView;

/*============================================================================*/
    class
CDoc    : public CObject                                                    /*

    This application's document management class.
*-----------------------------------------------------------------------------*/
{
    public:
        CDoc();
        virtual ~CDoc() = default;

        CString GetDocDir()  const;
        CString GetDocPath() const { return m_docPath;}
        BOOL    IsDirty() const;
        BOOL    IsOpen() const {return m_isOpen;}
        BOOL    IsSelected() const;
        BOOL    CanPaste() const;
        BOOL    MakeNewDoc(LPCWSTR);
        void    OnCloseDoc();
        void    OnFindReplace(UINT, WPARAM, LPARAM);
        void    OnFRFindNext(MyFindReplaceDialog*);
        void    OnFRReplaceAll(MyFindReplaceDialog*);
        void    OnFRReplaceCurrent(MyFindReplaceDialog*);
        void    OnFRTerminating(MyFindReplaceDialog*);
        BOOL    OnSaveDoc();
        BOOL    OpenDoc(LPCWSTR);
        void    SetDirty(BOOL b);
        void    SetDataPath(CView*);

    private:
        CHARRANGE   FindNext(const MyFindReplaceDialog&, CHARRANGE);
        CRichEditView& GetRichView() const;
        void        NotFound(const MyFindReplaceDialog&);

        BOOL        m_isOpen;       // the document status
        CString     m_findNext;     // current string to find
        CString     m_replaceWith;  // replacement string
        CString     m_docPath;      // the path of the open document
        CView*      m_data;         // path to the document data
};
/*-----------------------------------------------------------------------------*/
#endif //SDI_DOC_H
