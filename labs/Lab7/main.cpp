#include "libraries.h"
#include "newItem.h"

void createVirtualDisk() { 
	FILE *vitrualDisk;
	FILE *virtualDiskSectorsControl; 
	int diskSize = 0;
	int flag = 0;
	char buff = '0';

	cout << "Create Disk" << endl;
	while (diskSize <= 0 || diskSize > MAX_DISK_SIZE) {
		cout << endl << "	Enter disk size : ";
		cin >> diskSize;
		if (diskSize <= 0 || diskSize > MAX_DISK_SIZE) {
			cout << "Error!";
		}
	}
	cout << endl;

	vitrualDisk = fopen(DISK_NAME, "w");
	virtualDiskSectorsControl = fopen(DISK_SECTORS_CONTROL, "w");
	fprintf(virtualDiskSectorsControl, "%d", diskSize);

	VirtualItem root("root");
	root.writeDataInTreeFile();

	fclose(vitrualDisk);
	fclose(virtualDiskSectorsControl);
}

void loadFolderFromFile(FILE *vitrualDiskTree, VirtualItem **current, int depth) { //воссоздает древовидную структуру
	int buffForBranches = 0;                                                      
	int buffForSectorsSize = 0;                                               
	int buffForFileOrFolder = 0;
	int buffForStartSector = 0;

	if (depth == 0) {
		while (!feof(vitrualDiskTree)) {
			char *buffForItemName = new char[MAX_NAME_SIZE];
			fscanf(vitrualDiskTree, "%s ", buffForItemName); 
				fscanf(vitrualDiskTree, "%d ", &buffForFileOrFolder);
			if (buffForFileOrFolder == _FOLDER) {
				current[depth]->createNewFolder(buffForItemName);
				fscanf(vitrualDiskTree, "%d ", &buffForBranches);
				if (buffForBranches > 0) {
					depth++;
					current[depth] = current[depth - 1]->moveToNextFolder(buffForItemName);
					for (int i = 0; i < buffForBranches; i++) {
						loadFolderFromFile(vitrualDiskTree, current, depth);
					}
					current[depth] = NULL;
					depth--;
				}
			}
			else {
				fscanf(vitrualDiskTree, "%d ", &buffForSectorsSize);
				fscanf(vitrualDiskTree, "%d ", &buffForStartSector);
				current[depth]->createNewFile(buffForItemName, buffForSectorsSize, buffForStartSector);
			}
			delete[] buffForItemName;
		}
	}
	else {
		char *buffForItemName = new char[MAX_NAME_SIZE];
		fscanf(vitrualDiskTree, "%s ", buffForItemName);
		fscanf(vitrualDiskTree, "%d ", &buffForFileOrFolder);
		if (buffForFileOrFolder == _FOLDER) {
			current[depth]->createNewFolder(buffForItemName);
			fscanf(vitrualDiskTree, "%d ", &buffForBranches);
			if (buffForBranches > 0) {
				depth++;
				current[depth] = current[depth - 1]->moveToNextFolder(buffForItemName);
				for (int i = 0; i < buffForBranches; i++) {
					loadFolderFromFile(vitrualDiskTree, current, depth);
				}
				current[depth] = NULL;
				depth--;
			}
		}
		else {
			fscanf(vitrualDiskTree, "%d ", &buffForSectorsSize);
			fscanf(vitrualDiskTree, "%d ", &buffForStartSector);
			current[depth]->createNewFile(buffForItemName, buffForSectorsSize, buffForStartSector);
		}
		delete[] buffForItemName;
	}
}

void loadDataFormFile(VirtualItem **current) { //открытие vitrualDiskTree и чтение инф о корневом каталоге
	int depth = 0;                         
	FILE *vitrualDiskTree;
	vitrualDiskTree = fopen(DISK_TREE_NAME, "r");

	char *buffForRootItemName = new char[MAX_NAME_SIZE];
	int buffForFileOrFolder = 0;
	int buffForRootBranches = 0;

	fscanf(vitrualDiskTree, "%s ", buffForRootItemName);
	fscanf(vitrualDiskTree, "%d ", &buffForFileOrFolder);
	fscanf(vitrualDiskTree, "%d ", &buffForRootBranches);

	loadFolderFromFile(vitrualDiskTree, current, depth);

	delete[] buffForRootItemName;
	fclose(vitrualDiskTree);
}

void help() { 
	cout << endl << "struct         -- show directory structure";
	cout << endl << "cd 'name'      -- move to next folder";
	cout << endl << "fd             -- move to previous folder";
	cout << endl << "newfl 'name'   -- create new file in this directory";
	cout << endl << "newfr 'name'   -- create new folder in this directory";
	cout << endl << "del 'name      -- delete file or folder'";
	cout << endl << "push 'name'    -- copy file from real system to virtual disk";
	cout << endl << "pop 'name'     -- copy file from virtual dist to real system";
	cout << endl << "exit           -- close programm" << endl << endl;
}

void openVirtualDisk() { //открытие вирт. диска если он есть и
	bool flag = false;  //обработка команд пользователя
	int depth = 0;

	VirtualItem *current[MAX_FOLDERS_DEPTH];
	current[depth] = new VirtualItem("root");
	for (int i = 1; i < MAX_FOLDERS_DEPTH; i++) {
		current[i] = new VirtualItem();
		current[i] = NULL;
	}
	loadDataFormFile(current);

	while (1) {
		flag = false;
		if (depth == 0) {
			cout << "X:\\>";
		}
		else {
			cout << "X:\\";
			for (int i = 1; i <= depth; i++) {
				cout << current[i]->getItemName();
				cout << "\\";
			}
			cout << ">";
		}

		char *inputCommand = new char[CHAR_BUFFER_SIZE];
		cin >> inputCommand;

		for (int i = 0; i < NUMBER_OF_COMMANDS; i++) {
			if (strcmp(inputCommand, commands[i]) == 0) {
				flag = true;
				switch (i) {
				case COMMAND_HELP: {
					help();
					break;
				}
				case COMMAND_SHOW_DIRECTORY_STRUCTURE: {
					current[depth]->seeDirectoryStruct();
					break;
				}
				case COMMAND_MOVE_TO_NEXT_FOLDER: {
					char *inputSecondArg = new char[CHAR_BUFFER_SIZE];
					cin >> inputSecondArg;
					depth++;
					current[depth] = current[depth - 1]->moveToNextFolder(inputSecondArg);
					if (current[depth] == NULL) {
						depth--;
					}
					break;
				}
				case COMMAND_MOVE_TO_PREVIOUS_FOLDER: {
					if (depth > 0) {
						current[depth] = NULL;
						depth--;
					}
					else {
						cout << "	Error!" << endl << endl;
					}
					break;
				}
				case COMMAND_EXIT: {
					delete[] inputCommand;
					return;
				}
				case COMMAND_CREATE_NEW_FILE: {
					char *inputSecondArg = new char[CHAR_BUFFER_SIZE];
					cin >> inputSecondArg;
					current[depth]->createNewFile(inputSecondArg, 0, 0);
					delete[] inputSecondArg;
					remove(DISK_TREE_NAME);
					current[0]->writeDataInTreeFile();
					break;
				}
				case COMMAND_CREATE_NEW_FOLDER: {
					char *inputSecondArg = new char[CHAR_BUFFER_SIZE];
					cin >> inputSecondArg;
					current[depth]->createNewFolder(inputSecondArg);
					delete[] inputSecondArg;
					remove(DISK_TREE_NAME);
					current[0]->writeDataInTreeFile();
					break;
				}
				case COMMAND_DELETE_FILE_OR_FOLDER: {
					char *inputSecondArg = new char[CHAR_BUFFER_SIZE];
					cin >> inputSecondArg;
					current[depth]->deleteFileOrFolder(inputSecondArg);
					delete[] inputSecondArg;
					remove(DISK_TREE_NAME);

					int start = 0;
					int size = 0;
					FILE *tempFile;
					tempFile = fopen("temp2", "r");
					if (tempFile) {
						fscanf(tempFile, "%d ", &size);
						fscanf(tempFile, "%d ", &start);
						fclose(tempFile);
						remove("temp2");
						current[0]->defragmentation(start, size);
					}

					current[0]->writeDataInTreeFile();
					break;
					break;
				}
				case COMMAND_COPY_FILE_FROM_REAL_TO_VIRTUAL_DISK: {
					char *inputSecondArg = new char[CHAR_BUFFER_SIZE];
					cin >> inputSecondArg;
					current[depth]->copyFileFromRealSystem(inputSecondArg);
					delete[] inputSecondArg;
					remove(DISK_TREE_NAME);
					current[0]->writeDataInTreeFile();
					break;
				}
				case COMMAND_COPY_FILE_FROM_VIRTUAL_DISK_TO_REAL: {
					char *inputSecondArg = new char[CHAR_BUFFER_SIZE];
					cin >> inputSecondArg;
					current[depth]->copyFileFromVirtualSystem(inputSecondArg);
					delete[] inputSecondArg;
					remove(DISK_TREE_NAME);
					current[0]->writeDataInTreeFile();
					break;
				}
				default:
					break;
				}
				break;
			}
		}

		if (flag == false) {
			cout << endl << "Error! Use 'help'." << endl << endl;
		}

		delete[] inputCommand;
		while (cin.get() != '\n');
	}

	for (int i = 1; i < MAX_FOLDERS_DEPTH; i++) {
		delete current[i];
	}
	delete current[depth];
}

int main() { //проверка наличия необх. файлов
	        // и создание нового диска если их нет
	FILE *vitrualDisk;
	FILE *vitrualDiskTree;
	FILE *virtualDiskSectorsControl;

	vitrualDisk = fopen(DISK_NAME, "r+b");
	vitrualDiskTree = fopen(DISK_TREE_NAME, "r+b");
	virtualDiskSectorsControl = fopen(DISK_SECTORS_CONTROL, "r+b");

	if (!vitrualDisk || !vitrualDiskTree || !virtualDiskSectorsControl) {
		createVirtualDisk();
	}

	if (vitrualDisk) {
		fclose(vitrualDisk);
	}
	if (vitrualDiskTree) {
		fclose(vitrualDiskTree);
	}
	if (virtualDiskSectorsControl) {
		fclose(virtualDiskSectorsControl);
	}

	openVirtualDisk();

	return 0;
}
