#include <iostream>
#include <stdio.h>
using namespace std;

#define PLATFORM_WINDOWS 1
#define PLATFORM_UNIX 2

#if defined(_WIN32)
#define PLATFORM PLATFORM_WINDOWS
#else 
#define PLATFORM PLATFORM_UNIX
#endif

bool semaforeIsOpen = false;
bool openSemafore();
#if PLATFORM == PLATFORM_WINDOWS
#include <windows.h>
CRITICAL_SECTION CS;  //доступ к общему ресурсу 
#else
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include "sys/wait.h"
pthread_mutex_t m;
#endif

struct inform
{
#if PLATFORM == PLATFORM_UNIX
	pthread_t id;
#else 
	HANDLE newHandle;
#endif
};

void clear_buffer();
inform createChild();
void delProcess(inform);
void endProgram(inform &);
int maxCount = 0;

int main(int argc, char* argv[])
{
#if PLATFORM == PLATFORM_WINDOWS
	InitializeCriticalSection(&CS);
#else
	if (pthread_mutex_init(&m, NULL) != 0)
	{
		printf("Error\n");
		return 0;
	}
#endif
	inform  information[50];
	bool checkChoise = true;
	do{
		cout << "Program is working! Please, enter your comand!\n'+' - add\n'-' - del\n'q' - exit\nYou have " << maxCount << " child thread" << endl;
		char choise = getchar();
		clear_buffer();

		switch (choise)
		{
		case '+':
			information[maxCount] = createChild();
			maxCount++;
			break;
		case '-':
			if (maxCount>0)
			{
				endProgram(information[maxCount - 1]);
				maxCount--;
			}

			break;
		case 'q':
			if (maxCount != 0)
			{
				for (int k = maxCount - 1; k >= 0; k--)
					endProgram(information[k]);
			}
			getchar();
#if PLATFORM == PLATFORM_WINDOWS
			DeleteCriticalSection(&CS);
#else
#endif
			return 0;
		default:
			cout << "Enter somthing unknown! Please, try again!" << endl << endl;
		}
	} while (checkChoise);
}

void clear_buffer()
{
	int c;
	do {
		c = getchar();
	} while (c != '\n' && c != EOF);
}

#if PLATFORM ==PLATFORM_WINDOWS
void ThreadFunction(DWORD lpParam)
{
	while (1)
	{
		int id = GetCurrentThreadId();
		EnterCriticalSection(&CS);
		cout << "ID of thread " << id << endl;
		LeaveCriticalSection(&CS);
		Sleep(500);
	}
}
#else 
void *ThreadFunction(void* args)
{
	pthread_t id = pthread_self();
	pthread_mutex_lock(&m);
	while (1)
	{
		cout << "ID of thread " << id << endl;
		usleep(1000000);
		pthread_mutex_unlock(&m);

	}


	return NULL;
}
#endif

inform createChild()
{
	inform information;
#if PLATFORM == PLATFORM_WINDOWS
	DWORD dwId;
	information.newHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunction, 0, 0, &dwId);
#else 
	pthread_create(&information.id, NULL, ThreadFunction, NULL);
#endif
	return information;
}

void endProgram(inform& information)
{
#if PLATFORM == PLATFORM_WINDOWS
	DWORD dwExitCode;
	ZeroMemory(&dwExitCode, sizeof(DWORD));
	TerminateThread(information.newHandle, dwExitCode);
#else
	pthread_cancel(information.id);
#endif
}