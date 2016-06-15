#include "libs.h"

#ifdef _WIN32
#ifdef __cplusplus 
extern "C" {
#endif

	struct OperationInfo
	{
		HANDLE hFile;
		DWORD NumberOfBytes;
		CHAR  buf[100];
		DWORD  pos_out_file;
		OVERLAPPED Overlapped;
	} info;

	__declspec(dllexport) BOOL read(OperationInfo *info)
	{
		BOOL readResult;
		DWORD Num;
		info->Overlapped.Offset = 0; //устанавливаем смящение в 0

		ReadFile(info->hFile, info->buf, info->NumberOfBytes, NULL, &info->Overlapped);// ф-ция чтения их файла 
		WaitForSingleObject(info->Overlapped.hEvent, 1000); // ждем пока событие не перейдем сигнальное состояние(это произойдет тогда, когда вся информация файла будет прочитана )
		readResult = GetOverlappedResult(info->hFile, &info->Overlapped, &Num, FALSE); // получаем результат операции 

		return readResult;
	}
	__declspec(dllexport) BOOL write(OperationInfo *info)
	{
		BOOL writeResult;
		DWORD NumberOfBytesTrasferred;
		info->Overlapped.Offset = info->pos_out_file;

		WriteFile(info->hFile, info->buf, info->Overlapped.InternalHigh, NULL, &info->Overlapped);
		WaitForSingleObject(info->Overlapped.hEvent, 1000);
		writeResult = GetOverlappedResult(info->hFile, &info->Overlapped, &NumberOfBytesTrasferred, FALSE);

		if (writeResult) info->pos_out_file = info->pos_out_file + NumberOfBytesTrasferred;// если упешно записано, устанавливаем курсор файла в конец 
		return writeResult;
	}

#ifdef __cplusplus
}
#endif
#endif

#ifdef __linux__
/*структира для работы с файлом*/
struct FileInfo {
	char readFileName[500];
	char writeFileName[500];
	struct aiocb readCb;
	struct aiocb writeCb;
	char buffer[300];
	int bytesRead;
	int bytesWrite;
};


void asyncronicRead(struct FileInfo *fileInfo) {
	printf("\n\nReading from %s...\n", fileInfo->readFileName);

	int file = open(fileInfo->readFileName, O_RDONLY, 0);// открываем файл для чтения 
	if (file == -1) {// проверка на открытие 
		printf("Can't to open file for read!");
		return;
	}

	/*инц структуру аио*/
	fileInfo->readCb.aio_nbytes = sizeof(fileInfo->buffer); // размер
	fileInfo->readCb.aio_fildes = file; // файл
	fileInfo->readCb.aio_offset = 0; // смящение 
	fileInfo->readCb.aio_buf = fileInfo->buffer; // буфел для считывания 
	 
	aio_read(&fileInfo->readCb); // читаем 
	while (aio_error(&fileInfo->readCb) == EINPROGRESS); // ждем пока будет прочитано 
	fileInfo->bytesRead = aio_return(&fileInfo->readCb); // получаем кол-во считанных байт 
	close(file); // закрвыв 
}

void asyncronicWrite(struct FileInfo *fileInfo) {

	int file = open(fileInfo->writeFileName, O_CREAT/*если не создан содается/ | O_RDWR/*запись*/ | O_APPEND/*запись в конец*/, 0666); // открываем файл 
	if (file == -1) {
		printf("Can't to open file for write!");
		return;
	}

	fileInfo->writeCb.aio_nbytes = fileInfo->bytesRead;// байты
	fileInfo->writeCb.aio_fildes = file;
	fileInfo->writeCb.aio_buf = fileInfo->buffer;

	aio_write(&fileInfo->writeCb); // пишем 
	while (aio_error(&fileInfo->writeCb) == EINPROGRESS);// ждем пока все не запишет 
	fileInfo->bytesWrite = aio_return(&fileInfo->writeCb); // получаем кол-во записанных байт
	close(file); 
}
#endif