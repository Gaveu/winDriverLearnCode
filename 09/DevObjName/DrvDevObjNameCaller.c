#include<ntddk.h>

//δ�ĵ���������������
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

	//�����豸��Ѱ�Ҷ�Ӧ�豸����ָ��
	status = ObReferenceObjectByName(
		&DeviceName,	//�������豸������
		OBJ_CASE_INSENSITIVE,	//�������ԣ������ִ�Сд	
		NULL,			//�����÷��ʵȼ�
		FILE_ANY_ACCESS,//�����ļ�����Ȩ��
		*IoDeviceObjectType,//��������Ϊ�豸����
		KernelMode,		//�������������ں�ģʽ
		NULL,			//��������ģ��˴�����Ҫ
		&DeviceObject	//���ص��豸����
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