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
		void findFilesBySize(HWND hList,DWORD minSize, DWORD maxSize,int* count, int maxNestedFind ,int maxCount = -1);
		void findFilesByExtension(HWND hList,wchar_t* extension, int maxCount = -1);
		void findBiggestFiles(HWND hList, bool isBiggest, int count = 10);
		void findByName(HWND hList, wchar_t* name, int maxCount = 10);

		void deleteFiles(wchar_t** fileNames, int count);
		
		void SetCurrentRoot(wchar_t* root);

	private:
		wchar_t *currentroot;
		int currentNestLvl = 0;
		int currentFilesCount = 0;
		wchar_t buf[1024], newPath[1024];
		void MoveCurrentRootOneVolumeBack();

};