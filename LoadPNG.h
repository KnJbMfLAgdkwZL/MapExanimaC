#include <iostream>
#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")


const wchar_t* getMap(BYTE);
int getMapOffsetX(BYTE);
int getMapOffsetY(BYTE);
float getMapScale(BYTE);