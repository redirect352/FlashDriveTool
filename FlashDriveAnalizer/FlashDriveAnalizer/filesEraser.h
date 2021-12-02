#pragma once
#include "framework.h"
#include "CommCtrl.h"


typedef void CALLBACK Message(const wchar_t*);

class filesEraser 
{
	public:
		filesEraser(bool deleteFromListView );
		int CALLBACK DeleteSelectedFiles(HWND Listview,Message ms);
		int GetSuccesfullyDeleted();
		int GetFailedToDelete();
		void SetDeleteItemsFlag( bool flag);
		

	private:
		bool deleteItems = true;
		int successFullyDeleted;
		int failedToDelete;
		wchar_t buf1[1024], buf2[1024];
};

