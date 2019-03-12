#include<stdio.h>
#include<Windows.h>

int main()
{
	HANDLE hDevice = CreateFile(
		TEXT("\\\\.\\HelloDDK"),
		GENERIC_ALL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("File create failed! 0x%x\n",GetLastError());
		return -1;
	}
	DWORD dwRet;
	DWORD dwFileSize;
	WriteFile(hDevice, "This is a string from user", strlen("This is a string from user"), &dwRet, NULL);	//对应驱动中的DispatchWrite()
	dwFileSize = GetFileSize(hDevice, NULL);	//对应驱动中的DispatchQueryInfo()

	printf("FileSize:%d\n", dwFileSize);

	CloseHandle(hDevice);
	return 0;
}