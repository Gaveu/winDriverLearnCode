#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{

	KdPrint(("Goodbye DriverCaller!\n"));
}

NTSTATUS DrvCallerTest4()
{
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\DriverA");
	NTSTATUS status;
	KEVENT event;
	IO_STATUS_BLOCK IoStatus;
	PFILE_OBJECT pFileObject;
	PDEVICE_OBJECT DeviceObject;
	PIRP pIrp;

	//�����豸��Ѱ�Ҷ�Ӧ�豸����ָ�뼰���ļ�����ָ��
	status = IoGetDeviceObjectPointer(
		&DeviceName,	//��Ѱ�ҵ��豸��
		FILE_ANY_ACCESS,//�ļ�����Ȩ��
		&pFileObject,	//���ص��ļ�����ָ��
		&DeviceObject	//���ص��豸����ָ��
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("DeviceObject Find Failed!%x\n", status));
		return status;
	}

	KdPrint(("DeviceObject Get Success!\n"));

	KeInitializeEvent(
		&event,
		SynchronizationEvent,
		FALSE
		);


	//���й���һ��IRP���󣬲����г�ʼ��������Ӧ��ջ��Ԫ
	pIrp = IoAllocateIrp(//��ջ������һ��Irp�Ŀռ�
		DeviceObject->StackSize,	//ָ��������Irp��I/O����ջ�е�λ��
		FALSE						//��ѡ�����
		);
	if (pIrp != NULL)
	{
		//����IRP
		pIrp->UserIosb = &IoStatus;	//����IO״̬��
		pIrp->UserEvent = &event;	//�����¼�����
		pIrp->AssociatedIrp.SystemBuffer = NULL;	//���ö�д������
		pIrp->Tail.Overlay.Thread = PsGetCurrentThread();	//����IRP��Դ�߳�
		//��ʼ��IOջ��Ԫ
		PIO_STACK_LOCATION stack = IoGetNextIrpStackLocation(pIrp);
		stack->FileObject = pFileObject;	//����ջ��Ԫ���ļ�����
		stack->DeviceObject = DeviceObject;	//����IRP������豸����
		stack->MajorFunction = IRP_MJ_READ;	//����IRP�������������
		stack->MinorFunction = IRP_MN_NORMAL;//����IRP������ӹ�����
	}

	//��ָ���豸������IRP����
	KdPrint(("Read Start!\n"));
	status = IoCallDriver(DeviceObject, pIrp);
	KdPrint(("Read Wait!\n"));
	KeWaitForSingleObject(
		&event,
		Executive,
		KernelMode,
		FALSE,
		NULL
		);
	KdPrint(("Read Finished!\n"));

	//�����ļ�����
	ObDereferenceObject(pFileObject);

	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello DriverCaller!\n"));
	DriverObject->DriverUnload = Unload;

	DrvCallerTest4();

	return STATUS_SUCCESS;
}