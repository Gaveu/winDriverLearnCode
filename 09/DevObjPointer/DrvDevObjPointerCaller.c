#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{

	KdPrint(("Goodbye DriverCaller!\n"));
}

NTSTATUS DrvCallerTest3()
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

	//����ͬ��ϵͳ�ļ�����IRP,�Ժ�������ֵ����ʽ���ظù���õ�IRP
	//ͬ����ʽ
	//pIrp = IoBuildSynchronousFsdRequest(
	//	IRP_MJ_READ,	//�����͵�IRP��������
	//	DeviceObject,	//IRP�����͵�Ŀ���豸����
	//	NULL,			//��������ַ
	//	0,				//����������
	//	NULL,			//��д�ֽ���ʼƫ����
	//	&event,			//ͬ���¼����󣬵�IRP���ͳɹ�����ͷŸ��¼�
	//	&IoStatus		//���ص�IO״̬��
	//	);

	//����ͬ��ϵͳ�ļ�����IRP,�Ժ�������ֵ����ʽ���ظù���õ�IRP
	//�첽��ʽ
	pIrp = IoBuildAsynchronousFsdRequest(
		IRP_MJ_READ,	//�����͵�IRP��������
		DeviceObject,	//IRP�����͵�Ŀ���豸����
		NULL,			//��������ַ
		0,				//����������
		NULL,			//��д�ֽ���ʼƫ����
		&IoStatus		//���ص�IO״̬��
		);
	pIrp->UserEvent = &event;

	//��ʼ��IOջ��Ԫ
	PIO_STACK_LOCATION stack = IoGetNextIrpStackLocation(pIrp);
	stack->FileObject = pFileObject;

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

	DrvCallerTest3();

	return STATUS_SUCCESS;
}