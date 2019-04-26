#include<ntddk.h>

//CTL_CODE(设备类型,功能码,内存访问方式,文件访问权限)
#define CTLBUFFERED CTL_CODE(FILE_DEVICE_UNKNOWN,0x800,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define CTLDIRECT CTL_CODE(FILE_DEVICE_UNKNOWN,0x801,METHOD_OUT_DIRECT,FILE_ANY_ACCESS)
#define CTLNEITHER CTL_CODE(FILE_DEVICE_UNKNOWN,0x802,METHOD_NEITHER,FILE_ANY_ACCESS)

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloDDK_40839a6d");
	KdPrint(("Driver Unload!\n"));

	__try
	{
		IoDeleteSymbolicLink(&SymbolicLinkName);
		IoDeleteDevice(DriverObject->DeviceObject);
		KdPrint(("Delete Deivce and Symbolic Success!\n"));
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = GetExceptionCode();
		KdPrint(("Delete Failed! 0x%x\n", status));
	}

}

NTSTATUS DispatchRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	static PCHAR typeName[] =
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
	KdPrint(("%s\n",typeName[stack->MajorFunction]));
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DeviceControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	ULONG InputBufferLen = 0;	//读到的输入缓冲区长度
	ULONG OutputBufferLen = 0;	//读到的输出缓冲区长度
	ULONG Length = 0;			//实际读取或写入的数据长度
	PVOID InputBuffer = NULL;	//存储由IRP获得的输入缓冲区地址
	PVOID OutputBuffer = NULL;	//存储由IRP获得的输出缓冲区地址
	NTSTATUS status = STATUS_SUCCESS;	//运行存储状态返回值
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);		//获取当前IRP设备栈
	ULONG IoCtlCode = stack->Parameters.DeviceIoControl.IoControlCode;	//获取当前的IO控制码

	__try
	{
		switch (IoCtlCode)
		{

		case CTLBUFFERED:
		{
			InputBuffer = Irp->AssociatedIrp.SystemBuffer;
			InputBufferLen = stack->Parameters.DeviceIoControl.InputBufferLength;
			OutputBuffer = Irp->AssociatedIrp.SystemBuffer;
			OutputBufferLen = stack->Parameters.DeviceIoControl.OutputBufferLength;

			KdPrint(("InputBuffer:%s\n", InputBuffer));
			KdPrint(("InputBufferLen:%d\n", InputBufferLen));

			RtlCopyMemory(OutputBuffer, "This is the way of BUFFERED", strlen("This is the way of BUFFERED"));
			Length = strlen("This is the way of BUFFERED");
			status = STATUS_SUCCESS;

		}break;
		
		case CTLNEITHER:
		{
			InputBuffer = stack->Parameters.DeviceIoControl.Type3InputBuffer;
			InputBufferLen = stack->Parameters.DeviceIoControl.InputBufferLength;
			OutputBuffer = Irp->UserBuffer;
			OutputBufferLen = stack->Parameters.DeviceIoControl.OutputBufferLength;

			KdPrint(("InputBuffer:%s\n", InputBuffer));
			KdPrint(("InputBufferLength:%d\n", InputBufferLen));

			RtlCopyMemory(InputBuffer, "This is the way of NEITHER", strlen("This is the way of NEITHER"));
			status = STATUS_SUCCESS;
			Length = strlen("This is the way of NEITHER");
		}break;
		
		case CTLDIRECT:
		{
			InputBuffer = Irp->AssociatedIrp.SystemBuffer;
			InputBufferLen = stack->Parameters.DeviceIoControl.InputBufferLength;
			OutputBuffer = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
			OutputBufferLen = stack->Parameters.DeviceIoControl.OutputBufferLength;

			KdPrint(("InputBuffer:%s\n", InputBuffer));
			KdPrint(("InputBufferLength:%d\n", InputBufferLen));

			RtlCopyMemory(OutputBuffer, "This is the way of DIRECT", strlen("This is the way of DIRECT"));
			status = STATUS_SUCCESS;
			Length = 0;
		}break;

		default:
		{
			status = STATUS_SUCCESS;
			Length = 0;
		}break;
		}
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = GetExceptionCode();
		Length = 0;
		KdPrint(("DeviceControl Failed! 0x%x\n", status));

	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = Length;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Driver Load!\n"));
	DriverObject->DriverUnload = Unload;

	ULONG i = 0;
	PDEVICE_OBJECT pDevice = NULL;
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\HelloDDK_40839a6d");
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloDDK_40839a6d");

	status = IoCreateDevice(&DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &pDevice);
	if (!NT_STATUS(status))
	{
		KdPrint(("Deivce Create Failed! 0x%x\n", status));
		Unload(DriverObject);
		return status;
	}
	status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);
	if (!NT_STATUS(status))
	{
		KdPrint(("Symbolic Create Failed! 0x%x\n",status));
		Unload(DriverObject);
		return status;
	}

	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		DriverObject->MajorFunction[i] = DispatchRoutine;
	}
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceControl;


	pDevice->Flags |= DO_DIRECT_IO;
	pDevice->Flags ^= DO_DEVICE_INITIALIZING;




	return STATUS_SUCCESS;
}