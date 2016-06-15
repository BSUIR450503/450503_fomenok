#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#include <stdio.h>

using namespace std;

#define DISK_NAME "virtualDisk.dat"
#define DISK_TREE_NAME "virtualDiskTree.dat"
#define DISK_SECTORS_CONTROL "virtualDiskSectorsControl.dat"
#define MAX_DISK_SIZE 1073741824
#define MAX_NAME_SIZE 30
#define MAX_ITEMS_IN_FOLDER 10
#define MAX_FOLDERS_DEPTH 10
#define CHAR_BUFFER_SIZE 255
#define INT_BUFFER_SIZE 1024
#define _FILE 1
#define _FOLDER 0
#define NUMBER_OF_COMMANDS 10

enum Commands {
	COMMAND_HELP = 0,
	COMMAND_SHOW_DIRECTORY_STRUCTURE = 1,
	COMMAND_MOVE_TO_NEXT_FOLDER = 2,
	COMMAND_MOVE_TO_PREVIOUS_FOLDER = 3,
	COMMAND_EXIT = 4,
	COMMAND_CREATE_NEW_FILE = 5,
	COMMAND_CREATE_NEW_FOLDER = 6,
	COMMAND_DELETE_FILE_OR_FOLDER = 7,
	COMMAND_COPY_FILE_FROM_REAL_TO_VIRTUAL_DISK = 8,
	COMMAND_COPY_FILE_FROM_VIRTUAL_DISK_TO_REAL = 9,
};

static const char *commands[] = { "help", "struct", "cd", "fd",
"exit", "newfl", "newfr", "del",
"push", "pop" };