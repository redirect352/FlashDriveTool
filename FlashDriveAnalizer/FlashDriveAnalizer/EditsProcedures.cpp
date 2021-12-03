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
void MultiplySizes(HWND sizeCB, DWORD* min, DWORD* max)
{
    int k = SendMessage(sizeCB, CB_GETCURSEL, 0, 0);
    for (size_t i = 0; i < k; i++)
    {
        (*min) *= 1024;
        (*max) *= 1024;
    }
}

bool GetExtension(HWND extCB, const wchar_t* ext) 
{
    int ind = SendMessageW(extCB, CB_GETCURSEL, 0, 0);

    int res = SendMessageW(extCB, CB_GETLBTEXT, ind, (LPARAM)ext);

    if (res == CB_ERR)
    {
        MessageBox(NULL, L"Extension error", L"Error", 0);
        return false;
    }
    return true;
}