#include<stdio.h>
#include<windows.h>

int main()
{
	CHAR Data[1024] = { 0 };
	DWORD dwRet = 0;
	HANDLE hDevice = CreateFile(
		TEXT("\\\\.\\NeitherIO"),
		GENERIC_ALL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("File Create Failed!\n");
		return -1;
	}

	ReadFile(
		hDevice,
		Data,
		1024,
		&dwRet,
		0);
	Data[dwRet] = '\0';
	printf("%s\n", Data);
	printf("ReadLen:%d\n", dwRet);
	strcpy_s(Data, 1024, "This is a string from User!\n");
	WriteFile(
		hDevice,
		Data,
		strnlen(Data, 1024),
		&dwRet,
		0);

	return 0;
}

