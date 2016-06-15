#include "newItem.h"

VirtualItem::VirtualItem() {}

VirtualItem::VirtualItem(char *fileName, int size, int start) { 
	strcpy(itemName, fileName);                                 
	fileOrFolder = _FILE;                                       
	sectorsSize = size;                                        
	startSectors = start;                                    
}

VirtualItem::VirtualItem(char *folderName) {
	strcpy(itemName, folderName);
	fileOrFolder = _FOLDER;
	branches = 0;
}

VirtualItem::~VirtualItem() {
	if (branches > 0) {
		for (int i = 0; i < branches; i++) {
			delete nextFileOrFolder[i];
		}
	}
}

void VirtualItem::writeDataInTreeFile() {
	FILE *vitrualDiskTree;               
	vitrualDiskTree = fopen(DISK_TREE_NAME, "a+b");

	fprintf(vitrualDiskTree, "%s ", itemName);
	fprintf(vitrualDiskTree, "%d ", fileOrFolder);
	if (fileOrFolder == _FOLDER) {
		fprintf(vitrualDiskTree, "%d ", branches);
		fclose(vitrualDiskTree);
		if (branches > 0) {
			for (int i = 0; i < branches; i++) {
				nextFileOrFolder[i]->writeDataInTreeFile();
			}
		}
	}
	else {
		fprintf(vitrualDiskTree, "%d ", sectorsSize);
		fprintf(vitrualDiskTree, "%d ", startSectors);
		fclose(vitrualDiskTree);
	}
}

void VirtualItem::seeDirectoryStruct() { //показывает содержимое папки
	if (branches == 0) {
		cout << endl << "	No files or folders in this directory." << endl << endl;
	}
	else {
		cout << endl << "	NAME" << "			TYPE" << "			SIZE" << endl;
		for (int i = 0; i < branches; i++) {
			cout << endl << "	" << nextFileOrFolder[i]->itemName << "			";
			if (nextFileOrFolder[i]->fileOrFolder == _FILE) {
				cout << "<FILE>			" << nextFileOrFolder[i]->sectorsSize << " bytes";
			}
			else {
				cout << "<FOLDER>";
			}
		}
		cout << endl << endl;
	}
}

void VirtualItem::createNewFile(char *fileName, int sectors, int start) { 
	nextFileOrFolder[branches] = new VirtualItem(fileName, sectors, start);
	branches++;
}

void VirtualItem::createNewFolder(char *folderName) { 
	nextFileOrFolder[branches] = new VirtualItem(folderName);
	branches++;
}

void VirtualItem::defragmentation(int start, int size) { 
	if (fileOrFolder == _FOLDER) {                   
		if (branches > 0) {                     

			for (int i = 0; i < branches; i++) {
				nextFileOrFolder[i]->defragmentation(start, size);
			}
		}
	}
	else {
		if (startSectors > start) {
			startSectors -= size;
		}
	}
}

void VirtualItem::deleteFileOrFolder(char *fileOrFolderName) { 
	bool flag = false;

	if (branches > 0) {
		for (int i = 0; i < branches; i++) {
			if (strcmp(nextFileOrFolder[i]->itemName, fileOrFolderName) == 0) {
				flag = true;
				if (nextFileOrFolder[i]->fileOrFolder == _FOLDER) {
					if (nextFileOrFolder[i]->branches > 0) {
						cout << endl << "	Is not empty!" << endl << endl;
						return;
					}
				}
				else {
					if (nextFileOrFolder[i]->sectorsSize > 0) {
						cout << "	Defragmentation..." << endl << endl;

						char buff;
						FILE *vitrualDisk;
						vitrualDisk = fopen(DISK_NAME, "r+b");
						FILE *tempFile;
						tempFile = fopen("temp", "w+b");
						FILE *tempFile2;
						tempFile2 = fopen("temp2", "w");

						fprintf(tempFile2, "%d ", nextFileOrFolder[i]->sectorsSize);
						fprintf(tempFile2, "%d ", nextFileOrFolder[i]->startSectors);

						for (int j = 0; j < nextFileOrFolder[i]->startSectors; j++) {
							fread(&buff, 1, 1, vitrualDisk);
							fwrite(&buff, 1, 1, tempFile);
						}
						for (int j = 0; j < nextFileOrFolder[i]->sectorsSize; j++) {
							fread(&buff, 1, 1, vitrualDisk);
						}
						while (true) {
							if (fread(&buff, 1, 1, vitrualDisk) == 0) break;
							fwrite(&buff, 1, 1, tempFile);
						}

						fclose(tempFile2);
						fclose(vitrualDisk);
						fclose(tempFile);

						remove(DISK_NAME);
						rename("temp", DISK_NAME);
					}
				}


				VirtualItem *temp = NULL;
				for (int j = i; j < branches - 1; j++) {
					temp = nextFileOrFolder[j];
					nextFileOrFolder[j] = nextFileOrFolder[j + 1];
					nextFileOrFolder[j + 1] = temp;
				}
				delete nextFileOrFolder[branches - 1];
				branches--;
				break;
			}
		}
	}

	if (flag == false) {
		cout << endl << "	Incorrect name!" << endl << endl;
	}
}

VirtualItem *VirtualItem::moveToNextFolder(char *folderName) { 
	bool flag = false;

	if (branches > 0) {
		for (int i = 0; i < branches; i++) {
			if (strcmp(nextFileOrFolder[i]->itemName, folderName) == 0
				&& nextFileOrFolder[i]->fileOrFolder == _FOLDER) {
				return nextFileOrFolder[i];
			}
		}
	}

	if (flag == false) {
		cout << endl << "	Incorrect folder name!" << endl << endl;
	}
	return NULL;
}

char *VirtualItem::getItemName() { 
	return itemName;
}

void VirtualItem::copyFileFromRealSystem(char *fileName) { //
	char buffer;
	int clearSectors = 0;
	int clearBytes = 0;

	FILE *virtualDiskSectorsControl;
	virtualDiskSectorsControl = fopen(DISK_SECTORS_CONTROL, "r");
	fscanf(virtualDiskSectorsControl, "%d", &clearSectors);

	FILE *vitrualDisk;
	vitrualDisk = fopen(DISK_NAME, "r+b");
	fseek(vitrualDisk, 0, SEEK_END);
	long diskSize = ftell(vitrualDisk);
	clearBytes = clearSectors - diskSize;

	FILE *inputFile;
	inputFile = fopen(fileName, "r+b");
	if (!inputFile) {
		cout << endl << "	Incorrect file name!" << endl << endl;
		fclose(virtualDiskSectorsControl);
		fclose(vitrualDisk);
		return;
	}
	fseek(inputFile, 0, SEEK_END);
	long inputFileSize = ftell(inputFile);
	fseek(inputFile, 0, 0);

	if (inputFileSize < clearBytes) {
		while (true) {
			if (fread(&buffer, 1, 1, inputFile) == 0) break;
			fwrite(&buffer, 1, 1, vitrualDisk);
		}
	}
	else {
		cout << endl << "	Out of free space!" << endl << endl;
		fclose(inputFile);
		fclose(virtualDiskSectorsControl);
		fclose(vitrualDisk);
		return;
	}

	nextFileOrFolder[branches] = new VirtualItem(fileName, inputFileSize, diskSize);
	branches++;

	fclose(inputFile);
	fclose(virtualDiskSectorsControl);
	fclose(vitrualDisk);
}

void VirtualItem::copyFileFromVirtualSystem(char *fileName) {
	bool flag = false;
	char buff;

	if (branches > 0) {
		for (int i = 0; i < branches; i++) {
			if (strcmp(nextFileOrFolder[i]->itemName, fileName) == 0
				&& nextFileOrFolder[i]->fileOrFolder == _FILE) {
				flag = true;
				FILE *outputFile;
				FILE *vitrualDisk;
				outputFile = fopen(fileName, "w+b");
				vitrualDisk = fopen(DISK_NAME, "r+b");

				for (int j = 0; j < nextFileOrFolder[i]->startSectors; j++) {
					fread(&buff, 1, 1, vitrualDisk);
				}
				for (int j = 0; j < nextFileOrFolder[i]->sectorsSize; j++) {
					fread(&buff, 1, 1, vitrualDisk);
					fwrite(&buff, 1, 1, outputFile);
				}

				fclose(outputFile);
				fclose(vitrualDisk);
				break;
			}
		}
	}

	if (flag == false) {
		cout << endl << "	Incorrect file name!" << endl << endl;
	}
}
