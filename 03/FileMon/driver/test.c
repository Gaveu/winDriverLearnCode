#include "head.h"

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	DriverObject->DriverUnload = Unload;
	KdPrint(("Hello Driver!\n"));
	
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\FileMonTest");
	UNICODE_STRING SymboliclinkName = RTL_CONSTANT_STRING(L"\\??\\FileMonTest");
	PDEVICE_OBJECT DeviceObject = NULL;
	int i = 0;
	//创建硬件对象、符号链接
	status = IoCreateDevice(
		DriverObject,
		sizeof(DEVICE_EXTENSION),
		&DeviceName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&DeviceObject
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Device Create Failed!%x\n",status));
		return status;
	}
	status = IoCreateSymbolicLink(&SymboliclinkName, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("SymbolicLink Create Failed!%x\n", status));
		IoDeleteDevice(DeviceObject);
		IoDeleteSymbolicLink(&SymboliclinkName);
		return status;
	}
	//初始化硬件扩展
	status = InitDeviceExtension(DeviceObject->DeviceExtension);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(DeviceObject);
		return status;
	}

	//初始化派遣函数
	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		DriverObject->MajorFunction[i] = DispatchRoutine;
	}
	DriverObject->MajorFunction[IRP_MJ_READ] = DispatchRead;
	DriverObject->MajorFunction[IRP_MJ_WRITE] = DispatchWrite;
	DriverObject->MajorFunction[IRP_MJ_QUERY_INFORMATION] = DispatchQuery;
	
	//标志位设置
	DeviceObject->Flags |= DO_BUFFERED_IO;
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
	

	return STATUS_SUCCESS;
}