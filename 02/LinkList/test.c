#include<ntddk.h>

typedef struct _LinkData
{
	LIST_ENTRY ListEntry;
	int data;
}LINKDATA,*PLINKDATA;

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("GoodBye Driver!\n"));
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	DriverObject->DriverUnload = Unload;
	KdPrint(("Hello Driver!\n"));

	PLIST_ENTRY pLentry = NULL;
	int i = 0;
	PLINKDATA pData = NULL;
	LIST_ENTRY ListHead;
	InitializeListHead(&ListHead);	//初始化链表头节点，使其中的两个指针自指
	for (i = 0; i < 10; i++)
	{
		pData = (PLINKDATA)ExAllocatePool(PagedPool, sizeof(LINKDATA));
		pData->data = i;
		InsertHeadList(&ListHead, &pData->ListEntry);	//往链表头节点以头插的方式插入新节点
	}
	while (!IsListEmpty(&ListHead))
	{	//链表为空，即头节点的两个指针自指时返回true，循环结束
		pLentry = RemoveHeadList(&ListHead);
		pData = CONTAINING_RECORD(
			pLentry,	//链表入口点的指针 
			LINKDATA,	//链表外接的数据节点类型名
			ListEntry	//数据节点中入口点的变量名
			);//通过链表入口点在数据节点中的偏移，计算出数据节点的存储地址
		KdPrint(("Num:%d\n", pData->data));
	}

	return STATUS_SUCCESS;
}