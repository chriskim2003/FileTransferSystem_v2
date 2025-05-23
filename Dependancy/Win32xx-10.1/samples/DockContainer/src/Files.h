/////////////////////////////////////////////////////////
// Files.h - Declaration of the CViewFiles, CContainFiles
//           and CDockFiles classes.
//

#ifndef FILES_H
#define FILES_H


/////////////////////////////////////////////////////////////
// CViewFiles manages a list view control. It displays files.
// This is the view window for CContainClasses.
class CViewFiles : public CListView
{
public:
    CViewFiles() = default;
    virtual ~CViewFiles() override;
    void SetDPIColumnWidths();
    void SetDPIImages();

protected:
    // Virtual functions that override base class functions.
    virtual void    OnAttach() override;
    virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam) override;

private:
    CViewFiles(const CViewFiles&) = delete;
    CViewFiles& operator=(const CViewFiles&) = delete;

    // Command handlers
    LRESULT OnMouseActivate(UINT msg, WPARAM wparam, LPARAM lparam);
    LRESULT OnDpiChangedBeforeParent(UINT msg, WPARAM wparam, LPARAM lparam);

    void InsertItems();
    void SetColumns();
};


//////////////////////////////////////////////////
// CContainFiles manages a dock container. It uses
// CViewFiles as its view.
class CContainFiles : public CDockContainer
{
public:
    CContainFiles();
    virtual ~CContainFiles() override = default;

protected:
    // Virtual functions that override base class functions.
    virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam) override;

private:
    CContainFiles(const CContainFiles&) = delete;
    CContainFiles& operator=(const CContainFiles&) = delete;

    CViewFiles m_viewFiles;
};


/////////////////////////////////////////////////////
// CDockFiles manages a docker. It uses CContainFiles
// as its view.
class CDockFiles : public CDocker
{
public:
    CDockFiles();
    virtual ~CDockFiles() override = default;

protected:
    virtual void RecalcDockLayout() override;
    virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam) override;

private:
    CDockFiles(const CDockFiles&) = delete;
    CDockFiles& operator=(const CDockFiles&) = delete;

    CContainFiles m_files;
};


#endif // FILES_H
