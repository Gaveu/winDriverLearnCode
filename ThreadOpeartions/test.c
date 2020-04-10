#include"drvHead.h"
VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	//PsResume((HANDLE)2424);	//虚拟机内运行calc.exe进程对应pid为2424，执行成功时该进程继续执行

	KdPrint(("GoodBye Driver!\n"));
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;

	//PsEnum1();
	//PsEnum2();
	//PsSuspend((HANDLE)2424);	//虚拟机内运行calc.exe进程对应pid为2424，执行成功时该进程暂停执行
	//PsTerminate((HANDLE)2548);	//虚拟机内运行calc.exe进程对应pid为2548，执行成功时该进程结束执行

	PsEnum1();

	return STATUS_SUCCESS;
}

