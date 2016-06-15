#ifdef _WIN32
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <string>

using namespace std;

#endif

#ifdef __linux__
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <dirent.h>
#include <aio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <errno.h> 
#endif

#define READ 0
#define EXIT 1
#define WRITE 2