#include<ntddk.h>

PCHAR PsGetProcessImageFileName(IN PEPROCESS Process);

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("Driver unload!\n"));
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Driver load!\n"));
	DriverObject->DriverUnload = Unload;
	//KdPrint(("%s\n", PsGetProcessImageFileName(PsGetCurrentProcess())));
	PCHAR pcstr = (PCHAR)ExAllocatePool(NonPagedPool, 1024);
	if (!pcstr)
	{
		KdPrint(("PagedPool allocate failed!\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	KdPrint(("PagedPool allocate successed!\n"));
	RtlZeroMemory(pcstr, 1024);
	strcpy(pcstr, "This is a test of memory allocate!");
	KdPrint(("%s\n", pcstr));

	ExFreePool(pcstr);
	pcstr = NULL;
	KdPrint(("PagedPool free successed!\n"));

	return STATUS_SUCCESS;
}