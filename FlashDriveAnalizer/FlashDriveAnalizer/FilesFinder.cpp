#define _CRT_SECURE_NO_WARNINGS
#include "framework.h"
#include "FilesFinder.h"
#include "fileapi.h"
#include "FlashDriveAnalizer.h"




filesFinder::filesFinder(wchar_t* root)
{
	currentroot = new wchar_t[1024];
	wcscpy(currentroot,root);
}
filesFinder::~filesFinder() 
{
	delete currentroot;
}

void filesFinder::SetCurrentRoot(wchar_t* root)
{
	wcscpy(currentroot, root);
}

void filesFinder::findFilesBySize(HWND hList, DWORD minSize, DWORD maxSize, int* count, int MaxNestedFind,int maxCount)
{
	
	
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
		if (data.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY && data.dwFileAttributes != FILE_ATTRIBUTE_HIDDEN)
		//	&& data.dwFileAttributes != FILE_ATTRIBUTE_SYSTEM && data.dwFileAttributes != FILE_ATTRIBUTE_NO_SCRUB_DATA
		//	&& data.dwFileAttributes != FILE_ATTRIBUTE_READONLY)
		{
			
			if (data.nFileSizeLow 
				> minSize && data.nFileSizeLow < maxSize)
			{


				AddFileToListview(hList,currentroot, data.cFileName, data.nFileSizeLow);
				

			}

		}
		else if (data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && data.dwFileAttributes != FILE_ATTRIBUTE_HIDDEN
		 && wcscmp(data.cFileName,L".")!=0 && wcscmp(data.cFileName, L"..") != 0)
		{
			
			wsprintfW(currentroot, L"%s%s\\", currentroot,data.cFileName);
			currentNestLvl++;
			findFilesBySize(hList, minSize,maxSize,count, MaxNestedFind, maxCount);
			currentNestLvl--;
			MoveCurrentRootOneVolumeBack();
		}


	}
	while (FindNextFileW(hfind, &data));
	
	FindClose(hfind);

}


void filesFinder::MoveCurrentRootOneVolumeBack()
{
	int k ;
	currentroot[wcsnlen_s(currentroot, 512) - 1]='\0';
	for (int i = wcsnlen_s(currentroot, 512)-1;i>=1;i--)
	{
		if (currentroot[i] == '\\' )
		{
			currentroot[i+1] = '\0';
	
			break;
		}
		

	}

}





