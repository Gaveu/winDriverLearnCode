#include<ntddk.h>

void Unload(PDRIVER_OBJECT DriverObject)
{
	IoDeleteDevice(DriverObject->DeviceObject);	//PDEVICE_OBJECT
	KdPrint(("GoodBye Driver!\n"));
	
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;
	PDEVICE_OBJECT DeviceObject;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\HelloDDK");
	KdPrint(("DriverLoading!\n"));
	DriverObject->DriverUnload = Unload;

	status = IoCreateDevice(
		DriverObject,			//PDRIVER_OBJECT	DriverObject		指定的新建设备对象由什么驱动对象调度
		0,						//ULONG				DeviceExtensionSize	新建设备对象的设备扩展的字节大小
		&DeviceName,			//PUNICODE_STRING	DeviceName			可选项，指定该设备对象的命名，必须是一个完整的路径
		FILE_DEVICE_UNKNOWN,	//DEVICE_TYPE		DeviceType			指定设备对象的类型，如FILE_DEVICE_DISK、FILE_DEVICE_KEYBOARD等
		0,						//ULONG				DeviceCharacteristics	指定设备属性，如FILE_DEVICE_SECURE_OPEN等
		FALSE,					//BOOLEAN			Exclusive			指定是否有拓展设备
		&DeviceObject			//PDEVICE_OBJECT	*DeviceObject		返回新建的设备对象，该设备对象位于不分页内存池
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("DeviceObject Create Failed!\n"));
		return status;
	}
	//设定标志位，表示该设备对象以缓冲区输入输出，并结束初始化
	DeviceObject->Flags |= DO_BUFFERED_IO;			
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	return STATUS_SUCCESS;
}