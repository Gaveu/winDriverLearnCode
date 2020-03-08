#include<ntddk.h>

//未文档化，需自行声明
NTKERNELAPI
NTSTATUS
ObReferenceObjectByName(
IN PUNICODE_STRING ObjectName,
IN ULONG Attributes,
IN PACCESS_STATE PassedAccessState OPTIONAL,
IN ACCESS_MASK DesiredAccess OPTIONAL,
IN POBJECT_TYPE ObjectType,
IN KPROCESSOR_MODE AccessMode,
IN OUT PVOID ParseContext OPTIONAL,
OUT PVOID *Object
);
extern POBJECT_TYPE *IoDeviceObjectType;


VOID Unload(IN PDRIVER_OBJECT DriverObject)
{

	KdPrint(("Goodbye DriverCaller!\n"));
}

NTSTATUS DrvCallerTest5()
{
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\??\\DriverA");
	NTSTATUS status;
	KEVENT event;
	IO_STATUS_BLOCK IoStatus;
	PFILE_OBJECT pFileObject;
	PDEVICE_OBJECT DeviceObject;
	PIRP pIrp;

	//根据设备名寻找对应设备对象指针
	status = ObReferenceObjectByName(
		&DeviceName,	//待查找设备对象名
		OBJ_CASE_INSENSITIVE,	//访问属性，不区分大小写	
		NULL,			//不设置访问等级
		FILE_ANY_ACCESS,//任意文件访问权限
		*IoDeviceObjectType,//对象类型为设备对象
		KernelMode,		//驱动对象则填内核模式
		NULL,			//输出上下文，此处不需要
		&DeviceObject	//返回的设备对象
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("DeviceObject Find Failed!%x\n", status));
		return status;
	}

	KdPrint(("DeviceObject Get Success!\n"));

	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello DriverCaller!\n"));
	DriverObject->DriverUnload = Unload;

	DrvCallerTest5();

	return STATUS_SUCCESS;
}