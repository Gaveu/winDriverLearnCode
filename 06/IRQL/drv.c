#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{


	KdPrint(("Goodbye Driver!\n"));
}

VOID IrqlTest()
{
	KIRQL kirql = KeGetCurrentIrql();

	switch (kirql)
	{
	case PASSIVE_LEVEL:
		KdPrint(("PASSIVE_LEVEL\n"));
		break;
	case APC_LEVEL:
		KdPrint(("LOW_LEVEL\n"));
		break;
	case DISPATCH_LEVEL:
		KdPrint(("DISPATCH_LEVEL\n"));
		break;
	default:
		KdPrint(("Other LEVEL\n"));
	}

	KIRQL kirqlBak;
	KeRaiseIrql(
		DISPATCH_LEVEL, //提升到DISPATCH_LEVEL级别
		&kirqlBak		//将提升前的级别输出至kirqlBak
		);

	kirql = KeGetCurrentIrql();
	switch (kirql)
	{
	case PASSIVE_LEVEL:
		KdPrint(("PASSIVE_LEVEL\n"));
		break;
	case APC_LEVEL:
		KdPrint(("LOW_LEVEL\n"));
		break;
	case DISPATCH_LEVEL:
		KdPrint(("DISPATCH_LEVEL\n"));
		break;
	default:
		KdPrint(("Other LEVEL\n"));
	}

	KeLowerIrql(kirqlBak);	//恢复原来的IRQL


	kirql = KeGetCurrentIrql();
	switch (kirql)
	{
	case PASSIVE_LEVEL:
		KdPrint(("PASSIVE_LEVEL\n"));
		break;
	case APC_LEVEL:
		KdPrint(("LOW_LEVEL\n"));
		break;
	case DISPATCH_LEVEL:
		KdPrint(("DISPATCH_LEVEL\n"));
		break;
	default:
		KdPrint(("Other LEVEL\n"));
	}


}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;

	IrqlTest();

	return STATUS_SUCCESS;
}