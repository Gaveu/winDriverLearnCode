#include<stdio.h>
#include<windows.h>

#define CTLBUFSENTEVENT CTL_CODE(FILE_DEVICE_UNKNOWN,0x1000,METHOD_BUFFERED,FILE_ANY_ACCESS)

DWORD WINAPI ThreadProc(PVOID Context)
{
	HANDLE hEvent = *(PHANDLE)Context;

	printf("Enter SubThread!\n");
	printf("Waiting For Event Set From Kernel!\n");
	WaitForSingleObject(hEvent, INFINITE);
	printf("End SubThread!\n");
	return 0;
}

int main()
{
	DWORD dwRet;
	printf("Enter MainThread!\n");
	//连接内核态硬件对象
	HANDLE hDevice = CreateFile(
		TEXT("\\\\.\\UserEventTest"),
		GENERIC_ALL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Deivce Open Failed!\n");
		return -1;
	}
	//在用户态创建事件对象
	HANDLE hEvent = CreateEvent(
		NULL,
		FALSE,
		FALSE,
		NULL
		);

	//在用户态创建待同步的对象(线程)
	HANDLE hThread = CreateThread(
		NULL,
		0,
		ThreadProc,
		&hEvent,
		0,
		NULL
		);

	//往内核态发送由内核态处理的事件句柄
	DeviceIoControl(
		hDevice,
		CTLBUFSENTEVENT,
		&hEvent,
		sizeof(HANDLE),
		NULL,
		0,
		&dwRet,
		NULL
		);

	printf("Waiting For SubThread!\n");
	WaitForSingleObject(hThread, INFINITE);

	printf("End MainThread!\n");

	CloseHandle(hDevice);
	CloseHandle(hEvent);
	return 0;
}