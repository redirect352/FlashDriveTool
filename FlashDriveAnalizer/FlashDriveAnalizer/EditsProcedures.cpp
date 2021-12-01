#include "framework.h"
#include "EditsProcedures.h"



DWORD CALLBACK GetEditData(HWND edit) 
{
    wchar_t buf[20];
    GetWindowTextW(edit, buf, 19);
    DWORD data;
    wchar_t* end;
    data = wcstol(buf, &end,10);

    if ( wcscmp(buf,L"0")!=0 &&  data ==0) 
    {
        data = -1;
    }
    return data;
}