#pragma once
#include "CtrlsId.h"
#include "resource.h"
#include "cfgmgr32.h"
#include "CommCtrl.h"


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

