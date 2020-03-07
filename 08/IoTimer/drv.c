#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	//停止设备对象的IO定时器
	IoStopTimer(DriverObject->DeviceObject);
	IoDeleteDevice(DriverObject->DeviceObject);
	KdPrint(("Goodbye Driver!\n"));
}

VOID IoTimerRoutine(
	IN PDEVICE_OBJECT DeviceObject, 
	IN PVOID Context)
{
	KdPrint(("IoTimer Tik!\n"));
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;
	NTSTATUS status;
	PDEVICE_OBJECT DeviceObject;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\IoTimerTest");
	UNICODE_STRING SymboliclinkName = RTL_CONSTANT_STRING(L"\\??\\IoTimerTest");
	status = IoCreateDevice(
		DriverObject,
		0,
		&DeviceName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&DeviceObject
		);

	//初始化定时器
	IoInitializeTimer(
		DeviceObject,	//定时器所在的设备对象
		IoTimerRoutine,	//定时器触发的回调函数
		NULL			//上下文，纯地址
		);

	//启动IO定时器
	IoStartTimer(DeviceObject);


	return STATUS_SUCCESS;
}