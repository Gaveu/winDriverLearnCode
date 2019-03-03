#include<ntddk.h>


VOID Unload(IN PDRIVER_OBJECT DriverObject)
{


	KdPrint(("Driver Unload!\n"));
}

extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	PCHAR pStr = (PCHAR)ExAllocatePool(PagedPool, sizeof(char) * 16);
	RtlFillMemory(pStr, sizeof(pStr), 0xcc);
	DriverObject->DriverUnload = Unload;

	KdPrint(("%s\n", pStr));

	ExFreePool(pStr);
	pStr = NULL;
	return STATUS_SUCCESS;
}