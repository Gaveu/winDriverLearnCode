#include"drvHead.h"
VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	//PsResume((HANDLE)2424);	//�����������calc.exe���̶�ӦpidΪ2424��ִ�гɹ�ʱ�ý��̼���ִ��

	KdPrint(("GoodBye Driver!\n"));
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;

	//PsEnum1();
	//PsEnum2();
	//PsSuspend((HANDLE)2424);	//�����������calc.exe���̶�ӦpidΪ2424��ִ�гɹ�ʱ�ý�����ִͣ��
	//PsTerminate((HANDLE)2548);	//�����������calc.exe���̶�ӦpidΪ2548��ִ�гɹ�ʱ�ý��̽���ִ��

	PsEnum1();

	return STATUS_SUCCESS;
}

