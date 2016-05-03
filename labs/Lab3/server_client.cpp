#include "Head.h"

#if PLATFORM == PLATFORM_WIN

struct Data CreateApplication(char *path, struct Data data)
{
	char *str = (char*)malloc(sizeof(char) * 100);
	strcpy(str, path);
	strcat(str, " server");

	wchar_t wtext[200];
	mbstowcs(wtext, str, strlen(str) + 1);
	LPWSTR ptr = wtext;

	ZeroMemory(&(data.stInfo), sizeof(STARTUPINFO));
	data.stInfo.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(NULL, ptr, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &(data.stInfo), &(data.prInfo)))
	{
		printf("Process: Error %ld\n", GetLastError());
		exit(0);
	}

	strcpy(str, path);
	strcat(str, " client");

	mbstowcs(wtext, str, strlen(str) + 1);//Plus null 
	ptr = wtext;

	Sleep(1000);

	if (!CreateProcess(NULL, ptr, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &(data.stInfo), &(data.prInfo)))
	{
		printf("Process: Error %ld\n", GetLastError());
		exit(0);
	}

	free(str);

	return data;
}

struct Data Server(struct Data data)
{
	printf("Server: Waiting for connect...\n");

	data.fConnected = ConnectNamedPipe(data.hNamedPipe, NULL);

	if (!(data.fConnected))
	{
		CloseHandle(data.hNamedPipe);
		exit(0);
	}

	WaitForSingleObject(data.hSem, INFINITE);

	printf("Server: Connected...\n");

	char *buffer = NULL;
	buffer = (char *)malloc(sizeof(char) * SIZE_BUFF);

	while (1)
	{
		if (!SendDataChannel("Ready for typing command...", 28, &data))
			break;

		//ReleaseSemaphore(data.hSem, -1, NULL);
		ReleaseSemaphore(data.hSemClient, 1, NULL);

		WaitForSingleObject(data.hSem, INFINITE);
		printf("\ncmd server > ");

		fflush(stdin);
		gets(buffer);

		if (!SendDataChannel(buffer, strlen(buffer) + 1, &data))
			break;

		//	ReleaseSemaphore(data.hSem, -1, NULL);
		ReleaseSemaphore(data.hSemClient, 1, NULL);

		if (!strcmp(buffer, "exit"))
		{
			CloseApplication(data);
			free(buffer);

			_getch();
			return data;
		}

		WaitForSingleObject(data.hSem, INFINITE);

		if (ReceiveDataChannel(*&buffer, SIZE_BUFF, &data))
			printf("Client: %s\n", buffer);

		if (!strcmp(buffer, "exit"))
		{
			CloseApplication(data);
			free(buffer);

			_getch();
			return data;
		}
	}

	return data;
}

struct Data Client(struct Data data)
{
	printf("\nClient: Connected. Type 'exit' to terminate\n");

	ReleaseSemaphore(data.hSem, 1, NULL);

	char *buffer = NULL;
	buffer = (char *)malloc(sizeof(char) * SIZE_BUFF);

	while (1)
	{
		WaitForSingleObject(data.hSemClient, INFINITE);

		if (ReceiveDataChannel(*&buffer, SIZE_BUFF, &data))
		{
			printf("Server: %s\n", buffer);

			//ReleaseSemaphore(data.hSemClient, -1, NULL);
			ReleaseSemaphore(data.hSem, 1, NULL);
		}

		WaitForSingleObject(data.hSemClient, INFINITE);

		if (ReceiveDataChannel(*&buffer, SIZE_BUFF, &data))
		{
			if (!strcmp(buffer, "exit"))
			{
				CloseHandle(data.hSemClient);
				free(buffer);

				return data;
			}

			char input[SIZE_BUFF] = { '\0' };

			printf("Client: %s\n", buffer);

			cout << "cmd client > ";

			fflush(stdin);
			cin.getline(input, SIZE_BUFF);

			if (!SendDataChannel(input, strlen(input) + 1, &data))
				break;

			if (!strcmp(input, "exit"))
			{
				ReleaseSemaphore(data.hSem, 1, NULL);

				CloseHandle(data.hSemClient);
				free(buffer);

				return data;
			}
		}

		//ReleaseSemaphore(data.hSemClient, -1, NULL);
		ReleaseSemaphore(data.hSem, 1, NULL);
	}

	return data;
}

struct Data SetExistsData(struct Data data)
{
	data.hSem = OpenSemaphore(EVENT_ALL_ACCESS, FALSE, TEXT("MySemaphore"));
	data.hSemClient = OpenSemaphore(EVENT_ALL_ACCESS, FALSE, TEXT("MySemaphoreClient"));

	data.hNamedPipe = CreateFile(TEXT("\\\\.\\pipe\\mypipe"), GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, 0, NULL);

	if (data.hNamedPipe == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile: Error %ld\n", GetLastError());
		exit(0);
	}

	return data;
}

void InitSettings(struct Data *data)
{
	data->hNamedPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\mypipe"), PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES, SIZE_BUFF, SIZE_BUFF, NMPWAIT_USE_DEFAULT_WAIT, NULL);

	if (data->hNamedPipe == INVALID_HANDLE_VALUE)
	{
		printf("CreateNamedPipe1: Error %ld\n", GetLastError());

		system("pause");
		exit(1);
	}

	data->hSem = CreateSemaphore(NULL, 0, 1, TEXT("MySemaphore"));
	data->hSemClient = CreateSemaphore(NULL, 0, 1, TEXT("MySemaphoreClient"));
}

void CloseApplication(struct Data data)
{
	CloseHandle(data.hNamedPipe);
	CloseHandle(data.hSem);
}

bool ReceiveDataChannel(char *message, int size, struct Data *data)
{
	if (ReadFile(data->hNamedPipe, message, SIZE_BUFF, &(data->cbRead), NULL))
		return true;

	return false;
}

bool SendDataChannel(char *message, int size, struct Data *data)
{
	if (!WriteFile(data->hNamedPipe, message, size, &(data->cbWritten), NULL))
		return false;

	return true;
}

#else

struct Data CreateApplication(char *path, struct Data data)
{
	switch (data.pid = fork())
	{
	case -1:
		printf("error %d\n", data.pid);
		break;
	case 0:
		execlp("xterm", "xterm", "-e", path, "client", NULL);
		break;
	default:
		execlp("xterm", "xterm", "-e", path, "server", NULL);
	}

	return data;
}

struct Data Server(struct Data data)
{
	data.buf.sem_num = 0;
	data.buf.sem_op = 0;
	data.buf.sem_flg = 0;
	printf("Server: Ready for typing command");
	while (1) {
		char *message = NULL;
		message = (char *)malloc(SIZE_BUFF * sizeof(char));

		if (semop(data.semid, &(data.buf), 1) < 0)
			perror("semop");

		printf("\ncmd[server] > ");
		fflush(stdin);
		gets(message);

		SendDataChannel(message, strlen(message) + 1, &data);

		data.buf.sem_op = 3;
		semop(data.semid, &(data.buf), 1);

		data.buf.sem_op = 0;
		semop(data.semid, &(data.buf), 1);

		if (!strcmp("exit", message))
			return data;

		ReceiveDataChannel(*&message, SIZE_BUFF, &data);
		printf("Client: %s\n", message);

		data.buf.sem_op = 3;
		semop(data.semid, &(data.buf), 1);

		SendDataChannel("Ready for typing command...", 28, &data);

		data.buf.sem_op = 0;
	}
	return data;
}


struct Data Client(struct Data data)
{

	while (1) {
		char *message = NULL;
		message = (char *)malloc(SIZE_BUFF * sizeof(char));

		semop(data.semid, &(data.buf1), 1);

		ReceiveDataChannel(*&message, SIZE_BUFF, &data);

		if (!strcmp("exit", message))
		{
			data.buf1.sem_op = -1;
			semop(data.semid, &(data.buf1), 1);

			return data;
		}

		printf("Client: %s\n", message);

		printf("\ncmd[client] > ");

		fflush(stdin);
		gets(message);

		SendDataChannel(message, strlen(message) + 1, &data);

		data.buf1.sem_op = -1;
		semop(data.semid, &(data.buf1), 1);

		data.buf1.sem_op = -2;
		semop(data.semid, &(data.buf1), 1);

		ReceiveDataChannel(*&message, SIZE_BUFF, &data);

		printf("Server: %s\n", message);

		data.buf1.sem_op = -1;
		semop(data.semid, &(data.buf1), 1);

		data.buf1.sem_op = -2;
	}

	return data;
}

bool SendDataChannel(char *message, int size, struct Data *data)
{
	if (strcpy(data->segptr, message))
		return true;
	else
		return false;
}

bool ReceiveDataChannel(char *message, int size, struct Data *data)
{
	if (strcpy(message, data->segptr))
		return true;
	else
		return false;
}

struct Data SetExistsData(struct Data data)
{
	data.buf1.sem_op = -1;
	semop(data.semid, &(data.buf1), 1);
	data.buf1.sem_op = -2;

	if ((data.shmid = shmget(data.key, SIZE_BUFF, 0)) == -1)
	{
		perror("shmget");
		exit(1);
	}

	return data;
}

void InitSettings(struct Data *data)
{
	data->key = ftok("/home/romaresh/C++/3_lab", 0);

	if ((data->shmid = shmget(data.key, SIZE_BUFF, IPC_CREAT | IPC_EXCL | 0660)) == -1)
	{
		if ((data->shmid = shmget(data->key, SIZE_BUFF, 0)) == -1)
		{
			perror("shmget");
			exit(1);
		}
	}

	if ((data->segptr = (char *)shmat(data->shmid, NULL, 0)) == (char *)(-1)){
		printf("Can't attach shared memory\n");
		exit(1);
	}


	data->semid = semget(data->key, 1, 0666 | IPC_CREAT);

	if (data->semid < 0)
	{
		perror("semget");
		exit(EXIT_FAILURE);
	}

	if (semctl(data->semid, 0, SETVAL, (int)0) < 0)
	{
		perror("semctl");
		exit(EXIT_FAILURE);
	}

	data->buf1.sem_num = 0;
	data->buf1.sem_op = 1;
	data->buf1.sem_flg = 0;

	semop(data->semid, &(data->buf1), 1);
}

void CloseApplication(struct Data data)
{
	semctl(data.semid, 0, IPC_RMID, 1);
}


#endif