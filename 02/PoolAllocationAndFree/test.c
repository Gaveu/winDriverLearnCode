#include<ntddk.h>

void Unload(PDRIVER_OBJECT DriverObject)
{
	KdPrint(("GoodBye Driver!\n"));
}

//分页与非分页内存分配实验代码
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("DriverLoading!\n"));
	DriverObject->DriverUnload = Unload;
	
	PCHAR pstr = (PCHAR)ExAllocatePool(PagedPool, 1024);
	PCHAR npstr = (PCHAR)ExAllocatePool(NonPagedPool, 1024);
	if (!pstr)
	{
		KdPrint(("pagepool failed!\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	if (!npstr)
	{
		ExFreePool(pstr);
		KdPrint(("nonpagepool failed!\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	RtlZeroMemory(pstr, 1024);
	RtlZeroMemory(npstr, 1024);
	strcpy(pstr, "This is PagePool String!\n");
	strcpy(npstr, "This is NonPagePool String!\n");
	KdPrint(("%s\n", pstr));
	KdPrint(("%s\n", npstr));
	ExFreePool(pstr);
	ExFreePool(npstr);
	
	PCHAR pstrtag = (PCHAR)ExAllocatePoolWithTag(PagedPool, 1024, 'pTag');
	PCHAR npstrtag = (PCHAR)ExAllocatePoolWithTag(NonPagedPool, 1024, 'nTag');
	if (!pstrtag)
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	if (!npstrtag)
	{
		ExFreePoolWithTag(pstrtag,'pTag');
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	RtlZeroMemory(pstrtag, 1024);
	RtlZeroMemory(npstrtag, 1024);
	strcpy(pstrtag, "This is PagePoolWithTag String!\n");
	strcpy(npstrtag, "This is NonPagePoolWithTag String!\n");
	KdPrint(("%s\n", pstrtag));
	KdPrint(("%s\n", npstrtag));
	ExFreePoolWithTag(pstrtag, 'pTag');
	ExFreePoolWithTag(npstrtag, 'nTag');
	
	
	return STATUS_SUCCESS;
}