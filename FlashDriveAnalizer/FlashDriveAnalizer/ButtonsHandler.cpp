#include "ButtonsHandler.h"

ATOM MyRegisterClass1(HINSTANCE hInstance);
LRESULT CALLBACK    WndProc1(HWND, UINT, WPARAM, LPARAM);

HWND edit;

wchar_t buf[255];

bool getCheckState(HWND ckb1) 
{
    LRESULT  res = SendMessageW(ckb1,BM_GETCHECK,0,0);
    if (res == BST_CHECKED)
        return true;
    return false;
}


void DeleteExtensionHandler(WPARAM wParam, LPARAM lParam, HWND combobox)
{
	int currInd = SendMessage(combobox, CB_GETCURSEL, 0, 0);
	if (currInd < 0)
		return;

    if (MessageBox(NULL, L"Are you sure you want to delete this extension?", L"Confirm action", MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
		 SendMessage(combobox, CB_DELETESTRING, currInd, 0);
		 if (SendMessage(combobox, CB_GETCOUNT, 0, 0) > 0) 
		 {
			 SendMessage(combobox, CB_SETCURSEL, 0, 0);

		 }
    }
}

void AddExtensionHandler(WPARAM wParam, LPARAM lParam, HWND combobox, HWND parent, HINSTANCE hInstance)
{

    static int a;
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
    HWND hWnd = CreateWindowExW(0, L"Input window class", L"Input data", WS_BORDER | WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION,
        250, 250, 300, 130, parent, nullptr, hInstance, nullptr);

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

    if (wcslen(buf)>2)
    {
        SendMessage(combobox, CB_ADDSTRING, 0, (LPARAM)buf);
        int count = SendMessage(combobox, CB_GETCOUNT, 0, 0);
        SendMessage(combobox, CB_SETCURSEL, count - 1, 0);
        
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
            CreateWindow(L"BUTTON", L"Ok", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                20, 50, 100, 25, hWnd, (HMENU)BUTTON_OK_DLG_INPUT_EXTENSION, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
            CreateWindow(L"BUTTON", L"Cancel", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                160,50, 100, 25, hWnd, (HMENU)BUTTON_CANCEL_DLG_INPUT_EXTENSION, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

            edit = CreateWindow(L"Edit", L"", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER,
                20, 10, 240, 25, hWnd, (HMENU)EDIT_EXTENSION_DLG_INPUT_EXTENSION, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
            SetFocus(edit);

            break;
        }
        case WM_COMMAND: 
        {
            switch (wParam)
            {
            case BUTTON_OK_DLG_INPUT_EXTENSION: 
            {
                
                //MessageBox(NULL, L"OK", L"", 0);
                //CloseWindow(hWnd);
                GetWindowTextW(edit, buf, 255);
                if (wcslen(buf)<=2)
                {
                    MessageBox(NULL, L"Input correct extension", L"Error!", 0);
                    return 0;
                }
                
                SendMessageW(hWnd,WM_CLOSE,0,0);
                break;
            }
            case BUTTON_CANCEL_DLG_INPUT_EXTENSION:
            {
                SendMessageW(hWnd, WM_CLOSE, 0, 0);
                break;
            }


            default:
                break;
            }
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
            EnableWindow(GetParent(hWnd), true);
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

