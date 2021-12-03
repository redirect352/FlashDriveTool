#pragma once
#include "framework.h"
#include "resource.h"
#define BUTTON_OK_DLG_INPUT_EXTENSION 1
#define BUTTON_CANCEL_DLG_INPUT_EXTENSION 2
#define EDIT_EXTENSION_DLG_INPUT_EXTENSION 3




void DeleteExtensionHandler(WPARAM wParam, LPARAM lParam, HWND combobox);
void AddExtensionHandler(WPARAM wParam, LPARAM lParam, HWND combobox, HWND parent, HINSTANCE hInstance);
bool getCheckState(HWND ckb1);



