#include<ntddk.h>

VOID Unload(PDRIVER_OBJECT DriverObject)
{
	KdPrint(("Driver Unload!\n"));
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	DriverObject->DriverUnload = Unload;


	return STATUS_SUCCESS;
}