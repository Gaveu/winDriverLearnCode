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

	//创建或打开已有键
	InitializeObjectAttributes(
		&oa,						//返回的属性对象
		&KeyPath,					//键的路径
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,	//区分大小写、内核对象
		NULL,						//根路径
		NULL						//安全描述符
		);
	status = ZwCreateKey(
		&hKey,						//返回创建后的新键或打开已有键的句柄
		KEY_ALL_ACCESS,				//所有访问权限
		&oa,						//句柄属性
		0,							//设备和中间层驱动应设置为0
		NULL,						//指明该键的对象类型,用于配置管理器
		REG_OPTION_NON_VOLATILE,	//非易失,在存储本地的
		&ulRet						//返回状态
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

	//创建或打开已有分支
	InitializeObjectAttributes(
		&oa,						//返回的属性对象
		&SubName,					//分支名
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,	//区分大小写、内核对象
		hKey,						//根目录的句柄
		NULL						//安全描述符
		);
	status = ZwCreateKey(
		&hSub,						//返回创建后的新键或打开已有分支的句柄
		KEY_ALL_ACCESS,				//所有访问权限
		&oa,						//句柄属性
		0,							//设备和中间层驱动应设置为0
		NULL,						//指明该键的对象类型,用于配置管理器
		REG_OPTION_NON_VOLATILE,	//非易失,存储在本地的
		&ulRet						//返回状态
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
		&oa,						//返回的属性对象
		&KeyPath,					//键的路径
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,	//区分大小写、内核对象
		NULL,						//根路径
		NULL						//安全描述符
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
		hKey,		//待新建或修改键值的键句柄
		&valName,	//待修改键值名
		0,			//设备或驱动应设置为0
		REG_SZ,		//数据类型为空结尾unicode字符串
		data,		//键值数据
		wcslen(data)*sizeof(WCHAR)	//键值数据的字节长度
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Value Set Failed!%x\n", status));
		return;
	}
	PKEY_VALUE_BASIC_INFORMATION pKvbi = (PKEY_VALUE_BASIC_INFORMATION)ExAllocatePool(PagedPool,1024);
	status = ZwQueryValueKey(
		hKey,		//待查询键值所在键的句柄
		&valName,	//待查询键值名
		KeyValueBasicInformation,	//返回的键值信息格式
		pKvbi,		//返回的键值信息缓冲区地址
		1024,		//返回的键值信息缓冲区长度
		&ulRet		//返回写入数据长度
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
		&oa,						//返回的属性对象
		&KeyPath,					//键的路径
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,	//区分大小写、内核对象
		NULL,						//根路径
		NULL						//安全描述符
		);
	status = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &oa);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Key Open Failed!%x\n", status));
		return;
	}

	//获取hKey所对应的键的信息
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
		hKey,				//待查询键的句柄
		KeyFullInformation,	//返回的键信息类型
		pkfi,				//返回输出的缓冲区
		1024,				//输出的缓冲区长度
		&ulRet				//返回在输出缓冲区写入数据的长度
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
	{	//枚举hKey对应键下的所有子键
		RtlZeroMemory(pbi, 1024);
		status = ZwEnumerateKey(
			hKey,	//待枚举子键的键句柄
			i,		//表示被枚举的子键序号
			KeyBasicInformation,//枚举子键返回的信息类型
			pbi,	//返回信息的缓冲区
			1024,	//缓冲区长度
			&ulRet	//返回写入数据的长度
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
		KdPrint(("SubKey:%ls\n", pbi->Name));	//输出子键名
	}

	for (i = 0; i < pkfi->Values; ++i)
	{	//枚举hKey对应键下的所有键值
		RtlZeroMemory(pkvfi, 1024);
		status = ZwEnumerateValueKey(
			hKey,	//待枚举子键的键句柄
			i,		//表示被枚举的子键序号
			KeyValueFullInformation,//枚举键值返回的信息类型
			pkvfi,	//返回信息的缓冲区
			1024,	//缓冲区长度
			&ulRet	//返回写入数据的长度
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
		KdPrint(("Value:%ls\n", pkvfi->Name));	//输出键值名
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