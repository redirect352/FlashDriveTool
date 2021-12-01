#include "framework.h"
#include "FilesFinder.h"
#include "fileapi.h"
#include "FlashDriveAnalizer.h"
#include "sstream"



filesFinder::filesFinder(wchar_t* root)
{
	currentroot = root;
}
filesFinder::~filesFinder() 
{
	
}

void filesFinder::SetCurrentRoot(wchar_t* root)
{
	currentroot = root;
}

void filesFinder::findFilesBySize(HWND hList, DWORD minSize, DWORD maxSize, int* count, int MaxNestedFind,int maxCount)
{
	wchar_t buf[512], newPath[512];
	currentNestLvl++;
	if (MaxNestedFind < currentNestLvl) 
	{
		return;
	}
	WIN32_FIND_DATAW data;

	wsprintfW(newPath,L"%s*",currentroot);
	HANDLE hfind = FindFirstFileW(newPath, &data);
	if (hfind == INVALID_HANDLE_VALUE) 
	{
		MessageBox(NULL, L"Invalid path", L"", 0);
		return;

	}
	
	do
	{
		if (data.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY && data.dwFileAttributes != FILE_ATTRIBUTE_HIDDEN
			&& data.dwFileAttributes != FILE_ATTRIBUTE_SYSTEM && data.dwFileAttributes != FILE_ATTRIBUTE_NO_SCRUB_DATA
			&& data.dwFileAttributes != FILE_ATTRIBUTE_READONLY)
		{
			
			if (data.nFileSizeLow 
				> minSize && data.nFileSizeLow < maxSize)
			{
				std::stringstream s;
				s << data.nFileSizeLow;

				AddFileToListview(hList,currentroot, data.cFileName,GetWC(s.str().c_str()));
				

			}

		}
		else if (data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && data.dwFileAttributes != FILE_ATTRIBUTE_HIDDEN
			&& data.dwFileAttributes != FILE_ATTRIBUTE_SYSTEM && data.dwFileAttributes != FILE_ATTRIBUTE_NO_SCRUB_DATA
			&& data.dwFileAttributes != FILE_ATTRIBUTE_READONLY)
		{
			wsprintfW(newPath, L"%s%s\\", currentroot,data.cFileName);
			currentroot = newPath;
			findFilesBySize(hList, minSize,maxSize,count, MaxNestedFind, maxCount);
			MoveCurrentRootOneVolumeBack();
		}


	}
	while (FindNextFileW(hfind, &data));
	
	//MoveCurrentRootOneVolumeBack();
	currentNestLvl--;
}


void filesFinder::MoveCurrentRootOneVolumeBack()
{
	int k = 1;
	for (int i = wcsnlen_s(currentroot, 256)-1;i>=1;i--)
	{
		if (currentroot[i] == '\\' && k == 1)
		{
			currentroot[i] = 0;
			k++;
		}
		if (currentroot[i-1] == '\\' && k == 2) 
		{
			currentroot[i] = 0;
			break;
		}

	}

}





