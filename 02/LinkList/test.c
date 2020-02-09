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
	InitializeListHead(&ListHead);	//��ʼ������ͷ�ڵ㣬ʹ���е�����ָ����ָ
	for (i = 0; i < 10; i++)
	{
		pData = (PLINKDATA)ExAllocatePool(PagedPool, sizeof(LINKDATA));
		pData->data = i;
		InsertHeadList(&ListHead, &pData->ListEntry);	//������ͷ�ڵ���ͷ��ķ�ʽ�����½ڵ�
	}
	while (!IsListEmpty(&ListHead))
	{	//����Ϊ�գ���ͷ�ڵ������ָ����ָʱ����true��ѭ������
		pLentry = RemoveHeadList(&ListHead);
		pData = CONTAINING_RECORD(
			pLentry,	//������ڵ��ָ�� 
			LINKDATA,	//������ӵ����ݽڵ�������
			ListEntry	//���ݽڵ�����ڵ�ı�����
			);//ͨ��������ڵ������ݽڵ��е�ƫ�ƣ���������ݽڵ�Ĵ洢��ַ
		KdPrint(("Num:%d\n", pData->data));
	}

	return STATUS_SUCCESS;
}