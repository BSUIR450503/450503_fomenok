#pragma once

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_COUNT 10
#define RADIX 10

void print(int);

using std::cout;
using std::endl;

static const TCHAR * processes[] = { "1. First process\r\n", "2. Second process\r\n",
"3. Third process\r\n", "4. Fourth process\r\n",
"5. Fifth process\r\n", "6. Sixth process\r\n",
"7. Seventh process\r\n", "8. Eighth process\r\n",
"9. Ninth process\r\n", "10. Tenth process\r\n" };

#ifdef _WIN32

#include <Windows.h>
#include <conio.h>

#define _CRT_SECURE_NO_WARNINGS
#define SHORT_SLEEP 1
#define LONG_SLEEP 50
#define HANDLE_MACRO Process

int getch_noblock();

#elif __linux__

#include <curses.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void tempSleep(int);

#define Sleep tempSleep
#define getch_noblock getchar
#define SHORT_SLEEP 1000
#define LONG_SLEEP 50000
#define HANDLE_MACRO pid_t

#endif