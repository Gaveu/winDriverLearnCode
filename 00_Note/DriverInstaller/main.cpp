#include<stdio.h>
#include<windows.h>

int main()
{
	SERVICE_STATUS status;
	SC_HANDLE hScManager = OpenSCManager(
		NULL,	//���� 
		NULL,	//�������ݿ�
		SC_MANAGER_ALL_ACCESS	//��ȫȨ��
		);	
	if (!hScManager)
	{
		printf("OpenSCManager failed!\n");
		return -1;
	}
	
	SC_HANDLE hService = CreateService(
		hScManager,		//������������
		TEXT("Hello"),	//�½�������
		TEXT("Hello"),	//��ʾ��������
		SERVICE_ALL_ACCESS,	//����Ȩ��
		SERVICE_KERNEL_DRIVER,	//�������� �ں�����
		SERVICE_DEMAND_START,	//��������
		SERVICE_ERROR_IGNORE,	//������� ����
		TEXT("C:\\Users\\Administrator\\DriverCode\\02\\HelloWorld\\x64\\Win7Debug\\HelloWorld.sys"),	//����Ķ������ļ�·��
		NULL,	//��������
		NULL,	//�����ǩ
		NULL,	//��������ǰ���������ķ���������
		NULL,	//���������û���	NULL��ʾʹ��LocalSystem
		NULL	//���������û�����
		);	//��Ӧ����	sc create Hello binpath= C:\Users\Administrator\DriverCode\02\HelloWorld\x64\Win7Debug\HelloWorld.sys type= kernel
	if (!hService)
	{
		printf("CreateService failed! 0x%x\n",GetLastError());
		CloseServiceHandle(hScManager);
		return -2;
	}

	if (!StartService(
		hService,	//����������
		NULL,		//����������Ŀ
		NULL)		//����������
			)	//��Ӧ����	net start Hello
	{
		printf("StartService failed! 0x%x\n", GetLastError());
		CloseServiceHandle(hScManager);
		CloseServiceHandle(hService);
		return -3;
	}

	getchar();

	if (!ControlService(
		hService,	//���Ƶķ�����
		SERVICE_CONTROL_STOP,	//���Ʒ���Ĳ�������
		&status)		//���ش˴β����Ľ��
		)	//��Ӧ����	net stop Hello
	{
		printf("StopService failed! 0x%x\n", GetLastError());
		CloseServiceHandle(hScManager);
		CloseServiceHandle(hService);
		return -4;
	}
	
	if (!DeleteService(hService))	//��Ӧ����	sc delete Hello
	{
		printf("DeleteService failed! 0x%x\n", GetLastError());
		CloseServiceHandle(hScManager);
		CloseServiceHandle(hService);
		return -5;
	}

	CloseServiceHandle(hScManager);
	CloseServiceHandle(hService);

	return 0;
}
