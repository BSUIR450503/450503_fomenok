#include "libs.h"

#ifdef _WIN32

struct OperationInfo
{
	HANDLE hFile;
	DWORD NumberOfBytes;
	CHAR  buf[100];
	DWORD  pos_out_file;
	OVERLAPPED Overlapped;
} info;

HINSTANCE library;
HANDLE events[3];


DWORD WINAPI ReaderThread(PVOID folderPATH)
{
	string folder(((const char*)folderPATH));// change way to file
	folder.append("\\"); 
	string fileMask = folder + "in?.txt"; 
	char ReadFilePATH[MAX_PATH]; //write way of file

	BOOL(*Read)(OperationInfo*) = (BOOL(*)(OperationInfo*))GetProcAddress(library, "read");// get adr fun into libr

	
	WIN32_FIND_DATA FindFileData; // search file of folder, return description folder
	HANDLE find_Handle,
		hReadFile;

	BOOL readResult = false;

	find_Handle = FindFirstFile(fileMask.c_str(), &FindFileData);// search folder

	if (find_Handle == INVALID_HANDLE_VALUE)// the success check
	{
		printf(" Error: %d\n", GetLastError());
		return 0;
	}

	while (1)
	{
		WaitForSingleObject(events[WRITE], INFINITE);// wait WRITE go to signal state
		strcpy_s(ReadFilePATH, folder.c_str()); // copy way of file
		strcat_s(ReadFilePATH, FindFileData.cFileName); // add name of file
		hReadFile = CreateFile(ReadFilePATH, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL); // open file

		info.hFile = hReadFile; 
		printf("Reading from %s...\n", ReadFilePATH);
		readResult = (Read)(&info); //read file

		if (readResult) //check to successful
			SetEvent(events[READ]); // if successful READ stay in signal state
		else
			printf("Error while read from %s!\n", ReadFilePATH); 
		if (FindNextFile(find_Handle, &FindFileData)) // search next file 
		{
			CloseHandle(hReadFile); // close disk file 
			continue;
		}
		else break;
	}
	FindClose(find_Handle); 
	CloseHandle(hReadFile); 
	SetEvent(events[EXIT]); //  stay Event in signal state
	return 0;
}

DWORD WINAPI WriterThread(PVOID outFilePath)
{
	string file(((const char*)outFilePath));
	file.append("\\out.txt");
	HANDLE hOutputFile = CreateFile(file.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
	HANDLE forwait[2] = { events[READ], events[EXIT] };//struct with 2 sob
	BOOL(*Write)(OperationInfo*) = (BOOL(*)(OperationInfo*))GetProcAddress(library, "write"); // get adr fun
	while (1)
	{
		int event = WaitForMultipleObjects(2, forwait, FALSE, INFINITE) - WAIT_OBJECT_0;// wait sob in signal state 
		if (event == EXIT) 
			break;
		info.hFile = hOutputFile; // write HANDLE in str
		printf("Writing to %s...\n", file.c_str()); 
		(Write)(&info); 
		SetEvent(events[WRITE]); // signal state
	}
	CloseHandle(hOutputFile); 
	return 0;
}

int main(int argc, char *argv[])
{

	HANDLE hEvent;
	HANDLE hThreads[2];
	char path[MAX_PATH]; // way of file

	hEvent = CreateEvent(NULL, FALSE, TRUE, TEXT("event"));

	
	events[WRITE] = CreateEvent(NULL, FALSE, TRUE, NULL);// signal state, avt
	events[READ] = CreateEvent(NULL, FALSE, FALSE, NULL);// avt
	events[EXIT] = CreateEvent(NULL, TRUE, FALSE, NULL); // avt


	
	info.Overlapped.Offset = 0;
	info.Overlapped.OffsetHigh = 0;
	info.Overlapped.hEvent = hEvent;
	info.pos_out_file = 0; // set kyrs file in begin 
	info.NumberOfBytes = sizeof(info.buf); // buf 

	library = LoadLibrary("lib.dll");


	printf("Enter path to dir: ");
	gets_s(path);


	hThreads[0] = CreateThread(NULL, 0, ReaderThread, (LPVOID)path, 0, NULL);
	hThreads[1] = CreateThread(NULL, 0, WriterThread, (LPVOID)path, 0, NULL);

	// wait Thread end
	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);


	CloseHandle(hThreads[0]);
	CloseHandle(hThreads[1]);

	CloseHandle(events[WRITE]);
	CloseHandle(events[READ]);
	CloseHandle(events[EXIT]);
	CloseHandle(hEvent);


	FreeLibrary(library);
	printf("\n\n");
	system("pause");
	return 0;
}
#endif

#ifdef __linux__
struct FileInfo {
	char readFileName[500];
	char writeFileName[500];
	struct aiocb readCb;
	struct aiocb writeCb;
	char buffer[300];
	int bytesRead;
	int bytesWrite;
};

char fileNames[20][100];
struct FileInfo fileInfo;
pthread_t readThread;
pthread_t writeThread;
int numberOfFiles = 0;
pthread_mutex_t mutex;

void(*asyncronicWrite)(struct FileInfo *fileInfo);
void(*asyncronicRead)(struct FileInfo *fileInfo);

void findAllFiles() {
	struct dirent *dp;// search file into folder 
	DIR *dirp; // search folder 
	dirp = opendir("/home/thandor/Test"); // open folder 
	puts("Input files in directory: "); 
	while ((dp = readdir(dirp)) != NULL) { 
		if (strstr(dp->d_name, "in") != NULL) { // see file necessary mask
			strcpy(fileNames[numberOfFiles], dp->d_name); // write name into mas 
			numberOfFiles++;
			puts(dp->d_name); 
		}
	}
}

void *readFunc(void * arg){
	void *ext_library;
	ext_library = dlopen("/home/thandor/lib.so", RTLD_LAZY); 
	asyncronicRead = (void(*)(struct FileInfo *fileInfo)) dlsym(ext_library, "asyncronicRead"); // get adr of fun
	for (int i = 0; i < numberOfFiles; i++) { // for read file 
		pthread_mutex_lock(&mutex); 
		strcpy(fileInfo.readFileName, fileNames[i]); // copy name into struct 
		(*asyncronicRead)(&fileInfo); // fun read 
		pthread_mutex_unlock(&mutex); // unlock 
		puts("Wait for write...");
		usleep(100000);
	}
	dlclose(ext_library); 
	puts("\nDone!");
	return NULL;
}

void *writeFunc(void * arg) {
	usleep(10000);
	strcpy(fileInfo.writeFileName, "/home/thandor/Test/out.txt"); // copy name of file
	void *ext_library; 
	ext_library = dlopen("/home/thandor/lib.so", RTLD_LAZY);
	asyncronicWrite = (void(*)(struct FileInfo *fileInfo)) dlsym(ext_library, "asyncronicWrite"); // get adr of fun
	for (int i = 0; i < numberOfFiles; i++) { // for write
		pthread_mutex_lock(&mutex); 
		puts("Writing...");
		(*asyncronicWrite)(&fileInfo); // write
		pthread_mutex_unlock(&mutex); // unlock
		puts("Wait for read...");
		usleep(100000);
	}
	dlclose(ext_library);
	return NULL;
}

int main() {
	if (pthread_mutex_init(&mutex, NULL)) // 
	{
		printf("Can't create mutex");
		return 0;
	}
	remove("/home/thandor/Test/out.txt");// delete file

	findAllFiles();//

	pthread_create(&readThread, NULL, readFunc, NULL); 
	pthread_create(&writeThread, NULL, writeFunc, NULL);
	pthread_join(readThread, NULL);
	pthread_join(writeThread, NULL);

	return 0;
}
#endif