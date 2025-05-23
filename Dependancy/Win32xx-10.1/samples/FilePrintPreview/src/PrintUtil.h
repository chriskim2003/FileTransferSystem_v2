/* (13-Apr-2017) [Tab/Indent: 8/8][Line/Box: 80/74]              (PrintUtil.h) *
********************************************************************************
|                                                                              |
|                    Authors: Robert Tausworthe, David Nash                    |
|                                                                              |
===============================================================================*

    Contents Description: Declaration of printer global utilities using the
    Win32++ Windows interface classes.

    Programming Notes: The programming style roughly follows that established
    for the 1995-1999 Jet Propulsion Laboratory Deep Space Network Planning and
    Preparation Subsystem project for C++ programming.

*******************************************************************************/


#ifndef PRINTERUTIL_H
#define PRINTERUTIL_H

    CRect   GetPageRect(CDC& dcPrinter);
    CSize   GetPPI(CDC&);
    CRect   GetPrinterPageRect(CDC& dcPrinter, CSize margin = CSize(0, 0));
    CRect   GetPrintRect(CDC& dcPrinter);

/*----------------------------------------------------------------------------*/
#endif // PRINTERUTIL_H