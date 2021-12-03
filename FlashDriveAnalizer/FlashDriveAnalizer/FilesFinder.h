#pragma once
#include "resource.h"
#include <memory>
#include <stdexcept>
#include <set>
#include <map>
#include <string>
#include <Windows.h>
#include <cfgmgr32.h>


class filesFinder {

	public:
		filesFinder(wchar_t *root);
		~filesFinder();
		void findFiles(HWND hList,int* count, int maxNestedFind ,bool useSizeBorder, bool useExtension, bool UseNameTemlate,int maxCount = -1);
		void findBiggestFiles(HWND hList, bool isBiggest, int count = 10);
		
		
		void SetCurrentRoot(wchar_t* root);
		void SetExtension(wchar_t* extension);
		void SetNameTemplate(wchar_t* _template);
		void SetSizeBorder(DWORD _minSize, DWORD _maxSize);
	private:
		void MoveCurrentRootOneVolumeBack();
		bool CheckExtension(wchar_t* fileName);
		bool CheckNameTemplate(wchar_t* fileName);
		wchar_t *currentroot;
		wchar_t extension[255] = L"";
		wchar_t  nameTemplate[255] = L"";
		DWORD minSize =0, maxSize=0;
		
		int currentNestLvl = 0;
		int currentFilesCount = 0;
		wchar_t buf[1024] = L"", newPath[1024] = L"";
		
	

};