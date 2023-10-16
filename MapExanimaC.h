#pragma once

#include "resource.h"
#include <sstream>

#define IDM_MY_MSG_UPDATE_UI (WM_APP + 4)
#define _USE_MATH_DEFINES

#define HIDEMAP		 0x0001
#define CORNERMAP	 0x0010
#define FULLMAP		 0x0100
#define FULLTRANSMAP 0x1000


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

int vpointerExanimaRX = GetPrivateProfileInt(
	L"MemoryAddresses",
	L"rotationx_ptr",
	NULL,
	L".\\assets\\config.ini");

#define ADDR_X_ROTATE ((LPVOID)(vpointerExanimaRX))

int vpointerExanimaRY = GetPrivateProfileInt(
	L"MemoryAddresses",
	L"rotationy_ptr",
	NULL,
	L".\\assets\\config.ini");

#define ADDR_Y_ROTATE ((LPVOID)(vpointerExanimaRY))
