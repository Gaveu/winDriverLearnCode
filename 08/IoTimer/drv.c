#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	//ֹͣ�豸�����IO��ʱ��
	IoStopTimer(DriverObject->DeviceObject);
	IoDeleteDevice(DriverObject->DeviceObject);
	KdPrint(("Goodbye Driver!\n"));
}

VOID IoTimerRoutine(
	IN PDEVICE_OBJECT DeviceObject, 
	IN PVOID Context)
{
	KdPrint(("IoTimer Tik!\n"));
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;
	NTSTATUS status;
	PDEVICE_OBJECT DeviceObject;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\IoTimerTest");
	UNICODE_STRING SymboliclinkName = RTL_CONSTANT_STRING(L"\\??\\IoTimerTest");
	status = IoCreateDevice(
		DriverObject,
		0,
		&DeviceName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&DeviceObject
		);

	//��ʼ����ʱ��
	IoInitializeTimer(
		DeviceObject,	//��ʱ�����ڵ��豸����
		IoTimerRoutine,	//��ʱ�������Ļص�����
		NULL			//�����ģ�����ַ
		);

	//����IO��ʱ��
	IoStartTimer(DeviceObject);


	return STATUS_SUCCESS;
}