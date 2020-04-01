#include<ntifs.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("Goodbye Driver!\n"));

}

VOID GetDeviceInfo(IN PDEVICE_OBJECT DeviceObject)
{
	PDEVICE_OBJECT pDObj = DeviceObject;
	POBJECT_NAME_INFORMATION pNameInfo = (POBJECT_NAME_INFORMATION)ExAllocatePool(PagedPool, 1024);
	ULONG rtnLen;
	NTSTATUS status;

	if (!pNameInfo)
	{
		KdPrint(("PagedPool Allocation Failed!\n"));
		return;
	}

	while (pDObj)
	{
		RtlZeroMemory(pNameInfo, 1024);
		status = ObQueryNameString(
			pDObj,
			pNameInfo,
			1024,
			&rtnLen
			);
		if (NT_SUCCESS(status))
		{
			KdPrint(("DriverObject:%wZ\tDeviceObject:%wZ\n",
				&pDObj->DriverObject->DriverName,
				&pNameInfo->Name
				));
		}
		else
		{
			KdPrint(("NameString Query Failed!\n"));
		}
		pDObj = pDObj->AttachedDevice;
	}

	ExFreePool(pNameInfo);
}

void EnumDriver(IN PUNICODE_STRING DeviceName)
{
	NTSTATUS status;
	PFILE_OBJECT pFObj;
	PDEVICE_OBJECT pDObj;

	status = IoGetDeviceObjectPointer(
		DeviceName,
		FILE_ANY_ACCESS,
		&pFObj,
		&pDObj
		);
	if (NT_SUCCESS(status))
	{
		while (pDObj)
		{
			GetDeviceInfo(pDObj);
			pDObj = pDObj->NextDevice;
		}

	}
	else
	{
		KdPrint(("DeviceName Find Failed!\n"));
	}

}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\Ndis");
	EnumDriver(&DeviceName);

	return STATUS_SUCCESS;
}