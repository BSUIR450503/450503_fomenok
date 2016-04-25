#ifdef _WIN32
#define use_CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "windows.h"
#include <iostream>
#include <atlconv.h>

using namespace std;
class Process
{
public:
	PROCESS_INFORMATION p;
	Process(string name, string parametr, bool waitfor, int & status)
	{

		parametr = name + ' ' + parametr;

		STARTUPINFO a;
		ZeroMemory(&a, sizeof(a));
		a.cb = sizeof(a);
		ZeroMemory(&p, sizeof(p));

		wchar_t * t = new wchar_t[100]; 
		mbstowcs(t, parametr.c_str(), 100); 

		if (!CreateProcess(NULL, t, NULL, NULL, FALSE, 0, NULL, NULL, &a, &p))
			cout << "don't work" << endl;
		if (waitfor)
			WaitForSingleObject(p.hProcess, INFINITE);
		DWORD code;
		GetExitCodeProcess(p.hProcess, &code);
		status = code;
	}

	~Process()
	{
		CloseHandle(p.hProcess);
	}

};
#endif

