// FlashDriveAnalizer.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "FlashDriveAnalizer.h"
#include "Dbt.h"
#include "FlashMonitor.h"
#include "FilesFinder.h"
#include "EditsProcedures.h"



#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void CALLBACK CreateWindowControls(HWND hWnd);

HWND hlist, hbutton;
HWND clearDriveButton;
HWND flashDrivesList;
HWND MinSize, MaxSize;
HWND MaxSearchNesting;
HWND sizeChange;
HWND filesList;
HWND  deleteButton;
HWND deleteAllButton;

flashMonitor* mon;
filesFinder f = filesFinder::filesFinder((wchar_t*)L"D\\");


wchar_t chosenDrive[10];
bool isAnyDriveChoosen = false;




int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.
    
    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FLASHDRIVEANALIZER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FLASHDRIVEANALIZER));


    mon = flashMonitor::create( hInstance,false);
    mon->on_device_add(device_added);
    mon->on_device_remove(device_removed);
    mon->on_device_safe_remove(device_safe_removed);
    mon->on_device_remove_fail(device_remove_failed);
    mon->start();
    MSG msg;
   
    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FLASHDRIVEANALIZER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_FLASHDRIVEANALIZER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        CreateWindowControls(hWnd);
        OutputDrivesInListBox();


        break;
    }
    case WM_DEVICECHANGE:
    {
        OutputDrivesInListBox();
        LONG_PTR data = GetWindowLongPtrW(hWnd, GWLP_USERDATA);
        if (data)
            return reinterpret_cast<flashMonitor*>(data)->devices_changed(wParam, lParam);

        
    }

    case WM_NOTIFY: 
    {
        UINT id = ((LPNMHDR)lParam)->idFrom;
        switch (id)
        {
           

            case LVN_COLUMNCLICK:
            {
                //ListView_SortItems(filesList, LVCompareProc,(LPARAM)0);
            }
            default:{
                break;
            }

            break;
        }
        
        break;
    }

        case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case BUTTON_SEARCH_FILES:
            {
                ListView_DeleteAllItems(filesList);
                
                wchar_t t[] = L"D:\\";
                f.SetCurrentRoot((wchar_t*)t);
                
                DWORD min = GetEditData(MinSize), max = GetEditData(MaxSize),nest =  GetEditData(MaxSearchNesting);
                if (min < 0 || max < 0 || nest<0)
                {
                    OutputAdditionalInfo(L"Wrong input format" );
                    return NULL;

                }
                if (max < min) 
                {
                    OutputAdditionalInfo(L"Max should be bigger than min");
                    return NULL;
                }


                int k = SendMessage(sizeChange, CB_GETCURSEL, 0, 0);
                for (size_t i = 0; i < k; i++)
                {
                    min *= 1024;
                    max *= 1024;
                }
                int l;

                
                f.findFilesBySize(filesList,min,max,&l,nest,2);
                
                break;
            }
            case BUTTON_CLEAR_DRIVE: 
            {
                if (!isAnyDriveChoosen) 
                {
                    MessageBox(NULL, L"Choose flash drive", L"Error", 0);
                    return NULL;
                }
                wchar_t message[] = L"Вы точно хотите очистить данный диск? После этого восстановить данные будет невозможно";
                if (MessageBox(hWnd, message, L"Confirm action", MB_ICONQUESTION | MB_YESNO) == IDYES)
                {

                    wchar_t message2[100];
                    wsprintfW(message2, L"Usb drive %s cleared", chosenDrive);
                    OutputAdditionalInfo(message2);
                    return true;
                }
                else
                {
                    wchar_t message2[] = L"USB drive erase canseled";
                    OutputAdditionalInfo(message2);
                    return false;
                }
            
            }
            case LISTBOX_FLASHDRIVE:
            {
                switch (HIWORD(wParam))
                {
                case LBN_SELCHANGE: 
                {
                    //MessageBox(hWnd, L"Confirm action", L"Confirm action", MB_ICONQUESTION | MB_YESNO);
                    int ind = SendMessage(flashDrivesList, LB_GETCURSEL, 0, 0);
                    if (ind == LB_ERR) 
                    {
                        return NULL;
                    }
                    wchar_t buf[100], tmp[100];
                    ind = SendMessageW(flashDrivesList, LB_GETTEXT, (WPARAM)ind, (LPARAM)buf);
                    if (ind >100) 
                    {
                        buf[99] = 0;
                    }
                    ind = -1000;
                    for (int i = 0; i<99 && buf[i]!='\0' && buf[i]!=')';i++) 
                    {
                        if (buf[i]=='(') 
                        {
                            ind = -1;
                            
                        }
                        if (ind>=0 && ind < 10) 
                        {
                            chosenDrive[ind] = buf[i];
                        }
                        ind++;
                    
                    }
                    RECT r;
                    r.left = 0;
                    r.right = 300;
                    r.top = 0;
                    r.bottom = 50;
                    InvalidateRect(hWnd,&r, TRUE );
                    isAnyDriveChoosen = true;

                }
                default:
                    break;
                }
                break;
            }

          
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            DrawWindowText(hdc);
            EndPaint(hWnd, &ps);
        }
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
    }

    // Обработчик сообщений для окна "О программе".
    INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
    {
        UNREFERENCED_PARAMETER(lParam);
        switch (message)
        {
        case WM_INITDIALOG:
            return (INT_PTR)TRUE;
            
        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
            break;
        }
        return (INT_PTR)FALSE;
    }

    void CALLBACK CreateWindowControls(HWND hWnd)
    {
        hlist = CreateWindow(WC_LISTBOX , NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | LBS_NOTIFY,
            20, 350, 500, 50, hWnd, (HMENU)8, (HINSTANCE)GetWindowLongA(hWnd, -6), NULL);
        
        filesList = CreateWindow(WC_LISTVIEWW   , NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | LVS_EX_FULLROWSELECT | LBS_NOTIFY,
            170, 20, 400, 300, hWnd, (HMENU)LISTVIEW_FILES, (HINSTANCE)GetWindowLongA(hWnd, -6), NULL);

        ListView_SetExtendedListViewStyle(filesList,ListView_GetExtendedListViewStyle(filesList)|LVS_EX_FULLROWSELECT|LVS_EX_AUTOSIZECOLUMNS
        |LVS_EX_CHECKBOXES);
        flashDrivesList = CreateWindow(WC_LISTBOX   , NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | LBS_NOTIFY,
            10, 20, 130, 100, hWnd, (HMENU)LISTBOX_FLASHDRIVE, (HINSTANCE)GetWindowLongA(hWnd, -6), NULL);

        hbutton = CreateWindow(WC_BUTTON    , L"Search", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            600, 290, 100, 29, hWnd, (HMENU)BUTTON_SEARCH_FILES, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        deleteButton = CreateWindow(WC_BUTTON, L"Delete", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            710, 290, 100, 29, hWnd, (HMENU)BUTTON_DELETE_FILES, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        deleteButton = CreateWindow(WC_BUTTON, L"Delete All", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            820, 290, 100, 29, hWnd, (HMENU)BUTTON_DELETE_ALL_FILES, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);


        clearDriveButton = CreateWindow(L"BUTTON", L"ClearDrive", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 130, 130, 20, hWnd, (HMENU)BUTTON_CLEAR_DRIVE, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
       
        MinSize = CreateWindow(WC_EDIT  , L"0", WS_TABSTOP | WS_VISIBLE | WS_CHILD|WS_BORDER,
            645, 50, 90, 20, hWnd, (HMENU)EDIT_MIN_SIZE, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        MaxSize = CreateWindow(WC_EDIT     , L"1000", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER,
            770, 50, 90, 20, hWnd, (HMENU)EDIT_MAX_SIZE, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        MaxSearchNesting = CreateWindow(WC_EDIT , L"3", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER,
            710, 20, 25, 20, hWnd, (HMENU)EDIT_MAX_SEARCH_NESTING, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

        sizeChange = CreateWindow(WC_COMBOBOX   , L"DLL", WS_CHILD | WS_VISIBLE | CBS_HASSTRINGS | CBS_DROPDOWNLIST
            | WS_OVERLAPPED | WS_VSCROLL, 870, 49, 60,100 , hWnd, (HMENU)COMBOBOX_SIZE, (HINSTANCE)GetWindowLongA(hWnd, -6), NULL);

        SendMessage(hlist, WM_SETREDRAW, TRUE, 0L);

        

        SendMessage(sizeChange, CB_ADDSTRING, 0, (LPARAM)L"Byte");
        SendMessage(sizeChange, CB_ADDSTRING, 0, (LPARAM)L"Kb");
        SendMessage(sizeChange, CB_ADDSTRING, 0, (LPARAM)L"Mb");
        SendMessage(sizeChange, CB_ADDSTRING, 0, (LPARAM)L"Gb");
        SendMessage(sizeChange, WM_SETREDRAW, TRUE, 0L);
        SendMessage(sizeChange, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
        
        AddColumnToLIstview(filesList, L"File path",200);
        AddColumnToLIstview(filesList, L"File name", 100);
        AddColumnToLIstview(filesList, L"File size(bytes)", 100);
    }

    void CALLBACK DrawWindowText(HDC hdc)
    {

        TextOut(hdc, 10, 3, L"Inserted flash drives", 22);
        TextOut(hdc, 170, 3, L"Selected drive: ", 17);
        TextOut(hdc, 573, 52, L"Size from:", 11);
        TextOut(hdc, 745, 52, L"to ", 2);
        TextOut(hdc, 573, 22, L"Max search nesting:", 20);


        TextOut(hdc, 280, 3, chosenDrive, wcslen(chosenDrive));
    }


    void CALLBACK OutputDrivesInListBox() 
    {
        SendMessage(flashDrivesList, LB_RESETCONTENT, NULL, NULL);
        std::set<wchar_t> res = mon->get_flash_disks(false);
        wchar_t driveTemplate[] = L"!:\\";
        wchar_t driveName[255];
        ;
        for (auto s :res) 
        {
            driveTemplate[0] = s;
            GetVolumeInformationW(driveTemplate, driveName, 255,NULL, NULL, NULL, NULL,0);
            wsprintfW(driveName,L"%s(%s)",driveName,driveTemplate);
            SendMessage(flashDrivesList, LB_ADDSTRING, NULL, (LPARAM)driveName);
        }
        SendMessage(flashDrivesList, LB_ADDSTRING, NULL, (LPARAM)L"FakeDrive1(E:\\)");
        SendMessage(flashDrivesList, LB_ADDSTRING, NULL, (LPARAM)L"FakeDrive2(G:\\)");
        SendMessage(flashDrivesList, LB_ADDSTRING, NULL, (LPARAM)L"vFakeDrive3(K:\\)");
    }

    void device_added(char letter)
    {
        wchar_t message[] = L"Added USB drive : 0";
        message[17] = (wchar_t)letter;
        OutputAdditionalInfo(message);
    }

    void device_removed(char letter)
    {
        wchar_t message[] = L"UNSAFE-removed USB drive: 0";
        message[25] = (wchar_t)letter;
        OutputAdditionalInfo(message);
    }

    bool device_safe_removed(char letter)
    {
        wchar_t message[] = L"Разрешить извлечь диск 0 ?";
        message[23] = (wchar_t)letter;
        if (MessageBox(NULL, 0, message, MB_ICONQUESTION | MB_YESNO) == IDYES)
        {

            wchar_t message2[] = L"Safe-removed USB drive: 0";
            message2[22] = (wchar_t)letter;
            OutputAdditionalInfo(message2);
            return true;
        }
        else
        {
            return false;
        }
    }

    void device_remove_failed(char letter)
    {
        wchar_t message[] = L"Failed to eject device: 0";
        message[24] = (wchar_t)letter;
        OutputAdditionalInfo(message);
    }


    
    void CALLBACK OutputAdditionalInfo( const wchar_t *info) 
    {
        SendMessage(hlist, LB_ADDSTRING, NULL, (LPARAM)info);
    }
    


