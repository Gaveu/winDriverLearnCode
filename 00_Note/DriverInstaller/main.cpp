#include<stdio.h>
#include<windows.h>

int main()
{
	SERVICE_STATUS status;
	SC_HANDLE hScManager = OpenSCManager(
		NULL,	//本机 
		NULL,	//本机数据库
		SC_MANAGER_ALL_ACCESS	//完全权限
		);	
	if (!hScManager)
	{
		printf("OpenSCManager failed!\n");
		return -1;
	}
	
	SC_HANDLE hService = CreateService(
		hScManager,		//服务管理器句柄
		TEXT("Hello"),	//新建服务名
		TEXT("Hello"),	//显示服务名称
		SERVICE_ALL_ACCESS,	//访问权限
		SERVICE_KERNEL_DRIVER,	//服务类型 内核驱动
		SERVICE_DEMAND_START,	//按需启动
		SERVICE_ERROR_IGNORE,	//错误控制 忽略
		TEXT("C:\\Users\\Administrator\\DriverCode\\02\\HelloWorld\\x64\\Win7Debug\\HelloWorld.sys"),	//服务的二进制文件路径
		NULL,	//服务组名
		NULL,	//服务标签
		NULL,	//启动服务前必须启动的服务或服务组
		NULL,	//服务启动用户名	NULL表示使用LocalSystem
		NULL	//服务启动用户密码
		);	//对应命令	sc create Hello binpath= C:\Users\Administrator\DriverCode\02\HelloWorld\x64\Win7Debug\HelloWorld.sys type= kernel
	if (!hService)
	{
		printf("CreateService failed! 0x%x\n",GetLastError());
		CloseServiceHandle(hScManager);
		return -2;
	}

	if (!StartService(
		hService,	//启动服务句柄
		NULL,		//启动参数数目
		NULL)		//启动参数组
			)	//对应命令	net start Hello
	{
		printf("StartService failed! 0x%x\n", GetLastError());
		CloseServiceHandle(hScManager);
		CloseServiceHandle(hService);
		return -3;
	}

	getchar();

	if (!ControlService(
		hService,	//控制的服务句柄
		SERVICE_CONTROL_STOP,	//控制服务的操作类型
		&status)		//返回此次操作的结果
		)	//对应命令	net stop Hello
	{
		printf("StopService failed! 0x%x\n", GetLastError());
		CloseServiceHandle(hScManager);
		CloseServiceHandle(hService);
		return -4;
	}
	
	if (!DeleteService(hService))	//对应命令	sc delete Hello
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
