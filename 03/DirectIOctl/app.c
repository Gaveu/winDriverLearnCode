#include<stdio.h>
#include<windows.h>
#include<winioctl.h>

#define CTLDRTINCODE CTL_CODE(FILE_DEVICE_UNKNOWN,0x1001,METHOD_IN_DIRECT,FILE_ANY_ACCESS)
#define CTLDRTOUTCODE CTL_CODE(FILE_DEVICE_UNKNOWN,0x1002,METHOD_OUT_DIRECT,FILE_ANY_ACCESS)

int main()
{
	HANDLE hDevice = CreateFile(
		TEXT("\\\\.\\CTLDirectIO"),
		GENERIC_ALL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("File Create Failed!\n");
		return -1;
	}
	CHAR inData[1024] = { 0 };
	CHAR outData[1024] = { 0 };
	int retLen = 0;
	RtlCopyMemory(inData, "This is a string from User!", 1024);
	DeviceIoControl(
		hDevice,
		CTLDRTINCODE,
		inData,
		1024,
		outData,
		1024,
		&retLen,
		NULL);
	DeviceIoControl(
		hDevice,
		CTLDRTOUTCODE,
		inData,
		1024,
		outData,
		1024,
		&retLen,
		NULL);
	printf("DataGet:%s\n", outData);
	printf("DataLen:%d\n", retLen);
	CloseHandle(hDevice);
	return 0;
}
