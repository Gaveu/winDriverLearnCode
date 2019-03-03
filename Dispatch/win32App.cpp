#include<Windows.h>
#include<stdio.h>
#include<winioctl.h>

#define IOTEST CTL_CODE(FILE_DEVICE_UNKNOWN,0x800,METHOD_BUFFERED,FILE_ANY_ACCESS)
int main()
{
	DWORD dwRet;
	HANDLE hDevice = CreateFile(TEXT("\\\\.\\HelloDDK"),
		GENERIC_ALL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);//对应IRP请求中的IRP_MJ_CREATE
	if (hDevice == NULL)
	{
		printf("Device open failed! %d\n",GetLastError());
		return -1;
	}

	ReadFile(hDevice, NULL, 0, &dwRet, NULL);	//对应IRP请求中的IRP_MJ_READ
	WriteFile(hDevice, NULL, 0, &dwRet, NULL);	//对应IRP请求中的IRP_MJ_WRITE
	GetFileSize(hDevice, NULL);	//对应IRP请求中的IRP_MJ_QUERY_INFORMATION
	DeviceIoControl(hDevice, IOTEST, NULL, 0, NULL, 0, &dwRet, NULL);	//对应IRP请求中的IRP_MJ_DEVICE_CONTROL
	CloseHandle(hDevice);	//对应IRP请求中的IRP_MJ_CLEANUP和IRP_MJ_CLOSE


	return 0;
}