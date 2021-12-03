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

bool CheckMinMax(DWORD min, DWORD max, DWORD nest ,Message ms)
{
    if (min < 0 || max < 0 || nest < 0)
    {
        ms(L"Wrong input format");
        return false;

    }
    if (max < min)
    {
        ms(L"Max should be bigger than min");
        return false;
    }
    return true;
}