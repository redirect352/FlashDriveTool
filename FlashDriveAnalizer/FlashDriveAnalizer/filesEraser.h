#pragma once
#include "framework.h"
#include "CommCtrl.h"


//typedef void CALLBACK Message(const wchar_t*);

class filesEraser 
{
	public:
		filesEraser(bool deleteFromListView );
		int CALLBACK DeleteSelectedFiles(HWND Listview,Message ms);
		int GetSuccesfullyDeleted();
		int GetFailedToDelete();
		void SetDeleteItemsFlag( bool flag);
		int CALLBACK CopySelectedFiles(HWND Listview, wchar_t*copyPath,Message ms, HWND parentWindow,bool deleteCopiedfiles = false);
		

	private:
		bool deleteItems = true;
		int successFullyDeleted = 0;
		int failedToDelete = 0;
		wchar_t buf1[1024] = L"", buf2[1024] = L"";
 

};

