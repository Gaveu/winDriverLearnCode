#include<ntddk.h>

typedef struct _DEVICE_EXTENSION
{
	POOL_TYPE type;
	size_t size;
	PCHAR mem;
#define FILE_MAX_SIZE 4096
}DEVICE_EXTENSION, *PDEVICE_EXTENSION;

VOID Unload(IN PDRIVER_OBJECT DriverObject);
NTSTATUS DispatchRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP PIrp);
NTSTATUS DispatchRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP PIrp);
NTSTATUS DispatchWrite(IN PDEVICE_OBJECT DeviceObject, IN PIRP PIrp);
NTSTATUS DispatchQuery(IN PDEVICE_OBJECT DeviceObject, IN PIRP PIrp);
NTSTATUS InitDeviceExtension(IN PDEVICE_EXTENSION DeviceExtension);