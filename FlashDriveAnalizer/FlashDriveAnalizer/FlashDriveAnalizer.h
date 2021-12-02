#pragma once

#include "resource.h"
#include "cfgmgr32.h"
#include "CommCtrl.h"




#define  BUTTON_CLEAR_DRIVE 11
#define  BUTTON_SEARCH_FILES 19
#define  BUTTON_DELETE_FILES 20
#define  BUTTON_DELETE_ALL_FILES 21

#define  LISTBOX_FLASHDRIVE 12

#define  EDIT_MAX_SIZE 14
#define  EDIT_MIN_SIZE 15
#define  EDIT_MAX_SEARCH_NESTING 18
#define  EDIT_CURRENT_PATH 22


#define  COMBOBOX_SIZE 16
#define  LISTVIEW_FILES 17



void device_added(char letter);
void device_removed(char letter);
bool device_safe_removed(char letter);
void device_remove_failed(char letter);
void CALLBACK DrawWindowText(HDC hdc);
void CALLBACK OutputDrivesInListBox();
void CALLBACK OutputAdditionalInfo(const wchar_t* info);
void CALLBACK AddFileToListview(HWND listview, const  wchar_t* col1, const  wchar_t* col2, DWORD col3, int ind = 1000000);
void CALLBACK AddColumnToLIstview(HWND Listview, const wchar_t* colName, int cx,int ind=10000);
int CALLBACK LVCompareProc(LPARAM p1, LPARAM p2, LPARAM p);
const wchar_t* GetWC(const char* src);

struct sortListviewParams
{
	bool order;
	int ind;
	HWND ListView;
};

