#include "Header.h"
#ifdef _WIN32
#define FNAME "ConsoleApplication1.exe"
#include <fstream>
#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string>
#include "windows.h"
#include <iostream>
#include <atlconv.h>

using namespace std;
int main(int argc, char * argv[])
{

	string name, parametr;
	int result;
	name = FNAME;
	cout << "Enter the side of the square = ";
	getline(cin, parametr); 
	Process pr(name, parametr, 1, result);
	cout << "Area = " << result << endl;
	system("pause");
	return 0;
}


#elif linux

#include <stdio.h>
#include<stdlib.h>
#include<errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main()
{
int result;
pid_t pid;
int a=4;
switch(pid=fork()) {
case -1:
perror("fork");
exit(1);
case 0:
result*=result;
printf("%d",result);
exit(result);
default:

wait(&a);
WEXITSTATUS(a);

}
}
#endif

















