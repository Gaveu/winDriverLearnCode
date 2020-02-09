#include<ntddk.h>

typedef struct _MYDATA
{
	INT idata;
	CHAR cdata;
}MYDATA, *PMYDATA;

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("GoodBye Driver!\n"));
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	DriverObject->DriverUnload = Unload;
	KdPrint(("Hello Driver!\n"));

	int i = 0;
	NPAGED_LOOKASIDE_LIST npLList;
	ExInitializeNPagedLookasideList(
		&npLList,		//PNPAGED_LOOKASIDE_LIST	Lookaside	待初始化的快查表
		NULL,			//PALLOCATE_FUNCTION		Allocate	分配函数
		NULL,			//PFREE_FUNCTION			Free		释放函数
		0,				//ULONG						Flags		申请和释放时的参数，如申请只读内存的POOL_NX_ALLOCATION、
		//														申请失败时抛出异常的POOL_RAISE_IF_ALLOCATION_FAILURE
		sizeof(MYDATA),	//SIZE_T					Size		每次分配的内存块大小
		'PLst',			//ULONG						Tag			待初始化的快查表的标签
		0				//USHORT					Depth		保留选项，值必须为0
		);

	PMYDATA pData[10];
	for (i = 0; i < 10; ++i)
	{
		PMYDATA pTmp = (PMYDATA)ExAllocateFromNPagedLookasideList(&npLList);
		if (!pTmp)
		{
			KdPrint(("NPagedLookasideList Allocate Failed!\n"));
			return STATUS_INSUFFICIENT_RESOURCES;
		}
		pTmp->cdata = 'a' + i;
		pTmp->idata = i;
		pData[i] = pTmp;
	}

	for (i = 0; i < 10; ++i)
	{	//逐个输出分配的内存块的数据并回收其空间
		KdPrint(("Free\tNo:%d\tChar:%c\n", pData[i]->idata, pData[i]->cdata));
		ExFreeToNPagedLookasideList(
			&npLList,	//PNPAGED_LOOKASIDE_LIST	Lookaside	待回收内存块所在的快查表
			pData[i]	//PVOID						Entry		指向待回收内存块首地址的指针
			);
	}

	ExDeleteNPagedLookasideList(&npLList);

	return STATUS_SUCCESS;
}