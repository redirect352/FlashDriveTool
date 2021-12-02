#define _CRT_SECURE_NO_WARNINGS
#include "framework.h"
#include "FlashDriveAnalizer.h"
#include "Dbt.h"
#include "sstream"
#include "string"
#include "cstring"



void CALLBACK AddFileToListview(HWND listview, const  wchar_t* col1, const  wchar_t* col2, DWORD col3, int ind)
{
	static int i;
	LVITEM lvI;

	std::stringstream s;
	s << col3;
	const wchar_t* bufpath3 = GetWC(s.str().c_str());

	lvI.pszText = (wchar_t*)col1;
	lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvI.stateMask = 0;
	lvI.iSubItem = 0;
	lvI.state = 0;
	lvI.iItem = 0;
	lvI.iImage = 0;
	lvI.iGroupId = 0;
	lvI.lParam = i;
	lvI.cchTextMax = wcslen(col1)+1;
	i++;
	ListView_InsertItem(listview, &lvI);
	ListView_SetItemText(listview, 0, 1, (wchar_t*)col2);
	ListView_SetItemText(listview, 0, 2, (wchar_t*)bufpath3);
	delete bufpath3;

}

void CALLBACK AddColumnToLIstview(HWND Listview, const wchar_t* colName,int cx, int ind) 
{
	LV_COLUMN lv;
	memset(&lv, 0, sizeof(lv));
	lv.pszText = (wchar_t*)colName;
	lv.cx = cx;
	lv.cxMin = cx/2;
	lv.fmt = LVCFMT_LEFT;
	lv.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT|LVCF_SUBITEM;
	ListView_InsertColumn(Listview, ind, &lv);
}

const wchar_t* GetWC(const char*src) 
{
	const size_t size = strlen(src) + 1;
	wchar_t* wc = new wchar_t[size];
	mbstowcs(wc,src,size);
	return wc;
}


int CALLBACK  LVCompareProc(LPARAM p1, LPARAM p2, LPARAM p)
{
	HWND filesList = ((sortListviewParams*)p)->ListView;
	wchar_t buf1[1024],
		buf2[1024];
	LPWSTR lpStr1, lpStr2;
	LVFINDINFO ItemInfo;
	ItemInfo.flags = LVFI_PARAM;
	ItemInfo.lParam = p1;
	int ind = ListView_FindItem(filesList, -1, &ItemInfo);
	ListView_GetItemText(filesList, ind, (int)((sortListviewParams*)p)->ind, buf1, sizeof(buf1));
	lpStr1 = buf1;

	ItemInfo.lParam = p2;
	ind = ListView_FindItem(filesList, -1, &ItemInfo);
	ListView_GetItemText(filesList, ind, (int)((sortListviewParams*)p)->ind, buf2, sizeof(buf2));
	lpStr2 = buf2;
	if (lpStr1 && lpStr2)
	{
		int res = wcscmp(lpStr1, lpStr2);
		if (((sortListviewParams*)p)->order)
			return res;
		else
		{
			return -res;
		}
	}


	return 0;

}

