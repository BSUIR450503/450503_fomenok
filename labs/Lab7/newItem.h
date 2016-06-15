#pragma once
#include "libraries.h"

class VirtualItem {
private:
	char itemName[MAX_NAME_SIZE];
	bool fileOrFolder;
	int sectorsSize;
	int startSectors;
	int branches;
	VirtualItem *nextFileOrFolder[MAX_ITEMS_IN_FOLDER];

public:
	VirtualItem();
	VirtualItem(char*, int, int);
	VirtualItem(char*);
	~VirtualItem();

	void writeDataInTreeFile();
	void seeDirectoryStruct();
	void createNewFile(char*, int, int);
	void createNewFolder(char*);
	void deleteFileOrFolder(char*);
	void copyFileFromRealSystem(char*);
	void copyFileFromVirtualSystem(char*);
	void defragmentation(int, int);
	VirtualItem *moveToNextFolder(char*);
	char *getItemName();
};