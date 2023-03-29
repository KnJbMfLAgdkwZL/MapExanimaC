#include "printDebug.h"
using namespace std;

void OutputIntToHex(UINT x) {
    static const char* digits = "0123456789ABCDEF";
    size_t hex_len = sizeof(x) << 1;
    string rc(hex_len, '0');
    for (size_t i = 0, j = (hex_len - 1) * 4; i < hex_len; ++i, j -= 4)
        rc[i] = digits[(x >> j) & 0x0f];

    OutputDebugStringA(LPCSTR(&rc[0]));
    OutputDebugStringA(LPCSTR("\n"));
}
void debug_w(const char s[]) {
    OutputDebugStringA(LPCSTR(s));
    OutputDebugStringA(LPCSTR("\n"));
}
void debug_w(const wchar_t* s) {
    OutputDebugStringW(LPCWSTR(s));
    OutputDebugStringW(LPCWSTR(L"\n"));
}
void debug_w(const string s) {

    OutputDebugStringA(LPCSTR(s.c_str()));
    OutputDebugStringA(LPCSTR("\n"));
}
void debug_w(const DWORD s) {
    std::ostringstream out;
    out << s;
    OutputDebugStringA(LPCSTR(out.str().c_str()));
    OutputDebugStringA(LPCSTR("\n"));
}
void debug_w(const float s) {
    std::ostringstream out;
    out << s;
    OutputDebugStringA(LPCSTR(out.str().c_str()));
    OutputDebugStringA(LPCSTR("\n"));
}
void debug_w(const unsigned long long s) {
    std::ostringstream out;
    out << s;
    OutputDebugStringA(LPCSTR(out.str().c_str()));
    OutputDebugStringA(LPCSTR("\n"));
}
void debug_w(const int s) {
    std::ostringstream out;
    out << s;
    OutputDebugStringA(LPCSTR(out.str().c_str()));
    OutputDebugStringA(LPCSTR("\n"));
}
