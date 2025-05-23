MDIFrame Example
================
This project demonstrates the use of the CMDIFrame class to display a MDI frame
window. MDI (Multiple Document Interface) frames allow different types of view
windows to be displayed simultaneously.

This sample also demonstrates how we can use Dockers for any view window to
provide splitter windows. In this case the MDI Child's view window uses CDocker
to create a splitter window. The splitter window has dock containers docked at 
the left and the right.


Caveat
======
Multiple-document interface (MDI) applications are officially discouraged
by Microsoft. They do not render properly with Per-Monitor (V2) DPI Awareness.

Win32++ provides the CTabbedMDI class which can be used as an alternative to
MDI applications.


Features demonstrated in this example
=====================================
* Use of CMDIFrame and CMDIChild classes to implement a MDI frame.
* Setting a separate menu and icon for the CMDIChild.
* Responding to the MDI specific commands in CMDIFrame::OnCommand.
* Using a splitter(docker) window as a MDI child.
* Adding dock containers to the splitter window.
