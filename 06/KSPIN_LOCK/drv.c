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

VOID SpinLockTest()
{

	KIRQL kirql = KeGetCurrentIrql();
	KIRQL kirqlBak;
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

	KSPIN_LOCK kslck;
	KeInitializeSpinLock(&kslck);

	KeAcquireSpinLock(
		&kslck,		//待拿锁的锁变量 
		&kirqlBak	//返回拿锁前的IRQL
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

	KeReleaseSpinLock(
		&kslck,		//待释放的锁变量 
		kirqlBak	//恢复至拿锁前的IRQL
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
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;


	SpinLockTest();

	return STATUS_SUCCESS;
}