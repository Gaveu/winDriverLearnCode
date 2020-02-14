#include<stdio.h>
#include<windows.h>

int main()
{
	char buffer[1024] = { 0 };
	DWORD dwRet;
	HANDLE hDevice = CreateFile(
		TEXT("\\\\.\\DirectIOtest"),
		GENERIC_ALL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Device Create Failed!\n");
		return -1;
	}
	WriteFile(
		hDevice,
		"This is a data form User!\n",
		strlen("This is a data form User!\n"),
		&dwRet,
		NULL
		);
	printf("DataBytesWriten:%d\n", dwRet);
	ReadFile(
		hDevice,
		buffer,
		32,
		&dwRet,
		NULL);
	printf("DataRead:%s\n", buffer);
	printf("DataBytesRead:%d\n", dwRet);
	printf("DataReadPtr:%p\n", buffer);
	return 0;
}