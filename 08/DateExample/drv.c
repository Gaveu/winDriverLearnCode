#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("Goodbye Driver!\n"));
}


VOID DateTest()
{
	LARGE_INTEGER systemTime;
	LARGE_INTEGER localTime;
	TIME_FIELDS timefields;
	
	//获取当前系统时间
	KeQuerySystemTime(&systemTime);
	//获取将当前系统时间转换为本地时间
	ExSystemTimeToLocalTime(&systemTime, &localTime);
	//由本地时间获得时域
	RtlTimeToTimeFields(&localTime, &timefields);
	//时域使用样例
	KdPrint(("%d-%d-%d %d:%d:%d.%.3d\n",
		timefields.Year,
		timefields.Month,
		timefields.Day,
		timefields.Hour,
		timefields.Minute,
		timefields.Second,
		timefields.Milliseconds
		));

}

NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;
	DateTest();
	return STATUS_SUCCESS;
}