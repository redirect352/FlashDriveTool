#pragma once

#include "resource.h"
#include "cfgmgr32.h"
#define  BUTTON_CLEAR_DRIVE 11
#define  LISTBOX_FLASHDRIVE 12

void FindAllDrivesInSystem(char** drivesList, int* drivesCount, HWND hlist);
void device_added(char letter);
void device_removed(char letter);
bool device_safe_removed(char letter);
void device_remove_failed(char letter);
void CALLBACK DrawWindowText(HDC hdc);
void CALLBACK OutputDrivesInListBox();
void CALLBACK OutputAdditionalInfo(wchar_t* info);