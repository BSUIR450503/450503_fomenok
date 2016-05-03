#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <iostream>

#define SIZE_BUFF 512
#define PLATFORM_WIN 1 
#define PLATFORM_UNIX 2 

using namespace std;

#if defined(_WIN32) || defined(_WIN64) 
#include <windows.h> 
#define PLATFORM PLATFORM_WIN 
#else 
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#define PLATFORM PLATFORM_UNIX 
#endif 

struct Data
{
#if PLATFORM == PLATFORM_WIN
	BOOL   fConnected;
	DWORD  cbRead;
	DWORD  cbWritten;

	HANDLE hSem;
	HANDLE hSemClient;
	HANDLE hNamedPipe;

	PROCESS_INFORMATION prInfo;
	STARTUPINFO stInfo;
#else
	int pid;
	int semid;
	struct sembuf buf1, buf;
	char *msg, *segptr;
	int shmid;

	key_t key;
#endif
};

void CloseApplication(struct Data);

bool ReceiveDataChannel(char *, int, struct Data *);
bool SendDataChannel(char *, int, struct Data *);

void InitSettings(struct Data *);
struct Data SetExistsData(struct Data);

struct Data CreateApplication(char *, struct Data);
struct Data Server(struct Data);
struct Data Client(struct Data);