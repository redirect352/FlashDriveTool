#include "framework.h"
#include "InitProcedures.h"


void initSizeCombobox(HWND sizeChange) 
{
    SendMessage(sizeChange, CB_ADDSTRING, 0, (LPARAM)L"Byte");
    SendMessage(sizeChange, CB_ADDSTRING, 0, (LPARAM)L"Kb");
    SendMessage(sizeChange, CB_ADDSTRING, 0, (LPARAM)L"Mb");
    SendMessage(sizeChange, CB_ADDSTRING, 0, (LPARAM)L"Gb");
    SendMessage(sizeChange, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
}

void initExtensionCombobox(HWND comboboxExtension) 
{

    SendMessage(comboboxExtension, CB_ADDSTRING, 0, (LPARAM)L".pdf");
    SendMessage(comboboxExtension, CB_ADDSTRING, 0, (LPARAM)L".jpg");
    SendMessage(comboboxExtension, CB_ADDSTRING, 0, (LPARAM)L".png");
    SendMessage(comboboxExtension, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

}