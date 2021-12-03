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
void filesFinder::SetSizeBorder(DWORD _minSize, DWORD _maxSize) 
{
	this->minSize = _minSize;
	this->maxSize = _maxSize;
}

void filesFinder::SetExtension(wchar_t* extension) 
{
	wcscpy_s(this->extension,extension);
}



void filesFinder::findFiles(HWND hList, int* count, int MaxNestedFind, bool useSizeBorder, bool useExtension, bool UseNameTemlate,int maxCount)
{
	
	
	if (MaxNestedFind < currentNestLvl) 
	{
		return;
	}
	WIN32_FIND_DATAW data;
	wsprintfW(newPath, L"%s*", currentroot);
	/*if (!useExtension) 
	{
		
	}
	else
	{
		wsprintfW(newPath, L"%s*%s", currentroot,this->extension);
	}*/
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
			
			if ( !useSizeBorder || (data.nFileSizeLow > minSize && data.nFileSizeLow < maxSize))
			{
				if (!useExtension || this->CheckExtension(data.cFileName)) 
				{
					AddFileToListview(hList, currentroot, data.cFileName, data.nFileSizeLow);
					(*count)++;
				}
			}

		}
		else if (data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && data.dwFileAttributes != FILE_ATTRIBUTE_HIDDEN
		 && wcscmp(data.cFileName,L".")!=0 && wcscmp(data.cFileName, L"..") != 0)
		{
			
			wsprintfW(currentroot, L"%s%s\\", currentroot,data.cFileName);
			currentNestLvl++;
			findFiles(hList,count, MaxNestedFind, useSizeBorder,useExtension,UseNameTemlate,maxCount);
			currentNestLvl--;
			MoveCurrentRootOneVolumeBack();
		}


	}
	while (FindNextFileW(hfind, &data));
	FindClose(hfind);

}


bool filesFinder::CheckExtension(wchar_t* fileName) 
{
	bool res = true;
	for (int i = wcslen(fileName) - 1, j = wcslen(extension)-1; i >= 0 && j>=0;i--,j--) 
	{
		if (fileName[i] != extension[j])
		{
			return false;
		}
	}
	return res;
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





