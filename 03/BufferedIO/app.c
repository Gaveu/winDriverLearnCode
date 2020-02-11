#include<stdio.h>
#include<windows.h>
#include<winioctl.h>

#define IOCTLCODE CTL_CODE(FILE_DEVICE_UNKNOWN,0X800,METHOD_BUFFERED,FILE_ANY_ACCESS)

int main()
{
	HANDLE hDevice = CreateFile(
		TEXT("\\\\.\\BufferIO"),				//LPCSTR		lpFileName,			"\\"表示本机全局根路径
		GENERIC_ALL,							//DWORD			dwDesiredAccess,
		FILE_SHARE_READ | FILE_SHARE_WRITE,		//DWORD			dwShareMode,
		NULL,									//LPSECURITY_ATTRIBUTES	lpSecurityAttributes,
		OPEN_EXISTING,							//DWORD			dwCreationDisposition,
		FILE_ATTRIBUTE_NORMAL,					//DWORD			dwFlagsAndAttributes,
		NULL									//HANDLE		hTemplateFile
		);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("File Create Failed!\n");
		return -1;
	}

	char buffer[1024] = { 0 };
	DWORD dwRet;
	ReadFile(
		hDevice,
		buffer,
		1024,
		&dwRet,
		NULL);
	printf("Read:%s\n", buffer);
	printf("ReadLen:%d\n", dwRet);
	printf("File Read Success!\n");

	char *buffer2 = "This a data form User!\n";
	WriteFile(
		hDevice,
		buffer2,
		strlen(buffer2),
		&dwRet,
		NULL
		);
	printf("Write:%s\n", buffer);
	printf("WriteLen:%d\n", dwRet);
	printf("File Write Success!\n");
	CloseHandle(hDevice);
	return 0;
}