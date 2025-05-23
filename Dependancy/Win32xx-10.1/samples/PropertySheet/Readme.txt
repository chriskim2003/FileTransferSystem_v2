PropertySheet Example
=====================
A property sheet will have one or more property pages. These pages are much 
like dialogs that are presented within a tabbed dialog or within a wizard. 
The data on a property page can be validated before the next page is presented.
Property sheets have three modes of use: Modal, Modeless, and Wizard.

In this demo CMyProptertySheet inherits from CPropertySheet. It displays
two property pages, namely CButtonPage and CComboPage. The demo shows how a
Modal, Modeless or Wizard property sheet can be displayed.
 

Features demonstrated in this example
=====================================
* Use of CFrame to display the window frame.
* Use of modal, modeless and wizard property sheets.
* Use of OnIdle to dynamically update the toolbar buttons.
* Use of OnMenuUpdate to dynamically update the menu items.
* Using a drop down menu with a toolbar button.
