#include <stddef.h>
#include <stdio.h>
#include <locale.h>
#include <windows.h>
#define SIZE 512

struct Block 
{
	int begin;
	int size;
	void **ptr;
};

unsigned char memory[SIZE];
int num_of_blocks;
struct Block  busy[SIZE];


void my_malloc(void **ptr, int size) 
{
	if (size <= 0 || size>SIZE)
	{
		return;
	}	
	if (num_of_blocks == 0)
	{
		busy[0].begin = 0;
		busy[0].size = size;
		num_of_blocks++;
		busy[0].ptr = ptr;
		*ptr = memory;
		return;
	}
	int prev = 0;
	for (int i = 0; i <= num_of_blocks; i++)
	{
		int end = busy[i].begin + busy[i].size;
		if (i == num_of_blocks-1)
		{
			int space_left = SIZE - end;
			if (space_left > size)
			{
				busy[i + 1].begin = end+1;
				busy[i + 1].size = size;
				num_of_blocks++;
				busy[i+1].ptr = ptr;
				*ptr = memory + end+1;
				return;
			}
			else
				return;
		}
		if (busy[i].begin - prev > size)
		{
			for (int j = num_of_blocks; j >= i; j--)
				busy[j + 1] = busy[j];
			busy[i].begin = prev+1;
			busy[i].size = size;
			num_of_blocks++;
			busy[i].ptr = ptr;
			*ptr = memory + prev+1;
			return;
		}
		prev = end;
	}
}

void my_free(void *ptr)
{
	int begin = -1*(memory - ptr);
	for (int i = 0; i <= num_of_blocks; i++)
	{
		if (begin == busy[i].begin)
		{
			int size = busy[i].size;
			for (int j = busy[i].begin; j < SIZE - size; j++)
				memory[j] = memory[j + size];
			*(busy[i].ptr)=NULL;
			for (int j = i; j < num_of_blocks-1; j++)
			{
				busy[j] = busy[j + 1];
				busy[j].begin -= size;
				*(busy[j].ptr) = memory + busy[j].begin;
			}
			num_of_blocks--;
			return;
		}
	}
}


void* my_realloc(void *ptr, int new_size)
{
	int begin = -1 * (memory - ptr);
	for (int i = 0; i <= num_of_blocks; i++)
	{
		if (begin == busy[i].begin)
		{
			unsigned char buf[SIZE];
			int size = busy[i].size;
			for (int j = 0; j < size; j++)
				buf[j] = memory[j + begin];
			my_free(ptr);
			unsigned char *new_ptr;
			my_malloc((void **)&new_ptr, new_size);
			for (int j = 0; j < size; j++)
				new_ptr[j] = buf[j];
			return new_ptr;
		}
	}
}

void mem_state()
{
	int busy_space = 0;
	for (int i = 0; i < num_of_blocks; i++)
		busy_space += busy[i].size;
	printf("%d blocks now in memory takes %d bytes, %d bytes if free.\n", num_of_blocks, busy_space, SIZE - busy_space);
}

int main()
{
	num_of_blocks = 0;
	char *str;
	my_malloc((void**)&str, sizeof(char)* 10);
	for (int i = 0; i < 10; i++)
		str[i] = i+48;
	puts(str);

	mem_state();

	int *mas;
	my_malloc((void **)&mas,sizeof(int)* 10);
	for (int i = 0; i < 10; i++)
	{
		mas[i] = i + 10;
		printf("%d ", mas[i]);
	}
	printf("\n");
	//puts(str);

	mem_state();

	char *str2;
	my_malloc((void **)&str2, sizeof(char)* 10);
	for (int i = 0; i < 10; i++)
		str2[i] = i + 97;
	puts(str2);

	mem_state();

	my_free(mas);
	mas;
	printf("\n");

	puts(str2);
	mem_state();

	str = (char*)my_realloc(str, 11);
	str[10] = 107;
	puts(str);

	mem_state();
	system("pause");
}