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
		&npLList,		//PNPAGED_LOOKASIDE_LIST	Lookaside	����ʼ���Ŀ���
		NULL,			//PALLOCATE_FUNCTION		Allocate	���亯��
		NULL,			//PFREE_FUNCTION			Free		�ͷź���
		0,				//ULONG						Flags		������ͷ�ʱ�Ĳ�����������ֻ���ڴ��POOL_NX_ALLOCATION��
		//														����ʧ��ʱ�׳��쳣��POOL_RAISE_IF_ALLOCATION_FAILURE
		sizeof(MYDATA),	//SIZE_T					Size		ÿ�η�����ڴ���С
		'PLst',			//ULONG						Tag			����ʼ���Ŀ���ı�ǩ
		0				//USHORT					Depth		����ѡ�ֵ����Ϊ0
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
	{	//������������ڴ������ݲ�������ռ�
		KdPrint(("Free\tNo:%d\tChar:%c\n", pData[i]->idata, pData[i]->cdata));
		ExFreeToNPagedLookasideList(
			&npLList,	//PNPAGED_LOOKASIDE_LIST	Lookaside	�������ڴ�����ڵĿ���
			pData[i]	//PVOID						Entry		ָ��������ڴ���׵�ַ��ָ��
			);
	}

	ExDeleteNPagedLookasideList(&npLList);

	return STATUS_SUCCESS;
}