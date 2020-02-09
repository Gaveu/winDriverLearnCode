#include<ntddk.h>

//windows驱动的new、delete需要手动重载
void *__cdecl operator new(size_t size)
{
	return ExAllocatePool(PagedPool, size);
}

void __cdecl operator delete(void *p)
{
	if (p)
	{
		ExFreePool(p);
	}
}

void *__cdecl operator new[](size_t size)
{
	return ExAllocatePool(PagedPool, size);
}

void __cdecl operator delete[](void *p)
{
	if (p)
	{
		ExFreePool(p);
	}
}

class MyData
{
public:
	int idata;
	char cdata;
	PCHAR pStr;

	MyData()
	{
		idata = 0;
		cdata = 0;
		pStr = NULL;
		KdPrint(("Construct Function!\n"));
	}
	~MyData()
	{
		KdPrint(("Destruct Function!\n"));
		if (pStr)
		{
			ExFreePool(pStr);
		}
	}

};

typedef MyData* pMyData;

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("Goodbye Driver!\n"));
}

extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	DriverObject->DriverUnload = Unload;
	KdPrint(("Hello Driver!\n"));

	int i;
	pMyData pData = new MyData;
	pData->cdata = 'a';
	pData->idata = 0;
	KdPrint(("cData:%c\tiData:%d\n", pData->cdata, pData->idata));
	delete pData;

	KdPrint(("*****************************\n"));
	pData = new MyData[10];
	for (i = 0; i < 10; ++i)
	{
		pData[i].cdata = 'a' + i;
		pData[i].idata = i;
	}
	for (i = 0; i < 10; ++i)
	{
		KdPrint(("cData:%c\tiData:%d\n", pData[i].cdata, pData[i].idata));
	}
	delete[] pData;

	return STATUS_SUCCESS;
}