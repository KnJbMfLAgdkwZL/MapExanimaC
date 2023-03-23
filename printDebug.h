#pragma once


#include <stdlib.h>
#include <sstream>
#include <windows.h>

void OutputIntToHex(UINT x);
void debug_w(const char s[]);
void debug_w(const std::string s);
void debug_w(const DWORD s);
void debug_w(const float s);