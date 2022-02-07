#ifndef UTILS_H
#define UTILS_H
#include <Windows.h>
#include <TlHelp32.h>
#include <codecvt>

DWORD GetProcessID(wchar_t* process_name);
std::wstring NarrowToWide(const std::string& s);

#endif // UTILS_H
