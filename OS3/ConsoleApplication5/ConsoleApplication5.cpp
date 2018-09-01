// ConsoleApplication5.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <windows.h>

using namespace std;
int main(int argc, _TCHAR* argv[])
{
    HANDLE file_handle;
    SECURITY_ATTRIBUTES secAtrib;
    secAtrib.nLength = sizeof(secAtrib);
    secAtrib.lpSecurityDescriptor = NULL;
    secAtrib.bInheritHandle = TRUE;
    file_handle = CreateFile(L"F:\\stackoverflow\\stackoverflow.txt",
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        &secAtrib,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
        
        SECURITY_ATTRIBUTES secAtrib1;
    	secAtrib1.nLength = sizeof(secAtrib);
    	secAtrib1.lpSecurityDescriptor = NULL;
    	secAtrib1.bInheritHandle = TRUE;
        HANDLE tmp = CreateFile(L"F:\\stackoverflow\\tmp5.txt",
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        &secAtrib1,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
        char* buf1 = (char*)malloc(100000 * sizeof(char));
        DWORD bytes_readed;
        DWORD bytes_written;
        SetFilePointer(file_handle, 0, NULL, FILE_BEGIN);
        int n=0;
        while(n<100){
    	ReadFile(file_handle, buf1, 100000, &bytes_readed, NULL);
    	WriteFile(tmp, buf1, 100000, &bytes_written, NULL);
        n++;
        }
        return 0;
}

