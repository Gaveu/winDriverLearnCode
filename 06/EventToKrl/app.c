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
	//�����ں�̬Ӳ������
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
	//���û�̬�����¼�����
	HANDLE hEvent = CreateEvent(
		NULL,
		FALSE,
		FALSE,
		NULL
		);

	//���û�̬������ͬ���Ķ���(�߳�)
	HANDLE hThread = CreateThread(
		NULL,
		0,
		ThreadProc,
		&hEvent,
		0,
		NULL
		);

	//���ں�̬�������ں�̬������¼����
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