FormDocView Example
===================
This program has a frame window that includes a Tool Bar and 
Status Bar.  A modeless dialog is used as the view window of our frame.

This project uses an FormDoc class to store the various Form settings, and
save these to the registry. This is much like the Doc/View approach used
in many MFC applications. 


Features demonstrated in this example
=====================================
* Use of CFrame to provide a frame window
* Using a modeless dialog as a view window for the frame.
* Implementing a Doc/View architecture much like MFC.
* Saving the dialog's settings in the registry.
* Use of OnMenuUpdate to update check state of menu items


About Dialogs
=============
Dialogs are easy to create, provided we have access to a resource 
editor to build the resource script file for us (often called
resource.rc). Commercial compilers usually include a resource editor
for this purpose, but free compilers generally don't. A resource
editor is include with Microsoft's Visual Studio Studio Community.



