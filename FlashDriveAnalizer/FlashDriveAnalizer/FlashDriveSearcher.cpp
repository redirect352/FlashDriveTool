#include "framework.h"
#include "FlashDriveAnalizer.h"
#include "Dbt.h"





void FindAllDrivesInSystem(char** drivesList, int* drivesCount, HWND hlist) 
{
	unsigned int drives = GetLogicalDrives();
	wchar_t driveName[] = L"?:";

	for (int i = 31; i >= 0;i--) 
	{
		if (drives &(1<<i)) 
		{
			driveName[0] = (wchar_t)('A'+i);
			SendMessage(hlist, LB_ADDSTRING, NULL, (LPARAM)driveName);

			DWORD type = GetDriveTypeW(driveName);

			if (type == DRIVE_REMOVABLE) 
			{
				SendMessage(hlist, LB_ADDSTRING, NULL, (LPARAM)L"is flash drive");
			}
		}
	
	}


}



