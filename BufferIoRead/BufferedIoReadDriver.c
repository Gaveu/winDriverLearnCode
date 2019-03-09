#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("Driver Unload!\n"));
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloDDK");
	IoDeleteDevice(DriverObject->DeviceObject);
	IoDeleteSymbolicLink(&SymbolicLinkName);
}

NTSTATUS DispatchRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	static PCHAR typeIrp[] = {
		  "IRP_MJ_CREATE			     "
		, "IRP_MJ_CREATE_NAMED_PIPE		 "
		, "IRP_MJ_CLOSE					 "
		, "IRP_MJ_READ					 "
		, "IRP_MJ_WRITE					 "
		, "IRP_MJ_QUERY_INFORMATION		 "
		, "IRP_MJ_SET_INFORMATION		 "
		, "IRP_MJ_QUERY_EA				 "
		, "IRP_MJ_SET_EA				 "
		, "IRP_MJ_FLUSH_BUFFERS			 "
		, "IRP_MJ_QUERY_VOLUME_INFORMATION"
		, "IRP_MJ_SET_VOLUME_INFORMATION "
		, "IRP_MJ_DIRECTORY_CONTROL		 "
		, "IRP_MJ_FILE_SYSTEM_CONTROL	 "
		, "IRP_MJ_DEVICE_CONTROL		 "
		, "IRP_MJ_INTERNAL_DEVICE_CONTROL"
		, "IRP_MJ_SHUTDOWN				 "
		, "IRP_MJ_LOCK_CONTROL			 "
		, "IRP_MJ_CLEANUP				 "
		, "IRP_MJ_CREATE_MAILSLOT		 "
		, "IRP_MJ_QUERY_SECURITY		 "
		, "IRP_MJ_SET_SECURITY			 "
		, "IRP_MJ_POWER					 "
		, "IRP_MJ_SYSTEM_CONTROL		 "
		, "IRP_MJ_DEVICE_CHANGE			 "
		, "IRP_MJ_QUERY_QUOTA			 "
		, "IRP_MJ_SET_QUOTA				 "
		, "IRP_MJ_PNP					 " };

	KdPrint(("%s\n", typeIrp[stack->MajorFunction]));
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;

}

NTSTATUS DispatchRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_SUCCESS;
	ULONG Length;
	/*
	windows�е�try-except���ƣ���c++�е�try-catch����Ҫ������Щ
	���__try����������ܱ����Ĵ���ִ���г������쳣��
	���Ȱ���������˳���������������а����˳����쳣��ִ�е��try-except��䣬ִ��ÿ��except�Ĺ��˺�����
	ֱ��ĳ��try-except���Ĺ��˺������ֵΪEXCEPTION_EXECUTE_HANDLER��
	��ʱ���ٴӰ����쳣���ֵ�ִ�е�����ڲ�try-finally��俪ʼ��������ִ��ÿ��finally���еĴ��룬
	ֱ�����˵��Ǹ����˺������ֵΪEXCEPTION_EXECUTE_HANDLER��try-except���ִ����except�飬
	���ִ�и�try-except��������������롣
	
	�����÷��ɲο�msdn
	*/
	
	__try
	{
		//��ȡ�û��ռ���ڴ�ĳ���
		Length = stack->Parameters.Read.Length;
		if (Length > 512)
		{
			RtlCopyMemory(Irp->AssociatedIrp.SystemBuffer, "This is a string from kernel", strlen("This is a string from kernel"));
			status = STATUS_SUCCESS;
			Length = strlen("This is a string from kernel");
		}
		else
		{
			status = STATUS_INSUFFICIENT_RESOURCES;
			Length = 0;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		//�����쳣ʱ������GetExceptionCode����ȡ�뱾���쳣��ص�NTSTATUS��
		status = GetExceptionCode();
		KdPrint(("DispatchRead failed! 0x%x\n", status));
		Length = 0;
	}

	Irp->IoStatus.Information = Length;	//������dwRet
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;
	PDEVICE_OBJECT DeviceObject;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\HelloDDK");
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloDDK");
	DriverObject->DriverUnload = Unload;

	status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("CreateDevice failed! 0x%x\n",status));
		return status;
	}
	
	status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("CreateSymbolicLink failed! 0x%x\n",status));
		IoDeleteDevice(DeviceObject);
		return status;
	}

	ULONG i;
	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		DriverObject->MajorFunction[i] = DispatchRoutine;
	}
	DriverObject->MajorFunction[IRP_MJ_READ] = DispatchRead;

	DeviceObject->Flags |= DO_BUFFERED_IO;				//֪ͨ����ϵͳ���豸Ϊ��������д��ʽ
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;		//֪ͨ����ϵͳ���豸�ѿ���ʹ����

	return STATUS_SUCCESS;
}