#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("Goodbye Driver!\n"));
}


VOID DateTest()
{
	LARGE_INTEGER systemTime;
	LARGE_INTEGER localTime;
	TIME_FIELDS timefields;
	
	//��ȡ��ǰϵͳʱ��
	KeQuerySystemTime(&systemTime);
	//��ȡ����ǰϵͳʱ��ת��Ϊ����ʱ��
	ExSystemTimeToLocalTime(&systemTime, &localTime);
	//�ɱ���ʱ����ʱ��
	RtlTimeToTimeFields(&localTime, &timefields);
	//ʱ��ʹ������
	KdPrint(("%d-%d-%d %d:%d:%d.%.3d\n",
		timefields.Year,
		timefields.Month,
		timefields.Day,
		timefields.Hour,
		timefields.Minute,
		timefields.Second,
		timefields.Milliseconds
		));

}

NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;
	DateTest();
	return STATUS_SUCCESS;
}