#include<stdio.h>
#include<Windows.h>
#include<stdlib.h>

//获取数组元素个数
#define ARR_LEN(a) (sizeof((a))/sizeof((a[0])))

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
		printf("Device open failed! %d\n", GetLastError());
		return -1;
	}

	/*
	由于用户空间中的读内存长度为1024，
	IO管理器首先会在内核空间中开辟一段长度也为1024的内存(本例中即为Irp->AssociatedIrp.SystemBuffer)，
	本例driver中收到读的IRP请求后，调用派遣函数DispatchRead将数据写入到这段内存中，
	完毕后再将这段内存中的数据由IO管理器来拷贝到用户空间的读内存。
	*/
	DWORD dwRet;
	char readBuffer[1024] = { 0 };
	ReadFile(hDevice, readBuffer, sizeof(readBuffer), &dwRet, NULL);
	printf("DataReadFromKernel:%s\n", readBuffer);
	printf("DataReadSize:%d\n", dwRet);

	CloseHandle(hDevice);
	return 0;
}