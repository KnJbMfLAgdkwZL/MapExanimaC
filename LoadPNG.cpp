#include "LoadPNG.h"
#include "printDebug.h"

wchar_t szBuffer[128];

const wchar_t* getMap(BYTE level) {
	if (level < 2)
		return L"assets\\title.png";
	
	GetPrivateProfileString(
		L"Maps", 
		(std::wstring(L"mapid_") + std::to_wstring(level) + std::wstring(L"_location")).c_str(), 
		NULL, 
		szBuffer, 
		128, 
		L".\\assets\\config.ini");	
	return szBuffer;
}

int getMapOffsetX(BYTE level) {
	if (level < 2)
		return 0;

	return GetPrivateProfileInt(
		L"Offsets",
		(std::wstring(L"mapid_") + std::to_wstring(level) + std::wstring(L"_offsetx")).c_str(),
		NULL,
		L".\\assets\\config.ini");	
}
int getMapOffsetY(BYTE level) {
	if (level < 2)
		return 0;

	return GetPrivateProfileInt(
		L"Offsets",
		(std::wstring(L"mapid_") + std::to_wstring(level) + std::wstring(L"_offsety")).c_str(),
		NULL,
		L".\\assets\\config.ini");
}
float getMapScale(BYTE level) {
	return 0.1574f;
}
BOOL isToFullscreen() {	
	return GetPrivateProfileInt(
		L"AppSettings",
		L"full_window_screen",
		NULL,
		L".\\assets\\config.ini") == 1;	
}

std::string getPathExanima() {
	GetPrivateProfileString(
		L"AppSettings ",
		L"pathToExanimaSaves",
		NULL,
		szBuffer,
		128,
		L".\\assets\\config.ini");
	std::wstring ws(szBuffer);
	std::string str(ws.begin(), ws.end());
	return str;
}