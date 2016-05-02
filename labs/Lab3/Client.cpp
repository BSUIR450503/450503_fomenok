#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <tchar.h>
#define _CRT_SECURE_NO_WARNINGS
DWORD main(int argc, char *argv[])
{
	printf("I'm client! I'm dispay your massages!\n");
	HANDLE hNamedPipe;	// ������������� ������ Pipe
	DWORD cbWritten;	// ���������� ����, ���������� ����� �����
	DWORD cbRead;	 // ���������� ����, �������� ����� �����
	char szBuf[256]; // ����� ��� �������� ������
	wchar_t szPipeName[256];	 // ����� ��� ����� ������ Pipe

	// ���� ��� ������� PIPEC ���� ������� ��� ������,
	// ��������� ��� � ����� ������ Pipe
	if (argc > 1)
		_stprintf_s(szPipeName, TEXT("\\\\%s\\pipe\\MyPipe"), argv[1]);
	// ���� ��� ������� ������ �� ����, ������� �����
	// � ��������� ���������
	else
		wcscpy_s(szPipeName, TEXT("\\\\.\\pipe\\MyPipe"));

	// ������� ����� � ��������� PIPES
	hNamedPipe = CreateFile(szPipeName,	//����� ������ ����� �����
		GENERIC_READ | GENERIC_WRITE,	//����� �������
		0,	//����� ���������� ������������� �����
		NULL,	//���������� ������
		OPEN_EXISTING,	//��������� ��������
		0,	//�������� �����
		NULL);	//������������� ����� � ����������

	// ���� �������� ������, ������� �� ��� � 
	// ��������� ������ ����������
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
	// ���� ������ ������� � ��������� ���������
	while (1)
	{
		// �������� ������� ������� �� �������
		if (ReadFile(hNamedPipe, szBuf, 512, &cbRead, NULL))
			printf("Received back: <%s>\n", szBuf);

		// ���� ��������� ������, ������� �� ��� �
		// ��������� ������ ����������
		else
		{
			fprintf(stdout, "ReadFile: Error %ld\n", GetLastError());
			_getch();
			break;
		}

		// � ����� �� ������� "exit" ��������� ����
		// ������ ������� � ��������� ���������
		if (!strcmp(szBuf, "exit"))
			break;
	}

	// ��������� ������������� ������
	CloseHandle(hNamedPipe);
	return 0;
}