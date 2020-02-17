#include<stdio.h>
#include<windows.h>
#include<winioctl.h>

#define CTLNEITHERCODE CTL_CODE(FILE_DEVICE_UNKNOWN,0x1003,METHOD_NEITHER,FILE_ANY_ACCESS)

int main()
{
	HANDLE hDevice = CreateFile(
		TEXT("\\\\.\\CTLNeitherIO"),
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
		CTLNEITHERCODE,
		inData,
		1024,
		outData,
		1024,
		&retLen,
		NULL);
	printf("DataGet:%s\n", outData);
	printf("DataLen:%d\n", retLen);

	return 0;
}
