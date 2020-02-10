#include<stdio.h>
#include<windows.h>
#include<winioctl.h>

#define IO_TEST_CODE CTL_CODE(FILE_DEVICE_UNKNOWN,0X800,METHOD_BUFFERED,FILE_ANY_ACCESS)
int main()
{
	HANDLE hDevice = CreateFile(
		TEXT("\\\\.\\DispatchTest"),			//LPCSTR		lpFileName,			"\\"表示本机全局根路径
		GENERIC_ALL,							//DWORD			dwDesiredAccess,
		FILE_SHARE_READ | FILE_SHARE_WRITE,		//DWORD			dwShareMode,
		NULL,									//LPSECURITY_ATTRIBUTES	lpSecurityAttributes,
		OPEN_EXISTING,							//DWORD			dwCreationDisposition,
		FILE_ATTRIBUTE_NORMAL,					//DWORD			dwFlagsAndAttributes,
		NULL									//HANDLE		hTemplateFile
		);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Device Create Failed!%d\n",GetLastError());
		return -1;
	}
	DWORD dwRet;
	ReadFile(
		hDevice, 
		NULL, 
		0, 
		&dwRet, 
		NULL
		);
	WriteFile(
		hDevice, 
		NULL, 
		0, 
		&dwRet,
		NULL
		);
	GetFileSize(hDevice, NULL);
	DeviceIoControl(hDevice,
		IO_TEST_CODE,
		NULL,
		0,
		NULL,
		0,
		&dwRet,
		NULL
		);
	CloseHandle(hDevice);
	return 0;
}