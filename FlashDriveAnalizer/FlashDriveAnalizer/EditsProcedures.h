#pragma once
#include "resource.h"
#include "CommCtrl.h"



DWORD CALLBACK GetEditData(HWND edit);

bool CheckMinMax(DWORD min, DWORD max, DWORD nest, Message ms);

void MultiplySizes(HWND sizeCB, DWORD* min, DWORD* max);

bool GetExtension(HWND extCB, const wchar_t *ext);