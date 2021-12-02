#include "filesEraser.h"



filesEraser::filesEraser(bool deleteFromListView) 
{
	this->deleteItems = deleteFromListView;
}

int filesEraser::GetFailedToDelete() 
{
	return this->failedToDelete;
}

int filesEraser::GetSuccesfullyDeleted()
{
	return this->successFullyDeleted;
}
void filesEraser::SetDeleteItemsFlag(bool flag)
{
	this->deleteItems = flag;
}
int CALLBACK filesEraser::DeleteSelectedFiles(HWND Listview, Message ms)
{
	this->failedToDelete = 0;
	this->successFullyDeleted = 0;

	int ind = ListView_GetNextItem(Listview,-1, LVNI_SELECTED);
	
	while (ind !=-1) 
	{
		ListView_GetItemText(Listview, ind, 0, buf1, sizeof(buf1));
		ListView_GetItemText(Listview, ind, 1, buf2, sizeof(buf2));
		wcscat_s(buf1, buf2);

		//If file was suuccessfully deleted
		if (true)
		{
			
			//MessageBox(NULL,buf1,L"Delete",0);
			ms(buf1);
		    ListView_DeleteItem(Listview, ind);
			ind--;
			this->successFullyDeleted++;
		}
		else 
		{
			this->failedToDelete++;
		}
		ind = ListView_GetNextItem(Listview, ind, LVNI_SELECTED);
	}


	return this->successFullyDeleted;
}