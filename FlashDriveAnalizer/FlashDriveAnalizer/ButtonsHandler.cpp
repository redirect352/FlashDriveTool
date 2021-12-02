#include "ButtonsHandler.h"

ATOM MyRegisterClass1(HINSTANCE hInstance);
LRESULT CALLBACK    WndProc1(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

HWND parent1;

void DeleteExtensionHandler(WPARAM wParam, LPARAM lParam, HWND combobox)
{
	int currInd = SendMessage(combobox, CB_GETCURSEL, 0, 0);
	if (currInd < 0)
		return;

    if (MessageBox(NULL, L"Are you sure you want to delete this extension?", L"Confirm action", MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
		 SendMessage(combobox, CB_DELETESTRING, 0, (LPARAM)currInd);
		 if (SendMessage(combobox, CB_GETCOUNT, 0, 0) > 0) 
		 {
			 SendMessage(combobox, CB_SETCURSEL, 0, 0);

		 }
    }
}

void AddExtensionHandler(WPARAM wParam, LPARAM lParam, HWND combobox, HWND parent, HINSTANCE hInstance) 
{
    
    static int a;
    
    
    parent1 = parent;
    if (a == 0)
    {
        ATOM d = MyRegisterClass1(hInstance);
        if (!d)
        {
           // EnableWindow(parent, true);
            DWORD err = GetLastError();
            return;
        }
    }
    a++;
    //|WS_CHILD WS_OVERLAPPEDWINDOW
    HWND hWnd = CreateWindowExW(0,L"Input window class", L"Input data", WS_BORDER|WS_VISIBLE| WS_POPUPWINDOW | WS_CAPTION,
        100, 100, 200, 200, parent, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
       // EnableWindow(parent, true);
        return;
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

    
   // EnableWindow(parent, true);
   // ShowWindow(parent, SW_SHOW);
}


LRESULT CALLBACK WndProc1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
        {
            
            break;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
        }
        break;
        case WM_CLOSE:
            EnableWindow(parent1, true);
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

ATOM MyRegisterClass1(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    memset(&wcex,0,sizeof(wcex));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc1;
    wcex.hInstance = hInstance;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"Input window class";
    wcex.hCursor = LoadCursor(NULL,IDC_CROSS);
    

    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));


    return RegisterClassExW(&wcex);
}

