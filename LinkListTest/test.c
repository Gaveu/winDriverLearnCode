#include<ntddk.h>

typedef struct _MYDATA
{
	LIST_ENTRY ListEntry;
	int pos;
}MYDATA, *PMYDATA;

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("Driver Unload!\n"));
}

VOID LinkListTest()
{
	int i;
	PMYDATA ptr_MyData;
	PLIST_ENTRY pEntry;
	LIST_ENTRY listHead;
	InitializeListHead(&listHead);//listHead.Blink = listHead.Flink = &listHead

	//Insert Data
	for (i = 0; i < 10; ++i)
	{
		ptr_MyData = (PMYDATA)ExAllocatePool(NonPagedPool, sizeof(MYDATA));
		InsertHeadList(&listHead, &(ptr_MyData->ListEntry));
		ptr_MyData->pos = i;
	}

	//Get&Remove Data
	while (!IsListEmpty(&listHead))
	{
		pEntry = RemoveTailList(&listHead);
		ptr_MyData = CONTAINING_RECORD(pEntry, MYDATA, ListEntry);
		KdPrint(("%d\n", ptr_MyData->pos));
		ExFreePool(ptr_MyData);
	}
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Driver Load!\n"));
	DriverObject->DriverUnload = Unload;
	LinkListTest();

	return STATUS_SUCCESS;
}