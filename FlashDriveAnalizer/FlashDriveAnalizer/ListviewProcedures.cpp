#define _CRT_SECURE_NO_WARNINGS
#include "framework.h"
#include "FlashDriveAnalizer.h"
#include "Dbt.h"

void CALLBACK AddFileToListview(HWND listview, const  wchar_t* col1, const  wchar_t* col2, const  wchar_t* col3, int ind)
{
	
	
	LVITEM lvI;
	lvI.pszText = (wchar_t*)col1; 
	lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvI.stateMask = 0;
	lvI.iSubItem = 0;
	lvI.state = 0;
	lvI.iItem = ind;
	lvI.iImage = 0;
	lvI.iGroupId = 0;
	ListView_InsertItem(listview, &lvI);
	ListView_SetItemText(listview, 0, 1, (wchar_t*)col2);
	ListView_SetItemText(listview, 0, 2, (wchar_t*)col3);

}

void CALLBACK AddColumnToLIstview(HWND Listview, const wchar_t* colName,int cx, int ind) 
{
	LV_COLUMN lv;
	memset(&lv, 0, sizeof(lv));
	lv.pszText = (wchar_t*)colName;
	lv.cx = cx;
	lv.cxMin = cx/2;
	lv.fmt = LVCFMT_LEFT;
	lv.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	ListView_InsertColumn(Listview, ind, &lv);
}

const wchar_t* GetWC(const char*src) 
{
	const size_t size = strlen(src) + 1;
	wchar_t* wc = new wchar_t[size];
	mbstowcs(wc,src,size);
	return wc;
}

