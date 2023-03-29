#pragma once

#include "resource.h"
#include <sstream>

#define IDM_MY_MSG_UPDATE_UI (WM_APP + 4)



int vpointerExanimaLVL = GetPrivateProfileInt(
	L"MemoryAddresses",
	L"offset_lvl_ptr ",
	NULL,
	L".\\assets\\config.ini");
#define ADDR_MAP_LVL ((LPVOID)vpointerExanimaLVL)
int vpointerExanimaX = GetPrivateProfileInt(
	L"MemoryAddresses",
	L"offset_x_ptr  ",
	NULL,
	L".\\assets\\config.ini");

#define ADDR_X_POS ((LPVOID)vpointerExanimaX)

int vpointerExanimaY = GetPrivateProfileInt(
	L"MemoryAddresses",
	L"offset_y_ptr",
	NULL,
	L".\\assets\\config.ini");

#define ADDR_Y_POS ((LPVOID)(vpointerExanimaY))
