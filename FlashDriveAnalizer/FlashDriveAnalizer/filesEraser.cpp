#include "filesEraser.h"


ATOM RegisterWaitWndClass(HINSTANCE hInstance);
LRESULT CALLBACK  WaitWnd(HWND, UINT, WPARAM, LPARAM);
void OutputFileCopyInfo(HDC hdc, DWORD filesize, DWORD alreadyCopied, wchar_t* filename);
int WINAPI CreateWaitWindow(HWND Listview,HWND *, HINSTANCE);

DWORD CALLBACK LpprogressRoutine(
               LARGE_INTEGER TotalFileSize,
               LARGE_INTEGER TotalBytesTransferred,
               LARGE_INTEGER StreamSize,
               LARGE_INTEGER StreamBytesTransferred,
               DWORD dwStreamNumber,
              DWORD dwCallbackReason,
              HANDLE hSourceFile,
               HANDLE hDestinationFile,
      LPVOID lpData
);

HWND par;
typedef struct _CopyParams{
    HWND parent;
    HWND* hwnd;
    HINSTANCE inst;
} CopyParams, *LPCopyParams;

DWORD totalTransfered = 0, totalBytes = 0, fileNumb = -1;
HWND hWnd;
wchar_t* fileNameDr = (wchar_t*)L"";

filesEraser::filesEraser(bool deleteFromListView) 
{
	this->deleteItems = deleteFromListView;
}

int filesEraser::GetFailedToDelete() 
{
	return this->failedToDelete;
}

int filesEraser::GetSuccesfullyDeleted()
{
	return this->successFullyDeleted;
}
void filesEraser::SetDeleteItemsFlag(bool flag)
{
	this->deleteItems = flag;
}
int CALLBACK filesEraser::DeleteSelectedFiles(HWND Listview, Message ms)
{
	this->failedToDelete = 0;
	this->successFullyDeleted = 0;

	int ind = ListView_GetNextItem(Listview,-1, LVNI_SELECTED);
	
	while (ind !=-1) 
	{
		ListView_GetItemText(Listview, ind, 0, buf1, sizeof(buf1));
		ListView_GetItemText(Listview, ind, 1, buf2, sizeof(buf2));
		wcscat_s(buf1, buf2);

		//If file was suuccessfully deleted
		if (DeleteFileW(buf1))
		{
            wsprintfW(buf2, L"Deleted %s", buf1);
			//MessageBox(NULL,buf1,L"Delete",0);
			ms(buf2);
		    ListView_DeleteItem(Listview, ind);
			ind--;
			this->successFullyDeleted++;
		}
		else 
		{
            int res = GetLastError();
            wsprintfW(buf2, L"Error! Cannot delete file %s", buf1);
            ms(buf2);
            this->failedToDelete++;
		}
		ind = ListView_GetNextItem(Listview, ind, LVNI_SELECTED);
	}


	return this->successFullyDeleted;
}



int CALLBACK filesEraser::CopySelectedFiles(HWND Listview, wchar_t* copyPath, Message ms, HWND parentWindow, bool deleteCopiedfiles)
{
    this->failedToDelete = 0;
    this->successFullyDeleted = 0;
    wchar_t fileName[255];
    static int a;

	int count = 0;
	int ind = ListView_GetNextItem(Listview, -1, LVNI_SELECTED);
	while (ind!=-1)
	{
		count++;
		ind = ListView_GetNextItem(Listview, ind, LVNI_SELECTED);
	}
    if (count == 0)
        return 0;
    if (a == 0)
    {
        ATOM d = RegisterWaitWndClass((HINSTANCE)GetWindowLongW(parentWindow, GWL_HINSTANCE));
        if (!d)
        {
            DWORD err = GetLastError();
            return 0;
        }
    }
    a++;

    LPCopyParams params = (LPCopyParams)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(CopyParams));
    if (params == NULL)
    {
        ExitProcess(2);
    }
    params->hwnd = &hWnd;
    params->parent = parentWindow;
    params->inst = (HINSTANCE)GetWindowLong(parentWindow,GWLP_HINSTANCE);
    DWORD id;
    HANDLE thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CreateWaitWindow,params,0,&id);
    if (thread == 0)
        return -1;
    ind = ListView_GetNextItem(Listview, -1, LVNI_SELECTED);
    int k = 0;
    while (ind != -1)
    {
        k++;
        ListView_GetItemText(Listview, ind, 0, buf1, sizeof(buf1));
        ListView_GetItemText(Listview, ind, 1, fileName, sizeof(buf2));
        wcscat_s(buf1, fileName);
        wcscpy_s(buf2, copyPath);
        wcscat_s(buf2, fileName);
        fileNameDr = fileName;

        DWORD info = count;
        info = info << 16;
        info = info |k;
        //If file was suuccessfully deleted
        if (k == count)
            Sleep(500);
        if (CopyFileExW(buf1, buf2, LpprogressRoutine, &info, NULL, 0))
        {
            if (deleteCopiedfiles) {
                if (DeleteFileW(buf1))
                {
                    wsprintfW(buf2, L"Deleted %s", buf1);
                    ms(buf2);
                    ListView_DeleteItem(Listview, ind);
                    ind--;
                    
                }
                else
                {
                    int res = GetLastError();
                    wsprintfW(buf2, L"Error! Cannot delete file %s", buf1);
                    ms(buf2);
                }
            }
            this->successFullyDeleted++;
            wcscat_s(buf1, L" copied");
             ms(buf1);
             
        }
        else
        {
            this->failedToDelete++;
      
        }
        ind = ListView_GetNextItem(Listview, ind, LVNI_SELECTED);
    }


    WaitForSingleObject(thread,INFINITE);
    CloseHandle(thread);
    HeapFree(GetProcessHeap(),0,params);
    params = NULL;
    return this->successFullyDeleted;

}
int WINAPI CreateWaitWindow(HWND parentWindow, HWND * _hwnd, HINSTANCE inst)
{
    par = parentWindow;
    HWND hWnd = *_hwnd;

    WNDCLASSEXW wcex;
    memset(&wcex, 0, sizeof(wcex));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WaitWnd;
    wcex.hInstance = inst;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"Wait_Wnd";
    wcex.hCursor = LoadCursor(NULL, IDC_WAIT);
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(108));
    RegisterClassExW(&wcex);
    hWnd = CreateWindowExW(WS_EX_NOPARENTNOTIFY, L"Wait_Wnd", L"Files are copied...", WS_BORDER | WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION,
        250, 250, 300, 130, NULL, nullptr, inst, nullptr);
    if (!hWnd)
    {
        DWORD err = GetLastError();
        return 0;
    }
    MSG msg;
    ShowWindow(hWnd, SW_NORMAL);
    UpdateWindow(hWnd);
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, nullptr, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }


    return 0 ;//this->successFullyDeleted;

}



DWORD CALLBACK LpprogressRoutine(
               LARGE_INTEGER TotalFileSize,
               LARGE_INTEGER TotalBytesTransferred,
              LARGE_INTEGER StreamSize,
               LARGE_INTEGER StreamBytesTransferred,
               DWORD dwStreamNumber,
               DWORD dwCallbackReason,
               HANDLE hSourceFile,
               HANDLE hDestinationFile,
     LPVOID lpData
) 
{

    totalBytes = TotalFileSize.LowPart;
    totalTransfered = TotalBytesTransferred.LowPart;
    fileNumb = *((DWORD*)lpData);
    return 0;
}


ATOM RegisterWaitWndClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    memset(&wcex, 0, sizeof(wcex));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WaitWnd;
    wcex.hInstance = hInstance;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"Wait_Wnd";
    wcex.hCursor = LoadCursor(NULL, IDC_WAIT);
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(108));
    return RegisterClassExW(&wcex);
}

LRESULT CALLBACK WaitWnd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        /*CreateWindow(L"BUTTON", L"Ok", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            20, 50, 100, 25, hWnd, (HMENU)BUTTON_OK_DLG_INPUT_EXTENSION, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        CreateWindow(L"BUTTON", L"Cancel", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            160, 50, 100, 25, hWnd, (HMENU)BUTTON_CANCEL_DLG_INPUT_EXTENSION, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

        edit = CreateWindow(L"Edit", L"", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER,
            20, 10, 240, 25, hWnd, (HMENU)EDIT_EXTENSION_DLG_INPUT_EXTENSION, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        SetFocus(edit);*/
        SetTimer(hWnd, 1,30,NULL);

        break;
    }
    case WM_TIMER:
    {
        InvalidateRect(hWnd,NULL,TRUE);
    }
    case WM_COMMAND:
    {
        switch (wParam)
        {
            //case BUTTON_OK_DLG_INPUT_EXTENSION:
            //{

            //    //MessageBox(NULL, L"OK", L"", 0);
            //    //CloseWindow(hWnd);
            //    GetWindowTextW(edit, buf, 255);
            //    if (wcslen(buf) <= 2)
            //    {
            //        MessageBox(NULL, L"Input correct extension", L"Error!", 0);
            //        return 0;
            //    }

            //    SendMessageW(hWnd, WM_CLOSE, 0, 0);
            //    break;
            //}
            //case BUTTON_CANCEL_DLG_INPUT_EXTENSION:
            //{
            //    SendMessageW(hWnd, WM_CLOSE, 0, 0);
            //    break;
            //}


            //default:
            //    break;
            //}
            //break;
        }
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        OutputFileCopyInfo(hdc,totalBytes,totalTransfered, fileNameDr);
        if (HIWORD(fileNumb) == LOWORD(fileNumb) && fileNumb !=-1)
            SendMessageW(hWnd,WM_CLOSE,0,0);

        EndPaint(hWnd, &ps);
        break;
    }    
    case WM_CLOSE:
        EnableWindow(par, true);
        PostMessageW(par,WM_ENABLE,true,0);
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    case WM_DESTROY:

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


void OutputFileCopyInfo(HDC hdc, DWORD filesize, DWORD alreadyCopied, wchar_t* filename)
{
    wchar_t buf[512];
    wsprintfW(buf,L"Copying file %d / %d", LOWORD(fileNumb), HIWORD(fileNumb));
    TextOut(hdc,10,10,buf,wcsnlen_s(buf,512));
    wsprintfW(buf, L"Copied %ld / %ld bytes", alreadyCopied, filesize);
    TextOut(hdc, 10, 30, buf, wcsnlen_s(buf, 512));
}
