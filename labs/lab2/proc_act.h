#pragma once

#include "processes.h"

void print(int numberOfProcess)
{
	for (int i = 0; i < strlen(processes[numberOfProcess - 1]); i++)
	{
		cout << processes[numberOfProcess - 1][i];
		Sleep(LONG_SLEEP);
	}

	return;
}

#ifdef __linux__

int printFlag = 0;
int endFlag = 1;

struct sigaction toPrintSignal;
struct sigaction toEndSignal;

void setPrint(int sig) {
	printFlag = 1;
}

void setEnd(int sig) {
	endFlag = 1;
}

void tempSleep(int slpTime)
{
	usleep(slpTime);
	fflush(stdout);
	refresh();
}

#endif

#ifdef _WIN32

class Process
{
private:
	PROCESS_INFORMATION processInfo;
	HANDLE printEvent;
	HANDLE closeEvent;
public:
	PROCESS_INFORMATION createNewProcess(TCHAR*, int);
	HANDLE getPrintEvent();
	HANDLE getCloseEvent();
};

PROCESS_INFORMATION Process::createNewProcess(TCHAR*path, int processNumber)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	char procNum[RADIX];
	char toClose[RADIX];
	char command[RADIX] = "-n ";

	_itoa(processNumber, procNum, RADIX);
	strcat(command, procNum);

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (!CreateProcess(path, command, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi))
	{
		cout << "CreateProcess failed. Error code: " << (int)GetLastError() << endl;
		exit((int)GetLastError());
	}

	strcpy(toClose, procNum);
	strcat(toClose, procNum);

	// closeEvent auto-resets
	closeEvent = CreateEvent(NULL, FALSE, FALSE, toClose);
	// printEvent resets by user
	printEvent = CreateEvent(NULL, TRUE, FALSE, procNum);

	return pi;
}

HANDLE Process::getPrintEvent()
{
	return printEvent;
}

HANDLE Process::getCloseEvent()
{
	return closeEvent;
}

int getch_noblock()
{
	if (_kbhit())
		return _getch();
	else
		return -1;
}

#endif