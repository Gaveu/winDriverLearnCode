#include<ntddk.h>

typedef struct _MYDATA
{
	int number;
}MYDATA,*PMYDATA;

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("Driver Unload!\n"));
}

VOID aside()
{
	PMYDATA pData[100];
	NPAGED_LOOKASIDE_LIST lookasideList;
	int i;
	ExInitializeNPagedLookasideList(&lookasideList, NULL, NULL, 0, sizeof(MYDATA), 'LIST', 0);
	KdPrint(("lookaside test!\n"));
	for (i = 0; i < 100; ++i)
	{
		pData[i] = (MYDATA *)ExAllocateFromNPagedLookasideList(&lookasideList);
		pData[i]->number = i;
		
	}

	for (i = 0; i < 100; ++i)
	{
		KdPrint(("%d", pData[i]->number));
		ExFreeToNPagedLookasideList(&lookasideList, pData[i]);

	}
	ExDeleteNPagedLookasideList(&lookasideList);

	KdPrint(("lookaside test end!\n"));

}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Driver Run!\n"));
	DriverObject->DriverUnload = Unload;
	aside();
		
	return STATUS_SUCCESS;
}