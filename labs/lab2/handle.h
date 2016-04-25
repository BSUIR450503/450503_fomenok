#pragma once

#include<TCHAR.H>
#include "proc_act.h"
#include "processes.h"

class LabHandle
{
public:
	void startLab(int, TCHAR**);
};

void LabHandle::startLab(int argNum, TCHAR **value)
{
	if (argNum == 2)
	{
		// only Windows' block, to access child process
#ifdef _WIN32
		char toClose[RADIX];
		// close-event name is made by concatenation of 2 the same names of print-event (it's number of our process)
		// so if it's the first process, print-event will have name "1" and close-event will have "11"
		strcpy(toClose, value[1]);
		strcat(toClose, value[1]);

		// open events using the names of events
		HANDLE canPrintEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, value[1]);
		HANDLE canCloseEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, toClose);

		while (1)
		{
			if (WaitForSingleObject(canPrintEvent, 1) == WAIT_OBJECT_0)
			{
				print(atoi(value[1]));
				ResetEvent(canPrintEvent);
			}
			if (WaitForSingleObject(canCloseEvent, 0) == WAIT_OBJECT_0)
			{
				CloseHandle(canCloseEvent);
				CloseHandle(canPrintEvent);
				return;
			}
		}
#endif
	}
	else
	{
		char key;
		int numOfProcess = 0;
		bool flag = false;
		int currentNum = 1;
		// array of processes (okay, sounds not very correctly, but it is)
		HANDLE_MACRO process[MAX_COUNT];

#ifdef _WIN32

		while (1)
		{
			key = getch_noblock();
			Sleep(SHORT_SLEEP);
			switch (key)
			{
			case '+':
			{
				if (numOfProcess < MAX_COUNT)
				{
					numOfProcess++;
					process[numOfProcess - 1].createNewProcess(value[0], numOfProcess);
				}
				break;
			}
			case '-':
			{
				if (numOfProcess > 0)
				{
					SetEvent(process[--numOfProcess].getCloseEvent());
				}
				break;
			}
			case 'q':
			{
				if (numOfProcess > 0)
				{
					for (int i = 0; i < numOfProcess; i++)
					{
						SetEvent(process[i].getCloseEvent());
					}
				}
				return;
			}
			}

			// if there are processes and we got the timeout of print-event signal
			if (numOfProcess > 0 && WaitForSingleObject(process[currentNum - 1].getPrintEvent(), 0) == WAIT_TIMEOUT)
			{
				// if current process number is equal to or bigger than number of existing processes
				if (currentNum >= numOfProcess)
				{
					// set current number to "1"
					currentNum = 1;
				}
				else
				{
					if (!flag)
					{
						currentNum++;
					}
				}
				flag = false;
				// send signal to print event
				SetEvent(process[currentNum - 1].getPrintEvent());
			}
		}

#elif __linux__

		initscr();
		clear();
		noecho();
		refresh();

		toPrintSignal.sa_handler = setPrint;
		sigaction(SIGUSR1, &toPrintSignal, NULL);

		toEndSignal.sa_handler = setEnd;
		sigaction(SIGUSR2, &toEndSignal, NULL);

		while (1)
		{
			key = getch_noblock();

			Sleep(SHORT_SLEEP);
			switch (key)
			{
			case '+':
			{
				if (numOfProcess < MAX_COUNT)
				{
					process[numOfProcess] = fork();
					numOfProcess++;

					switch (process[numOfProcess - 1])
					{
					case 0:
					{
						endFlag = 0;
						while (!endFlag)
						{
							Sleep(SHORT_SLEEP);
							if (printFlag)
							{
								print(numOfProcess);
								refresh();
								printFlag = 0;
								kill(getppid(), SIGUSR2);
							}
						}
						return;
						break;
					}
					case -1:
					{
						cout << "CreateProcess failed!" << endl;
						break;
					}
					default:
					{
						break;
					}
					}
				}
				break;
			}

			case '-':
			{
				if (numOfProcess > 0)
				{
					kill(process[--numOfProcess], SIGUSR2);
					waitpid(process[numOfProcess], NULL, 0);
					kill(process[numOfProcess], SIGKILL);
					waitpid(process[numOfProcess], NULL, 0);
				}
				break;
			}

			case 'q':
			{
				if (numOfProcess > 0)
				{
					for (; numOfProcess > 0; numOfProcess--)
					{
						kill(process[numOfProcess - 1], SIGUSR2);
						waitpid(process[numOfProcess - 1], NULL, 0);
						kill(process[numOfProcess - 1], SIGKILL);
						waitpid(process[numOfProcess - 1], NULL, 0);
					}
				}
				clear();
				endwin();
				return;
			}
			}

			// the same to windows 
			if (endFlag && numOfProcess > 0)
			{
				endFlag = 0;
				if (currentNum >= numOfProcess)
				{
					currentNum = 1;
				}
				else
				{
					if (!flag)
					{
						currentNum++;
					}
				}
				flag = false;
				kill(process[currentNum - 1], SIGUSR1);
			}
			refresh();
		}

#endif

	}
}