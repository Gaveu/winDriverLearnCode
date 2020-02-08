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
		DriverObject,			//PDRIVER_OBJECT	DriverObject		ָ�����½��豸������ʲô�����������
		0,						//ULONG				DeviceExtensionSize	�½��豸������豸��չ���ֽڴ�С
		&DeviceName,			//PUNICODE_STRING	DeviceName			��ѡ�ָ�����豸�����������������һ��������·��
		FILE_DEVICE_UNKNOWN,	//DEVICE_TYPE		DeviceType			ָ���豸��������ͣ���FILE_DEVICE_DISK��FILE_DEVICE_KEYBOARD��
		0,						//ULONG				DeviceCharacteristics	ָ���豸���ԣ���FILE_DEVICE_SECURE_OPEN��
		FALSE,					//BOOLEAN			Exclusive			ָ���Ƿ�����չ�豸
		&DeviceObject			//PDEVICE_OBJECT	*DeviceObject		�����½����豸���󣬸��豸����λ�ڲ���ҳ�ڴ��
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("DeviceObject Create Failed!\n"));
		return status;
	}
	//�趨��־λ����ʾ���豸�����Ի����������������������ʼ��
	DeviceObject->Flags |= DO_BUFFERED_IO;			
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	return STATUS_SUCCESS;
}