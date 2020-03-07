#include<ntddk.h>

#define CTLBUFSENTEVENT CTL_CODE(FILE_DEVICE_UNKNOWN,0x1000,METHOD_BUFFERED,FILE_ANY_ACCESS)

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING SymboliclinkName = RTL_CONSTANT_STRING(L"\\??\\UserEventTest");
	IoDeleteDevice(DriverObject->DeviceObject);
	IoDeleteSymbolicLink(&SymboliclinkName);
	KdPrint(("Goodbye Driver!\n"));
}

NTSTATUS DispatchRoutine(IN PDEVICE_OBJECT DeivceObject, IN PIRP Pirp)
{
	static PCHAR name[] =
	{
		"IRP_MJ_CREATE					",
		"IRP_MJ_CREATE_NAMED_PIPE		",
		"IRP_MJ_CLOSE					",
		"IRP_MJ_READ					",
		"IRP_MJ_WRITE					",
		"IRP_MJ_QUERY_INFORMATION		",
		"IRP_MJ_SET_INFORMATION			",
		"IRP_MJ_QUERY_EA				",
		"IRP_MJ_SET_EA					",
		"IRP_MJ_FLUSH_BUFFERS			",
		"IRP_MJ_QUERY_VOLUME_INFORMATION",
		"IRP_MJ_SET_VOLUME_INFORMATION	",
		"IRP_MJ_DIRECTORY_CONTROL		",
		"IRP_MJ_FILE_SYSTEM_CONTROL		",
		"IRP_MJ_DEVICE_CONTROL			",
		"IRP_MJ_INTERNAL_DEVICE_CONTROL	",
		"IRP_MJ_SHUTDOWN				",
		"IRP_MJ_LOCK_CONTROL			",
		"IRP_MJ_CLEANUP					",
		"IRP_MJ_CREATE_MAILSLOT			",
		"IRP_MJ_QUERY_SECURITY			",
		"IRP_MJ_SET_SECURITY			",
		"IRP_MJ_POWER					",
		"IRP_MJ_SYSTEM_CONTROL			",
		"IRP_MJ_DEVICE_CHANGE			",
		"IRP_MJ_QUERY_QUOTA				",
		"IRP_MJ_SET_QUOTA				",
		"IRP_MJ_PNP						"
	};

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Pirp);

	KdPrint(("%s\n", name[stack->MajorFunction]));
	Pirp->IoStatus.Status = STATUS_SUCCESS;
	Pirp->IoStatus.Information = 0;
	IoCompleteRequest(Pirp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DispatchIoCtl(IN PDEVICE_OBJECT DeivceObject, IN PIRP Pirp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Pirp);
	NTSTATUS status;
	PCHAR InBuf, OutBuf;
	int InLen, OutLen;
	int retLen = 0;
	ULONG ctlCode = stack->Parameters.DeviceIoControl.IoControlCode;
	
	switch (ctlCode)
	{
	case CTLBUFSENTEVENT:
	{
		//从用户态获取事件句柄
		HANDLE hEvent = *(PHANDLE)Pirp->AssociatedIrp.SystemBuffer;
		PKEVENT pEvent;
		status = ObReferenceObjectByHandle(
			hEvent,					//待回溯的对象句柄
			EVENT_MODIFY_STATE,		//访问权限
			*ExEventObjectType,		//对象类型为事件类型
			UserMode,				//对象在用户态下创建
			&pEvent,				//返回的事件地址
			NULL					//无需句柄信息
			);

		if (NT_SUCCESS(status))
		{
			//在内核态配置事件对象
			KeSetEvent(
				pEvent,				//待解引用的事件对象 
				IO_NO_INCREMENT,	//解引用后不提升优先级
				FALSE				//无后接等待类函数
				);
			//返还事件对象
			ObDereferenceObject(pEvent);
			retLen = 0;
		}
	}break;


	default:
	{
		KdPrint(("Unknown CtlCode!\n"));
		status = STATUS_UNSUCCESSFUL;
		retLen = 0;
	}break;
	}

	Pirp->IoStatus.Information = retLen;
	Pirp->IoStatus.Status = status;
	IoCompleteRequest(Pirp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	DriverObject->DriverUnload = Unload;
	KdPrint(("Hello Driver!\n"));

	int i;
	NTSTATUS status;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\UserEventTest");
	UNICODE_STRING SymboliclinkName = RTL_CONSTANT_STRING(L"\\??\\UserEventTest");
	PDEVICE_OBJECT DeviceObject = NULL;

	status = IoCreateDevice(
		DriverObject,
		0,
		&DeviceName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&DeviceObject
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Device Create Failed!%x\n", status));
		return status;
	}
	status = IoCreateSymbolicLink(
		&SymboliclinkName,
		&DeviceName
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("SymbolicLink Create Failed!%x\n", status));
		IoDeleteDevice(DeviceObject);
		return status;
	}

	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		DriverObject->MajorFunction[i] = DispatchRoutine;
	}
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIoCtl;
	


	DeviceObject->Flags |= 0;
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
	return STATUS_SUCCESS;
}