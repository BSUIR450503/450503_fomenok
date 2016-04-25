#include <cmath>  
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <windows.h>


int main(int argc, wchar_t *argv[])
{
#ifdef WIN32

	int  result = _wtoi(argv[1]);
	result = pow(result,2);
	return result;
}
#endif
















