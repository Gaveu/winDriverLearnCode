#include<ntddk.h>

void* __cdecl operator new(size_t Size_t, POOL_TYPE poolType)
{
	if (poolType == PagedPool)
	{
		KdPrint(("New PagedPool!\n"));
	}
	if (poolType == NonPagedPool)
	{
		KdPrint(("New NonPagedPool!\n"));
	}

	return ExAllocatePool(poolType, Size_t);
}

void __cdecl operator delete(void *p)
{
	KdPrint(("Delete Run!\n"));
	if (p == NULL)
	{
		return;
	}
	ExFreePool(p);
}

class MYDATA_A
{
public:
	int Data;
	MYDATA_A()
	{
		KdPrint(("MYDATA CONSTRUCT\n"));
	}

	~MYDATA_A()
	{
		KdPrint(("MYDATA DESTRUCT\n"));
	}

	
	void* __cdecl operator new(size_t Size_t,POOL_TYPE poolType)
	{
		KdPrint(("New Class!\n"));

		if (poolType == PagedPool)
		{
			KdPrint(("New Class PagedPool!\n"));
		}
		if (poolType == NonPagedPool)
		{
			KdPrint(("New Class NonPagedPool!\n"));
		}

		return ExAllocatePool(poolType, Size_t);
	}

	void __cdecl operator delete(void *p)
	{
		KdPrint(("Delete Run!\n"));
		if (p == NULL)
		{
			return;
		}
		ExFreePool(p);
	}
	
};

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("Driver Unload!\n"));
}

extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	DriverObject->DriverUnload = Unload;
	
	MYDATA_A *pMyDataA = new(PagedPool)MYDATA_A();
	MYDATA_A *pMyDataB = new(NonPagedPool)MYDATA_A();
	delete pMyDataA;
	delete pMyDataB;

	PCHAR pChrA = new (PagedPool)char[100];
	PCHAR pChrB = new (NonPagedPool)char[100];
	//PCHAR pChrC = new char[100];
	delete pChrA;
	delete pChrB;
	//delete pChrC;

	return STATUS_SUCCESS;
}