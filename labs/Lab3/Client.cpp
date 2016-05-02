#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <tchar.h>
#define _CRT_SECURE_NO_WARNINGS
DWORD main(int argc, char *argv[])
{
	printf("I'm client! I'm dispay your massages!\n");
	HANDLE hNamedPipe;	// Идентификатор канала Pipe
	DWORD cbWritten;	// Количество байт, переданных через канал
	DWORD cbRead;	 // Количество байт, принятых через канал
	char szBuf[256]; // Буфер для передачи данных
	wchar_t szPipeName[256];	 // Буфер для имени канала Pipe

	// Если при запуске PIPEC было указано имя срвера,
	// указываем его в имени канала Pipe
	if (argc > 1)
		_stprintf_s(szPipeName, TEXT("\\\\%s\\pipe\\MyPipe"), argv[1]);
	// Если имя сервера задано не было, создаем канал
	// с локальным процессом
	else
		wcscpy_s(szPipeName, TEXT("\\\\.\\pipe\\MyPipe"));

	// Создаем канал с процессом PIPES
	hNamedPipe = CreateFile(szPipeName,	//адрес строки имени файла
		GENERIC_READ | GENERIC_WRITE,	//режим доступа
		0,	//режим совмесного использования файла
		NULL,	//дескриптор защиты
		OPEN_EXISTING,	//параметры создания
		0,	//атрибуты файла
		NULL);	//идентификатор файла с атрибутами

	// Если возникла ошибка, выводим ее код и 
	// завершаем работу приложения
	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		fprintf(stdout, "CreateFile: Error %ld\n", GetLastError());
		_getch();
		return 0;
	}
	else
	{
		printf("Server if ready!\n");
	}
	// Цикл обмена данными с серверным процессом
	while (1)
	{
		// Получаем команду обратно от сервера
		if (ReadFile(hNamedPipe, szBuf, 512, &cbRead, NULL))
			printf("Received back: <%s>\n", szBuf);

		// Если произошла ошибка, выводим ее код и
		// завершаем работу приложения
		else
		{
			fprintf(stdout, "ReadFile: Error %ld\n", GetLastError());
			_getch();
			break;
		}

		// В ответ на команду "exit" завершаем цикл
		// обмена данными с серверным процессом
		if (!strcmp(szBuf, "exit"))
			break;
	}

	// Закрываем идентификатор канала
	CloseHandle(hNamedPipe);
	return 0;
}