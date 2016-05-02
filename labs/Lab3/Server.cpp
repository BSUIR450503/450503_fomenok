#include <stdio.h>
#include <conio.h>
#include <windows.h>
#define _CRT_SECURE_NO_WARNINGS

#ifdef _WIN32
int main()
{
	printf("I'm server! I'm working!\n I'm waiting your messages!\n");


	HANDLE hNamedPipe;	// ������������� ������ Pipe
	TCHAR lpszPipeName[] = TEXT("\\\\.\\pipe\\MyPipe");	 // ��� ������������ ������ Pipe
	char szBuf[512];	// ����� ��� �������� ������ ����� �����
	DWORD cbWritten;	// ���������� ���� ������, ���������� ����� �����

	// �������� ������������ �������
	hNamedPipe = CreateNamedPipe(lpszPipeName,	//����� ������ ����� ������
		PIPE_ACCESS_DUPLEX,	//����� ��������  ������
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,	//����� ������ ������
		PIPE_UNLIMITED_INSTANCES,	//����������� � ���������� ����������  ������
		512, 512,	//������ �������� � ��������� ������� � ������
		5000,	//����� �������� � ������������
		NULL);	//����� ���������� ��� ��������� ������

	// ���� �������� ������, ������� �� ��� � ���������
	// ������ ����������
	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		fprintf(stdout, "CreateNamedPipe: Error %ld\n", GetLastError());
		_getch();
		return 0;
	}

	TCHAR CommandLine[] = TEXT("C:\\Users\\vikyf_000\\Documents\\visual studio 2013\\Projects\\spovm\\Debug\\spovm.exe");
	STARTUPINFO si;
	PROCESS_INFORMATION piApp;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(CommandLine, NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &piApp))
		printf("Process isn't working\n");


	ConnectNamedPipe(hNamedPipe, NULL);

	// ���� ��������� ������ ����� �����
	while (1)
	{
		printf("Message:");
		gets_s(szBuf);

		// �������� ��������� ������ 
		if (!WriteFile(hNamedPipe, szBuf, strlen(szBuf) + 1, &cbWritten, NULL))
			break;

		// ���� ������ ������� "exit", 
		// ��������� ������ ����������
		if (!strcmp(szBuf, "exit"))
			break;
	}
	WaitForSingleObject(piApp.hProcess, INFINITE);
	CloseHandle(piApp.hThread);
	CloseHandle(piApp.hProcess);
	CloseHandle(hNamedPipe);
	_getch();
	return 0;
}


#elif linux

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include "sys/wait.h"
#include <signal.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
using namespace std;
int main()
{
	int fd_read[2], fd_write[2];
	pipe(fd_read);
	pipe(fd_write);
	pid_t clientPid;
	clientPid = fork();
	char str_write[512];
	char str_read[512];
	do
	{
		if (clientPid > 0)
		{
			int len;
			if ((len = read(fd_read[0], str_read, 512)) != 0)
				cout << "Ret: " << str_read << "\n";
			if ((len = read(fd_read[0], str_read, 512)) != 0)
				cout << "Ret: " << str_read << "\n";
			write(fd_write[1], "See!", 6);
		}
		else
		{
			write(fd_read[1], "Go!", 5);
			int ret;
			cout << "Mes: ";
			fflush(stdin);
			cin >> str_write;
			write(fd_read[1], str_write, 512);
			if ((ret = read(fd_write[0], str_read, 512)) != 0)
				cout << "Mes: " << str_read << "\n";
		}
	} while (strcmp(str_write, "exit"));
	kill(clientPid, SIGKILL);
	return(0);
}
#endif


