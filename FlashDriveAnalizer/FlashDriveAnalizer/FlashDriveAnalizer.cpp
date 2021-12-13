// FlashDriveAnalizer.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "FlashDriveAnalizer.h"
#include "Dbt.h"
#include "FlashMonitor.h"
#include "FilesFinder.h"
#include "EditsProcedures.h"
#include "FilesEraser.h"
#include "FilesEraser.h"
#include "ButtonsHandler.h"
#include "InitProcedures.h"
#include "shlobj.h"
#include "shellapi.h"

#include "thread"
#include "mutex"

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
void CALLBACK  WINAPI SearchFiles(HWND hList, int* count, int MaxNestedFind, bool useSizeBorder, bool useExtension, bool UseNameTemlate,
                    int maxCount, filesFinder f);

HWND hlist, hbutton;
HWND clearDriveButton;
HWND flashDrivesList;
HWND MinSize, MaxSize;
HWND MaxSearchNesting;
HWND sizeChange;
HWND filesList;
HWND  deleteButton;
HWND deleteAllButton;
HWND currentPath;
HWND comboboxExtension, addExtensionButton, deleteExtensionButton;
HWND checkboxSize, checkboxExtension, checkboxName, checkboxCaseSensitiveTeml;
HWND nameTemplate;

flashMonitor* mon;
filesFinder f = filesFinder::filesFinder((wchar_t*)L"D\\");
filesEraser fEraser = filesEraser::filesEraser(true);

wchar_t chosenDrive[10], prevPath[1024];
bool isAnyDriveChoosen = false;
bool addFlag = false;
sortListviewParams sortParameter = { true, 0,filesList};

struct SearchParams
{
    HWND listview;
    int* count;
    int MaxNestFound;
    bool useSizeBorder;
    bool useExtension;
    bool UseNameTemlate;
    int maxCount;
    filesFinder f;
} *LpSearchParams;



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

void CALLBACK  WINAPI SearchFiles(HWND hList, int* count, int MaxNestedFind, bool useSizeBorder, bool useExtension,
                            bool UseNameTemlate, int maxCount, filesFinder f)
{
    f.findFiles(hList, count, MaxNestedFind, useSizeBorder, useExtension, UseNameTemlate, maxCount);
}


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
        if (lParam == 0) 
        {
            return 0;
        }
        UINT id = ((LPNMHDR)lParam)->code;
        switch (id)
        {
            case NM_DBLCLK: 
            {
                wchar_t buf1[1024], buf2[255];
                LPNMITEMACTIVATE item = (LPNMITEMACTIVATE)lParam;
                if (item->iItem == -1)
                    return 0;
                ListView_GetItemText(filesList, item->iItem, 0, buf1, sizeof(buf1));
                ListView_GetItemText(filesList, item->iItem, 1, buf2, sizeof(buf2));
                wcscat_s(buf1,buf2);

                SHELLEXECUTEINFOW sh;
                sh.cbSize = sizeof(SHELLEXECUTEINFOW);
                sh.fMask = SEE_MASK_DEFAULT;
                sh.hwnd = NULL;
                sh.lpVerb = L"open";
                sh.lpFile = buf1;
                sh.lpParameters = NULL;
                sh.lpDirectory = NULL;
                sh.nShow = SW_NORMAL;
                int res = ShellExecuteExW(&sh);
                if (res > 32)
                    return 0;
                switch (res)
                {
                case ERROR_FILE_NOT_FOUND:
                    MessageBox(hWnd,L"Error. File not found!", L"Error", 0);
                    ListView_DeleteItem(filesList,item->iItem);
                    break;
                case SE_ERR_ACCESSDENIED:
                    MessageBox(hWnd, L"Error. Operating system denied access to the specified file!", L"Error", 0);
                    break;
                default:
                    break;
                }
                


                break;
            }
            case LVN_ITEMCHANGED: 
            {
                LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;
                if (pnmv->uChanged &LVIF_STATE) 
                {
                    bool isSelectedNow = ((pnmv->uNewState ^ pnmv->uOldState) & LVIS_SELECTED);
                    bool isSelectedAlready = (pnmv->uNewState & LVIS_SELECTED);
                    bool isChecked = (pnmv->uNewState & LVIS_STATEIMAGEMASK) == INDEXTOSTATEIMAGEMASK(2);
                    bool isUnchecked = (pnmv->uNewState & LVIS_STATEIMAGEMASK) == INDEXTOSTATEIMAGEMASK(1);
                    
                    if (isChecked && !isSelectedAlready)
                    {
                       // MessageBox(NULL, L"CHECKED", L"", 0);
                        ListView_SetItemState(filesList, pnmv->iItem,LVIS_SELECTED, LVIS_SELECTED);
                     }
                    
                    if (isUnchecked && !addFlag)
                    {
                        //MessageBox(NULL, L"UNCHECKED", L"", 0);
                        ListView_SetItemState(filesList, pnmv->iItem, !LVIS_SELECTED, LVIS_SELECTED);
                        
                    }

                }


                break;

                

            }

            case LVN_COLUMNCLICK:
            {
                //MessageBox(NULL,L"", L"",0);
                int count = ListView_GetItemCount(filesList);
                if (count>20000) 
                {
                    MessageBox(NULL, L"Sort is anawailible, when items count is more than 20000", L"Unawailible function", 0);
                    return 0;
                }
                sortParameter.ind = ((NM_LISTVIEW*)lParam)->iSubItem;
                sortParameter.order = !sortParameter.order;
                sortParameter.ListView = filesList;
                ListView_SortItems(filesList, LVCompareProc, (LPARAM)&sortParameter);
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
            //

            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case BUTTON_DELETE_ALL_FILES:
            {
                ListView_SetItemState(filesList, -1, LVIS_SELECTED, LVIS_SELECTED);
                SendMessageW(hWnd, WM_COMMAND, BUTTON_DELETE_FILES,0);
                break;
            }
            case BUTTON_COPY_ALL: 
            {
                ListView_SetItemState(filesList, -1, LVIS_SELECTED, LVIS_SELECTED);
                SendMessageW(hWnd, WM_COMMAND, BUTTON_COPY_TO_SELECTED_FOLDER, 0);
                break;
            }
            case BUTTON_COPY_TO_SELECTED_FOLDER:
            {
                wchar_t path[1024];
                GetWindowTextW(GetDlgItem(hWnd, EDIT_COPY_PATH), path, 1024);
                if (isDirectory(path)) {
                    if (path[wcsnlen_s(path, 1024) - 1] != L'\\')
                        wcscat_s(path, L"\\");
                    f.SetCurrentRoot((wchar_t*)path);
                }
                else
                {
                    OutputAdditionalInfo(L"Wrong copy folder path");
                    return 0;
                }
                bool deleteAfterCopy = getCheckState(GetDlgItem(hWnd,CHECKBOX_DELETE_AFTER_COPY));

                fEraser.CopySelectedFiles(filesList,path,OutputAdditionalInfo, hWnd,deleteAfterCopy);
                
                break;
            }
            case CHECKBOX_USE_MAX_COUNT:
            {
                bool state = getCheckState(GetDlgItem(hWnd, CHECKBOX_USE_MAX_COUNT));
                EnableWindow(GetDlgItem(hWnd, EDIT_MAX_COUNT), state);
                break;
            }
            case CHECKBOX_USE_NAME_TEMPLATE: 
            {
                bool state = getCheckState(checkboxName);
                EnableWindow(nameTemplate, state);
                EnableWindow(checkboxCaseSensitiveTeml, state);
                break;
            }
            case CHECKBOX_CASE_SENSITIVE_TEMPL:
            {
                f.SetTemplateCaseSensitive(getCheckState(checkboxCaseSensitiveTeml));
                break;
            }
            case CHECKBOX_USE_EXTENSION: 

            {
                bool state = getCheckState(checkboxExtension);
                EnableWindow(comboboxExtension,state);
                EnableWindow(addExtensionButton, state);                
                EnableWindow(deleteExtensionButton, state);
                break;
            }
            case CHECKBOX_USE_SIZE_BORDER: 
            {
                bool state = getCheckState(checkboxSize);
                EnableWindow(sizeChange, state);
                EnableWindow(MinSize, state);
                EnableWindow(MaxSize, state);
                break;
            }
            case BUTTON_ADD_EXTENSION:
            {
                EnableWindow(hWnd ,false);
                AddExtensionHandler(wParam,lParam,comboboxExtension,hWnd,hInst);
                EnableWindow(hWnd, true);
                SendMessage(hWnd, WM_ENABLE,true,0);
                break;
            }
            case BUTTON_DELETE_EXTENSION: 
            {
                DeleteExtensionHandler(wParam,lParam,comboboxExtension);
                break;
            }
            case EDIT_CURRENT_PATH: 
            {
                switch (HIWORD(wParam))
                {
                case EN_SETFOCUS:
                {
                    SendMessageW(currentPath, EM_SETREADONLY, false, 0);
                    break;
                }
                case EN_KILLFOCUS:
                {
                    SendMessageW(currentPath, EM_SETREADONLY, true, 0);
                    break;
                }
                default:
                    break;
                }
               
                break;
            }
            case BUTTON_SEARCH_FILES:
            {
                ListView_DeleteAllItems(filesList);

                bool useSize = getCheckState(checkboxSize), useExt = getCheckState(checkboxExtension),
                    useNameTmpl = getCheckState(checkboxName), useMaxCount = getCheckState(GetDlgItem(hWnd, CHECKBOX_USE_MAX_COUNT));

                wchar_t path[1024];
                GetWindowTextW(currentPath, path, 1024);
                if (isDirectory(path)) {
                    if (path[wcsnlen_s(path, 1024) - 1] != L'\\')
                        wcscat_s(path, L"\\");
                    f.SetCurrentRoot((wchar_t*)path);
                }
                else
                {
                    OutputAdditionalInfo(L"Wrong folder path");
                    return 0;
                }
                DWORD min = GetEditData(MinSize), max = GetEditData(MaxSize), nest = GetEditData(MaxSearchNesting);
                if (!CheckMinMax(min, max, nest, OutputAdditionalInfo))
                    return 0;
                MultiplySizes(sizeChange, &min, &max);
                f.SetSizeBorder(min, max);

                if (useExt)
                {
                    wchar_t ext[255];
                    if (!GetExtension(comboboxExtension, ext))
                        return 0;
                    f.SetExtension(ext);
                }
                if (useNameTmpl)
                {
                    wchar_t tmpl[255];
                    GetWindowTextW(nameTemplate, tmpl, 255);
                    f.SetNameTemplate(tmpl);
                }


                int maxCount = -1;
                if (useMaxCount)
                {
                    maxCount = GetEditData(GetDlgItem(hWnd, EDIT_MAX_COUNT));
                    if (maxCount == 0)
                    {
                        OutputAdditionalInfo(L"Wrong max count input. Try again.");
                        return 0;
                    }
                }

                int count = 0;
                addFlag = true;

                
                f.findFiles(filesList,&count,nest,useSize, useExt,useNameTmpl,maxCount);
                addFlag = false;
                wchar_t message[40];
                wsprintfW(message,L"%d files found", count);
                OutputAdditionalInfo(message);
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
                case LBN_DBLCLK: {
                    SetWindowTextW(GetDlgItem(hWnd, EDIT_COPY_PATH), chosenDrive);
                    SetWindowTextW(GetDlgItem(hWnd, EDIT_CURRENT_PATH), prevPath);
                    break;
                }
                              
                case LBN_SELCHANGE: 
                {
                    //MessageBox(hWnd, L"Confirm action", L"Confirm action", MB_ICONQUESTION | MB_YESNO);
                    GetWindowTextW(GetDlgItem(hWnd, EDIT_CURRENT_PATH),prevPath,1024 );
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
                    SetWindowTextW(GetDlgItem(hWnd,EDIT_CURRENT_PATH),chosenDrive);
                    isAnyDriveChoosen = true;

                }
                default:
                    break;
                }
                break;
            }
            case BUTTON_DELETE_FILES: 
            {
                fEraser.DeleteSelectedFiles(filesList, OutputAdditionalInfo);
                break;
            }
            case BUTTON_SET_COPY_PATH: 
            {
                wchar_t path[1024];
                BROWSEINFOW bi = { 0 };
                bi.lpszTitle = L"Title";
                LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
                if (pidl != 0)
                {
                    SHGetPathFromIDListW(pidl, path);
                    IMalloc* im = 0;
                    CoTaskMemFree(pidl);
                    SetWindowTextW(GetDlgItem(hWnd,EDIT_COPY_PATH), path);
                    wcscat_s(path, L" - new  copy path");
                    OutputAdditionalInfo(path);
                }
                else
                {
                    OutputAdditionalInfo(L"Change calseled");
                }
                break;
                break;
            }
            case BUTTON_SET_PATH: 
            {
                wchar_t path[1024];
                BROWSEINFOW bi = { 0 };
                bi.lpszTitle = L"Title";
                LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
                if (pidl !=0) 
                {
                    SHGetPathFromIDListW(pidl, path);
                    IMalloc* im = 0;
                    CoTaskMemFree(pidl);
                    /*if (SUCCEEDED(SHGetMalloc(&im))) 
                    {
                        im->Free(pidl);
                        im->Release();
                    }*/
                   
                    SetWindowTextW(currentPath,path);
                    wcscat_s(path, L" - new path");
                    OutputAdditionalInfo(path);
                }
                else 
                {
                    OutputAdditionalInfo(L"Change calseled");
                }
                break;
            }

            case IDM_ABOUT:
             {
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break; 
            }
            case IDM_EXIT:
            {
                DestroyWindow(hWnd);
                break;
            }   
            default:
            { 
                return DefWindowProc(hWnd, message, wParam, lParam); 
            }
            }
            break;
        }
        
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
            600, 297, 100, 23, hWnd, (HMENU)BUTTON_SEARCH_FILES, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        deleteButton = CreateWindow(WC_BUTTON, L"Delete", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            710, 297, 100, 23, hWnd, (HMENU)BUTTON_DELETE_FILES, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        deleteButton = CreateWindow(WC_BUTTON, L"Delete All", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            820, 297, 100, 23, hWnd, (HMENU)BUTTON_DELETE_ALL_FILES, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        addExtensionButton = CreateWindow(WC_BUTTON, L"+", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            750, 82, 20, 20, hWnd, (HMENU)BUTTON_ADD_EXTENSION, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        deleteExtensionButton =  CreateWindow(WC_BUTTON, L"-", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            775, 82, 20, 20, hWnd, (HMENU)BUTTON_DELETE_EXTENSION, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        CreateWindow(L"BUTTON", L"...", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON ,
            550, 3, 20, 16, hWnd, (HMENU)BUTTON_SET_PATH, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        CreateWindow(L"BUTTON", L"...", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            905, 3, 20, 16, hWnd, (HMENU)BUTTON_SET_COPY_PATH, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        deleteButton = CreateWindow(WC_BUTTON, L"Copy files", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            710, 325, 100, 23, hWnd, (HMENU)BUTTON_COPY_TO_SELECTED_FOLDER, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        deleteButton = CreateWindow(WC_BUTTON, L"Copy all files", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            820, 325, 100, 23, hWnd, (HMENU)BUTTON_COPY_ALL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);


        MinSize = CreateWindow(WC_EDIT  , L"0", WS_TABSTOP | WS_VISIBLE | WS_CHILD|WS_BORDER |ES_NUMBER,
            645, 50, 90, 20, hWnd, (HMENU)EDIT_MIN_SIZE, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        MaxSize = CreateWindow(WC_EDIT     , L"10", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            770, 50, 90, 20, hWnd, (HMENU)EDIT_MAX_SIZE, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        MaxSearchNesting = CreateWindow(WC_EDIT , L"3", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            710, 20, 25, 20, hWnd, (HMENU)EDIT_MAX_SEARCH_NESTING, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        currentPath = CreateWindow(WC_EDIT, L"D:\\", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER|ES_READONLY,
            270, 3, 270, 15, hWnd, (HMENU)EDIT_CURRENT_PATH, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        nameTemplate = CreateWindow(WC_EDIT, L"", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER,
            680, 110, 180, 20, hWnd, (HMENU)EDIT_NAME_TEMPLATE, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        CreateWindow(WC_EDIT, L"100", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER|ES_NUMBER,
            680, 137, 180, 20, hWnd, (HMENU)EDIT_MAX_COUNT, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        CreateWindow(WC_EDIT, L"D:\\temp\\", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY,
            660, 3, 240, 15, hWnd, (HMENU)EDIT_COPY_PATH, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);



        sizeChange = CreateWindow(WC_COMBOBOX   , L"Size", WS_CHILD | WS_VISIBLE | CBS_HASSTRINGS | CBS_DROPDOWNLIST
            | WS_OVERLAPPED | WS_VSCROLL, 870, 49, 60,100 , hWnd, (HMENU)COMBOBOX_SIZE, (HINSTANCE)GetWindowLongA(hWnd, -6), NULL);
        comboboxExtension = CreateWindow(WC_COMBOBOX, L"Extension", WS_CHILD | WS_VISIBLE | CBS_HASSTRINGS | CBS_DROPDOWNLIST
            | WS_OVERLAPPED | WS_VSCROLL, 680, 80, 60, 100, hWnd, (HMENU)COMBOBOX_EXTENSION, (HINSTANCE)GetWindowLongA(hWnd, -6), NULL);


        checkboxSize = CreateWindow(WC_BUTTON, L"Size borders", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX
            ,573, 200, 110, 25, hWnd, (HMENU)CHECKBOX_USE_SIZE_BORDER,(HINSTANCE)GetWindowLongA(hWnd, -6), NULL);
        checkboxExtension = CreateWindow(WC_BUTTON, L"Extension", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX
            , 683, 200, 95, 25, hWnd, (HMENU)CHECKBOX_USE_EXTENSION, (HINSTANCE)GetWindowLongA(hWnd, -6), NULL);
        checkboxName = CreateWindow(WC_BUTTON, L"Name template", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX
            , 778, 200, 120, 25, hWnd, (HMENU)CHECKBOX_USE_NAME_TEMPLATE, (HINSTANCE)GetWindowLongA(hWnd, -6), NULL);
        checkboxCaseSensitiveTeml = CreateWindow(WC_BUTTON, L"Case sensitive", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX
            , 778, 225, 120, 25, hWnd, (HMENU)CHECKBOX_CASE_SENSITIVE_TEMPL, (HINSTANCE)GetWindowLongA(hWnd, -6), NULL);
        CreateWindow(WC_BUTTON, L"Max count", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX
            , 683, 225, 95, 25, hWnd, (HMENU)CHECKBOX_USE_MAX_COUNT, (HINSTANCE)GetWindowLongA(hWnd, -6), NULL);
        CreateWindow(WC_BUTTON, L"Delete files after copying", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX
            , 710, 353, 200, 23 , hWnd, (HMENU)CHECKBOX_DELETE_AFTER_COPY, (HINSTANCE)GetWindowLongA(hWnd, -6), NULL);


        SendMessageW(checkboxSize, BM_SETCHECK, BST_CHECKED, 0);
        SendMessageW(checkboxCaseSensitiveTeml, BM_SETCHECK, BST_CHECKED, 0);
        
        SendMessageW(hWnd, WM_COMMAND,  CHECKBOX_USE_MAX_COUNT, 0);

        SendMessageW(hWnd, WM_COMMAND,CHECKBOX_USE_EXTENSION,0);
        SendMessageW(hWnd, WM_COMMAND, CHECKBOX_USE_NAME_TEMPLATE, 0);
        initSizeCombobox(sizeChange);
        initExtensionCombobox(comboboxExtension);

        AddColumnToLIstview(filesList, L"File path",200);
        AddColumnToLIstview(filesList, L"File name", 100);
        AddColumnToLIstview(filesList, L"File size(bytes)", 100);
    }

    void CALLBACK DrawWindowText(HDC hdc)
    {

        TextOut(hdc, 10, 3, L"Inserted flash drives", 22);
        TextOut(hdc, 170, 3, L"Selected path:", 15);
        TextOut(hdc, 580, 3, L"Copy path:", 11);

        //TextOut(hdc, 280, 3, chosenDrive, wcslen(chosenDrive));
        TextOut(hdc, 573, 22, L"Max search nesting:", 20);
        TextOut(hdc, 573, 52, L"Size from:", 11);
        TextOut(hdc, 745, 52, L"to ", 2);

        TextOut(hdc, 573, 85, L"File extension:", 16);
        TextOut(hdc, 573, 112, L"Name template:", 15);
        TextOut(hdc, 573, 139, L"Max Count:", 11);

        TextOut(hdc, 573, 180, L"Search settings:", 17);
        
    }


    void CALLBACK OutputDrivesInListBox() 
    {
        SendMessage(flashDrivesList, LB_RESETCONTENT, NULL, NULL);
        std::set<wchar_t> res = mon->get_flash_disks(false);
        wchar_t driveTemplate[] = L"!:\\";
        wchar_t driveName[255];
        
        for (auto s :res) 
        {
            driveTemplate[0] = s;
            GetVolumeInformationW(driveTemplate, driveName, 255,NULL, NULL, NULL, NULL,0);
            wsprintfW(driveName,L"%s(%s)",driveName,driveTemplate);
            SendMessage(flashDrivesList, LB_ADDSTRING, NULL, (LPARAM)driveName);
        }
       /* SendMessage(flashDrivesList, LB_ADDSTRING, NULL, (LPARAM)L"FakeDrive1(E:\\)");
        SendMessage(flashDrivesList, LB_ADDSTRING, NULL, (LPARAM)L"FakeDrive2(G:\\)");
        SendMessage(flashDrivesList, LB_ADDSTRING, NULL, (LPARAM)L"vFakeDrive3(K:\\)");*/
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
    

    