#include "utils.h"

std::wstring NarrowToWide(const std::string& s) {
            return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>("").from_bytes(s);
}
DWORD GetProcessID(wchar_t* process_name)
{
    PROCESSENTRY32 processInfo;
            processInfo.dwSize = sizeof(processInfo);

            HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
            if ( processesSnapshot == INVALID_HANDLE_VALUE )
                return 0;

            Process32First(processesSnapshot, &processInfo);
            if ( !wcscmp(process_name,processInfo.szExeFile) )
            {
                CloseHandle(processesSnapshot);
                return processInfo.th32ProcessID;
            }

            while ( Process32Next(processesSnapshot, &processInfo) )
            {
                if ( !wcscmp(process_name,processInfo.szExeFile) )
                {
                    CloseHandle(processesSnapshot);
                    return processInfo.th32ProcessID;
                }
            }

            CloseHandle(processesSnapshot);
}
