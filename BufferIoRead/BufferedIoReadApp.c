#include<stdio.h>
#include<Windows.h>
#include<stdlib.h>

//��ȡ����Ԫ�ظ���
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
	�����û��ռ��еĶ��ڴ泤��Ϊ1024��
	IO���������Ȼ����ں˿ռ��п���һ�γ���ҲΪ1024���ڴ�(�����м�ΪIrp->AssociatedIrp.SystemBuffer)��
	����driver���յ�����IRP����󣬵�����ǲ����DispatchRead������д�뵽����ڴ��У�
	��Ϻ��ٽ�����ڴ��е�������IO���������������û��ռ�Ķ��ڴ档
	*/
	DWORD dwRet;
	char readBuffer[1024] = { 0 };
	ReadFile(hDevice, readBuffer, sizeof(readBuffer), &dwRet, NULL);
	printf("DataReadFromKernel:%s\n", readBuffer);
	printf("DataReadSize:%d\n", dwRet);

	CloseHandle(hDevice);
	return 0;
}