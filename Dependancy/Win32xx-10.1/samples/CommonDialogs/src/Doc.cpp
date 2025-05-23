/* (20-Oct-2024) [Tab/Indent: 8/8][Line/Box: 80/74]                  (Doc.cpp) *
********************************************************************************
|                                                                              |
|                    Authors: Robert Tausworthe, David Nash                    |
|                                                                              |
===============================================================================*

    Contents Description:  Implementation of the CDoc class for the
    CommonDialogs sample application using the Win32++ Windows interface
    classes.

    Programming notes: Documents in this sample program are assumed to derive
    from simple text files of single byte characters from the Windows standard
    UTF8 code page.  For a complete Unicode operation, see the NotePad sample
    function in this distribution.

    Internally, the document contents are managed (as well as viewed) by the
    RichEditView class.

    Programming Notes: The programming style roughly follows that established
    by the 1995-1999 Jet Propulsion Laboratory Deep Space Network Planning and
    Preparation Subsystem project for C++ programming.

*******************************************************************************/

#include "stdafx.h"
#include "StdApp.h"

/*============================================================================*/
    CDoc::
CDoc()                                                                      /*

*-----------------------------------------------------------------------------*/
    : m_isOpen(FALSE), m_data(nullptr)
{
}

/*============================================================================*/
    CHARRANGE CDoc::
FindNext(const MyFindReplaceDialog& FR, CHARRANGE r)                        /*

    Find the next occurrence of the string returned by the find-replace
    dialog box FR in the document starting with the given character range
    and return the found character range, or an end-of-text range otherwise.
*-----------------------------------------------------------------------------*/
{
      // get find string that does not go out of scope
    m_findNext   = FR.GetFindString();
      // set the search parameters
    BOOL match = FR.MatchCase();
    BOOL whole = FR.MatchWholeWord();
    BOOL down  = FR.SearchDown();
    DWORD dwFlags = (match ? FR_MATCHCASE : 0) |
            (whole ? FR_WHOLEWORD : 0)         |
            (down  ? FR_DOWN : 0);
      // Perform the search per the dwFlags and the FINDTEXEX contents:
      // search down: end of current selection to end of text, or
      // search up:   beginning of current selection to beginning of text
    r.cpMin = (down ? r.cpMax : r.cpMin);
    r.cpMax = (down ? -1 : 0);
      // get current location or selection
    FINDTEXTEX ftx;
    ftx.chrg = r;
    ftx.lpstrText = const_cast<LPWSTR>(m_findNext.c_str());
    GetRichView().FindText(dwFlags, ftx);
    return ftx.chrgText;
}

/*============================================================================*/
    CString CDoc::
GetDocDir() const                                                          /*

    Returns the directory of the file associated with this object.
*----------------------------------------------------------------------------*/
{
    CFile f; f.SetFilePath(m_docPath);
    return f.GetFileDirectory();
}

/*============================================================================*/
    CRichEditView& CDoc::
GetRichView() const                                                         /*

    Return a reference to the document data repository object.
*----------------------------------------------------------------------------*/
{
    return m_data->GetRichView();
}

/*============================================================================*/
    BOOL CDoc::
IsDirty() const                                                             /*

    Indicate the modification state of the document text.
*-----------------------------------------------------------------------------*/
{
    return GetRichView().GetModify() && IsOpen();
}

/*============================================================================*/
    BOOL CDoc::
IsSelected() const                                                          /*

    Returns TRUE if text is selected.
*-----------------------------------------------------------------------------*/
{
    CHARRANGE range;
    GetRichView().GetSel(range);
    return m_isOpen && (range.cpMin != range.cpMax);
}

/*============================================================================*/
    BOOL CDoc::
CanPaste() const                                                            /*

    Returns TRUE if text can be pasted.
*-----------------------------------------------------------------------------*/
{
    return m_isOpen && GetRichView().CanPaste(CF_TEXT);
}

/*============================================================================*/
    BOOL CDoc::
MakeNewDoc(LPCWSTR docPath)                                                 /*

    Open a new document with the given filename; return TRUE if able to do
    so, or FALSE otherwise.
*-----------------------------------------------------------------------------*/
{
    SetDirty(FALSE);
    m_docPath.Empty();
    m_isOpen = FALSE;
    try      // try to create a file with the given path
    {
          // create the empty document file and close it
        CFile f(docPath, CREATE_NEW);
        f.Close();
    }
    catch (...) // if there was an error in opening the file
    {
        CString msg = "Could not create document file:\n";
        msg += docPath;
        ::MessageBox(nullptr, msg, L"Information", MB_OK |
            MB_ICONINFORMATION | MB_TASKMODAL);
        return FALSE;
    }
    return TRUE;
}

/*============================================================================*/
    void CDoc::
NotFound(const MyFindReplaceDialog& FR)                                     /*

    Post a message box relating that the current search did not find the
    string sought for.
*-----------------------------------------------------------------------------*/
{
    BOOL match = FR.MatchCase();
    BOOL whole = FR.MatchWholeWord();
    CString msg;
    LPCWSTR wholeword = (whole ? L"\nas a whole word" : L"");
    LPCWSTR matchcase = (match ? L"\nmatching case" : L"");
    msg.Format(L"'%s'was not found%s%s.", m_findNext.c_str(),
        wholeword, matchcase);
    ::MessageBox(nullptr, msg, L"Information", MB_OK |
        MB_ICONINFORMATION | MB_TASKMODAL);
}

/*============================================================================*/
    void CDoc::
OnCloseDoc()                                                                /*

    Perform any cleanup necessary to close the document.
*-----------------------------------------------------------------------------*/
{
    if (!IsOpen())
        return;

      //Check for unsaved text
    CString msg;
    msg.Format(L"Save changes to this document?\n    %s",
        m_docPath.c_str());
    if (m_isOpen && IsDirty() && (::MessageBox(nullptr, msg,
        L"Question...", MB_YESNO | MB_ICONQUESTION) == IDYES))
    {
        OnSaveDoc();
        return;
    }
      // mark the document is not open, with zero length
    m_isOpen = FALSE;
    m_docPath.Empty();
}

/*============================================================================*/
    void CDoc::
OnFindReplace(UINT msg, WPARAM wparam, LPARAM lparam)                       /*

    This method responds to the FINDMSGSTRING registered message sent by
    the find/replace dialog box. This is the callback function response
    method for all user interactions with the dialog boxes.
*-----------------------------------------------------------------------------*/
{
    UNREFERENCED_PARAMETER(msg);
    UNREFERENCED_PARAMETER(wparam);

    MyFindReplaceDialog* fr =
        (MyFindReplaceDialog*)MyFindReplaceDialog::GetNotifier(lparam);
    assert(fr != nullptr);
    if (fr != nullptr)
    {
        if (fr->IsTerminating())
            OnFRTerminating(fr);
        else if (fr->FindNext())
            OnFRFindNext(fr);
        else if (fr->ReplaceCurrent())
            OnFRReplaceCurrent(fr);
        else if (fr->ReplaceAll())
            OnFRReplaceAll(fr);
    }
}

/*============================================================================*/
    void CDoc::
OnFRFindNext(MyFindReplaceDialog* fr)                                      /*

    Find the next occurrence of the string returned by the find dialog box
    in the document.
*-----------------------------------------------------------------------------*/
{
      // get current location or selection
    CHARRANGE r;
    GetRichView().GetSel(r);
      // find the next occurrence
    r = FindNext(*fr, r);
    if (r.cpMin >= 0)
    {     // select the text and set the focus to show it
        GetRichView().SetSel(r);
        GetRichView().SetFocus();
    }
    else
        NotFound(*fr);
}

/*============================================================================*/
    void CDoc::
OnFRReplaceAll(MyFindReplaceDialog* fr)                                     /*

    Replace all occurrences of the string returned by the replace dialog box
    in the document by the string returned by that box.
*-----------------------------------------------------------------------------*/
{
      // get replacement string that does not go out of scope
    m_replaceWith = fr->GetReplaceString();
      // search the entire range, start at character 0
    CHARRANGE r = { 0, 0 };
    CHARRANGE r0 = r;
    r = FindNext(*fr, r);
      // if not found, say so
    if (r.cpMin < 0)
    {
        NotFound(*fr);
        return;
    }
      // replace this occurrence, seek the next, and repeat until no more
    do
    {
        GetRichView().SetSel(r);
        GetRichView().ReplaceSel(m_replaceWith.c_str(), TRUE);
          // save the last replacement site and reset the search
          // beginning point for the next
        r0 = r;
        r.cpMin = r.cpMax;
        r = FindNext(*fr, r);
    } while (r.cpMin >= 0);
      // show the last replacement
    r0.cpMax = r0.cpMin + m_replaceWith.GetLength();
    GetRichView().SetSel(r0);
    GetRichView().SetFocus();
}

/*============================================================================*/
    void CDoc::
OnFRReplaceCurrent(MyFindReplaceDialog* fr)                                 /*

    Replace the next occurrence of the string returned by the replace dialog
    box in the document by the string returned by that box.
*-----------------------------------------------------------------------------*/
{
      // get replacement string that does not go out of scope
    m_replaceWith = fr->GetReplaceString();
      // get current location or selection
    CHARRANGE r;
    GetRichView().GetSel(r);
      // find the next occurrence
    r = FindNext(*fr, r);
    if (r.cpMin >= 0)
    {     // select the text, make the replacement, and set the
          // focus to show it
        GetRichView().SetSel(r);
        GetRichView().ReplaceSel(m_replaceWith, TRUE);
        r.cpMax = r.cpMin + m_replaceWith.GetLength();
        GetRichView().SetSel(r);
        GetRichView().SetFocus();
    }
    else
        NotFound(*fr);
}

/*============================================================================*/
    void CDoc::
OnFRTerminating(MyFindReplaceDialog* fr)                                   /*

    Perform any actions necessary to terminate the FindReplaceDialog box.
*-----------------------------------------------------------------------------*/
{
    UNREFERENCED_PARAMETER(fr);
}

/*============================================================================*/
    BOOL CDoc::
OnSaveDoc()                                                                 /*

    Save current values of the document back into the currently named
    source file. Return TRUE if the document was not open or not dirty, or
    is saved properly, or FALSE otherwise.
*-----------------------------------------------------------------------------*/
{
      // if no document is open or, if open, not dirty
    if (!IsOpen() || !IsDirty())
        return TRUE;

      // make sure the file is ok to save
    if (m_docPath.IsEmpty())
    {
        CString msg = "Attempt to save an invalid file.";
        ::MessageBox(nullptr, msg, L"Information",
            MB_OK | MB_ICONINFORMATION | MB_TASKMODAL);
        m_isOpen = FALSE;
        return FALSE;
    }

    try
    {
        CFile f(m_docPath, CREATE_ALWAYS);
          // if there was no throw, the document opened
        GetRichView().StreamOutFile(f);
        f.Close();
        m_isOpen = TRUE;
    }
    catch (...) // if there was an error
    {
        CString msg = "Document file did not save.";
        msg += m_docPath;
        ::MessageBox(nullptr, msg, L"Information", MB_OK |
            MB_ICONINFORMATION | MB_TASKMODAL);
        m_isOpen = FALSE;
        m_docPath.Empty();
        return FALSE;
    }
      // document will not be dirty on exit, whatever its current state
    SetDirty(FALSE);
    return TRUE;
}

/*============================================================================*/
    BOOL CDoc::
OpenDoc(LPCWSTR docPath)                                                    /*

    Open the document from the given path.
    Return TRUE if file is open on return, FALSE if not.
*-----------------------------------------------------------------------------*/
{
    CString msg;
    if (CString(docPath).CompareNoCase(m_docPath) == 0)
    {
        msg.Format(L"Document file\n    '%s'\nis already open.",
            m_docPath.c_str());
        ::MessageBox(nullptr, msg, L"Information", MB_OK |
            MB_ICONINFORMATION | MB_TASKMODAL);
          // not deemed a failure, as the file is open, as specified
        return TRUE;
    }
      // try to open (it should, as we know it exists, but still ...)
    try
    {
        CFile f(docPath, OPEN_EXISTING);
        GetRichView().StreamInFile(f);
        m_docPath = docPath;
        f.Close();
        m_isOpen = TRUE;
    }
    catch (...) // if there was an error in opening the file
    {
        msg.Format(L"Document file\n    '%s'\ndid not open.", docPath);
        ::MessageBox(nullptr, msg, L"Information", MB_OK |
            MB_ICONINFORMATION | MB_TASKMODAL);
        m_isOpen = FALSE;
        m_docPath.Empty();
        return FALSE;
    }
      // regardless of whether it opens, it is not dirty
    SetDirty(FALSE);
    return TRUE;
}

/*============================================================================*/
    void CDoc::
SetDataPath(CView* path)                                                    /*

    Register the path to the document data repository.
*-----------------------------------------------------------------------------*/
{
    m_data = path;
}

/*============================================================================*/
    void CDoc::
SetDirty(BOOL b)                                                            /*

    Set the real edit window text modification state to b.
*-----------------------------------------------------------------------------*/
{
    GetRichView().SetModify(b);
}
/*----------------------------------------------------------------------------*/

