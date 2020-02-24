#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{


	KdPrint(("GoodBye Driver!\n"));
}

VOID KeyCreateTest()
{
	NTSTATUS status;
	HANDLE hKey;
	HANDLE hSub;
	OBJECT_ATTRIBUTES oa;
	ULONG ulRet;
	UNICODE_STRING KeyPath = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\SOFTWARE\\MyKey01");
	UNICODE_STRING SubName = RTL_CONSTANT_STRING(L"MySub01");

	//����������м�
	InitializeObjectAttributes(
		&oa,						//���ص����Զ���
		&KeyPath,					//����·��
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,	//���ִ�Сд���ں˶���
		NULL,						//��·��
		NULL						//��ȫ������
		);
	status = ZwCreateKey(
		&hKey,						//���ش�������¼�������м��ľ��
		KEY_ALL_ACCESS,				//���з���Ȩ��
		&oa,						//�������
		0,							//�豸���м������Ӧ����Ϊ0
		NULL,						//ָ���ü��Ķ�������,�������ù�����
		REG_OPTION_NON_VOLATILE,	//����ʧ,�ڴ洢���ص�
		&ulRet						//����״̬
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Key Create Failed!%x\n",status));
		return;
	}

	if (ulRet == REG_CREATED_NEW_KEY)
	{
		KdPrint(("New Key Created!\n"));
	}
	else if (ulRet == REG_OPENED_EXISTING_KEY)
	{
		KdPrint(("Existing Key Opened!\n"));
	}

	//����������з�֧
	InitializeObjectAttributes(
		&oa,						//���ص����Զ���
		&SubName,					//��֧��
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,	//���ִ�Сд���ں˶���
		hKey,						//��Ŀ¼�ľ��
		NULL						//��ȫ������
		);
	status = ZwCreateKey(
		&hSub,						//���ش�������¼�������з�֧�ľ��
		KEY_ALL_ACCESS,				//���з���Ȩ��
		&oa,						//�������
		0,							//�豸���м������Ӧ����Ϊ0
		NULL,						//ָ���ü��Ķ�������,�������ù�����
		REG_OPTION_NON_VOLATILE,	//����ʧ,�洢�ڱ��ص�
		&ulRet						//����״̬
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Subkey Create Failed!%x\n", status));
		ZwClose(hKey);
		return;
	}

	if (ulRet == REG_CREATED_NEW_KEY)
	{
		KdPrint(("New Subkey Created!\n"));
	}
	else if (ulRet == REG_OPENED_EXISTING_KEY)
	{
		KdPrint(("Existing Subkey Opened!\n"));
	}
	ZwClose(hKey);
	ZwClose(hSub);
}

VOID KeyOpenTest()
{
	NTSTATUS status;
	HANDLE hKey;
	OBJECT_ATTRIBUTES oa;
	ULONG ulRet;
	PWCHAR data;
	WCHAR buf[64] = { 0 };
	UNICODE_STRING valName;
	UNICODE_STRING KeyPath = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\SOFTWARE\\MyKey01");
	InitializeObjectAttributes(
		&oa,						//���ص����Զ���
		&KeyPath,					//����·��
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,	//���ִ�Сд���ں˶���
		NULL,						//��·��
		NULL						//��ȫ������
		);
	status = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &oa);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Key Open Failed!%x\n", status));
		return;
	}

	KdPrint(("Key Open Success!\n"));

	data = L"String";
	RtlInitUnicodeString(&valName, L"StringValue");
	status = ZwSetValueKey(
		hKey,		//���½����޸ļ�ֵ�ļ����
		&valName,	//���޸ļ�ֵ��
		0,			//�豸������Ӧ����Ϊ0
		REG_SZ,		//��������Ϊ�ս�βunicode�ַ���
		data,		//��ֵ����
		wcslen(data)*sizeof(WCHAR)	//��ֵ���ݵ��ֽڳ���
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Value Set Failed!%x\n", status));
		return;
	}
	PKEY_VALUE_BASIC_INFORMATION pKvbi = (PKEY_VALUE_BASIC_INFORMATION)ExAllocatePool(PagedPool,1024);
	status = ZwQueryValueKey(
		hKey,		//����ѯ��ֵ���ڼ��ľ��
		&valName,	//����ѯ��ֵ��
		KeyValueBasicInformation,	//���صļ�ֵ��Ϣ��ʽ
		pKvbi,		//���صļ�ֵ��Ϣ��������ַ
		1024,		//���صļ�ֵ��Ϣ����������
		&ulRet		//����д�����ݳ���
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Value Query Failed!%x\n", status));
		return;
	}
	switch (pKvbi->Type)
	{
	case REG_SZ:
	{
		KdPrint(("Value Query Type:REG_SZ\n"));
	}break;
	default:
	{
		KdPrint(("Value Query Type:UNKNOWN TYPE\n"));
	}
	}

	ExFreePool(pKvbi);
	ZwClose(hKey);

}

VOID KeyEnum()
{
	ULONG i;
	HANDLE hKey;
	NTSTATUS status;
	OBJECT_ATTRIBUTES oa;
	UNICODE_STRING KeyPath = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\SYSTEM\\Setup");

	InitializeObjectAttributes(
		&oa,						//���ص����Զ���
		&KeyPath,					//����·��
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,	//���ִ�Сд���ں˶���
		NULL,						//��·��
		NULL						//��ȫ������
		);
	status = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &oa);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Key Open Failed!%x\n", status));
		return;
	}

	//��ȡhKey����Ӧ�ļ�����Ϣ
	ULONG ulRet;
	PKEY_FULL_INFORMATION pkfi = (PKEY_FULL_INFORMATION)ExAllocatePool(PagedPool, 1024);
	if (!pkfi)
	{
		KdPrint(("PagedPool Allocate Failed!%x\n", STATUS_INSUFFICIENT_RESOURCES));
		ZwClose(hKey);
		return;
	}
	RtlZeroMemory(pkfi, 1024);
	status = ZwQueryKey(
		hKey,				//����ѯ���ľ��
		KeyFullInformation,	//���صļ���Ϣ����
		pkfi,				//��������Ļ�����
		1024,				//����Ļ���������
		&ulRet				//���������������д�����ݵĳ���
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Key Query Failed!%x\n", status));
		ZwClose(hKey);
		return;
	}
	PKEY_BASIC_INFORMATION pbi = (PKEY_BASIC_INFORMATION)ExAllocatePool(PagedPool, 1024);
	if (!pbi)
	{
		KdPrint(("PagedPool Allocate Failed!%x\n", STATUS_INSUFFICIENT_RESOURCES));
		ZwClose(hKey);
		ExFreePool(pkfi);
		return;
	}
	RtlZeroMemory(pbi, 1024);
	PKEY_VALUE_FULL_INFORMATION pkvfi = (PKEY_VALUE_FULL_INFORMATION)ExAllocatePool(PagedPool, 1024);
	if (!pkvfi)
	{
		KdPrint(("PagedPool Allocate Failed!%x\n", STATUS_INSUFFICIENT_RESOURCES));
		ZwClose(hKey);
		ExFreePool(pkfi);
		ExFreePool(pbi);
		return;
	}
	RtlZeroMemory(pkvfi, 1024);

	for (i = 0; i < pkfi->SubKeys; ++i)
	{	//ö��hKey��Ӧ���µ������Ӽ�
		RtlZeroMemory(pbi, 1024);
		status = ZwEnumerateKey(
			hKey,	//��ö���Ӽ��ļ����
			i,		//��ʾ��ö�ٵ��Ӽ����
			KeyBasicInformation,//ö���Ӽ����ص���Ϣ����
			pbi,	//������Ϣ�Ļ�����
			1024,	//����������
			&ulRet	//����д�����ݵĳ���
			);
		if (!NT_SUCCESS(status))
		{
			KdPrint(("SubKey Enumerate Failed!%x\n", status));
			ZwClose(hKey);
			ExFreePool(pkfi);
			ExFreePool(pbi);
			ExFreePool(pkvfi);
			return;
		}
		KdPrint(("SubKey:%ls\n", pbi->Name));	//����Ӽ���
	}

	for (i = 0; i < pkfi->Values; ++i)
	{	//ö��hKey��Ӧ���µ����м�ֵ
		RtlZeroMemory(pkvfi, 1024);
		status = ZwEnumerateValueKey(
			hKey,	//��ö���Ӽ��ļ����
			i,		//��ʾ��ö�ٵ��Ӽ����
			KeyValueFullInformation,//ö�ټ�ֵ���ص���Ϣ����
			pkvfi,	//������Ϣ�Ļ�����
			1024,	//����������
			&ulRet	//����д�����ݵĳ���
			);
		if (!NT_SUCCESS(status))
		{
			KdPrint(("Value Enumerate Failed!%x\n", status));
			ZwClose(hKey);
			ExFreePool(pkfi);
			ExFreePool(pbi);
			ExFreePool(pkvfi);
			return;
		}
		KdPrint(("Value:%ls\n", pkvfi->Name));	//�����ֵ��
	}

	KdPrint(("Key Enumerate Successed!\n"));

	ZwClose(hKey);
	ExFreePool(pkfi);
	ExFreePool(pbi);
	ExFreePool(pkvfi);
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{

	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;

	KeyEnum();

	return STATUS_SUCCESS;
}