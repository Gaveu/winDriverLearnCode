#include<ntddk.h>
#include<ntstrsafe.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("GoodBye Driver!\n"));
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;
	/*
	ANSI_STRING as1 = { 
		strlen("Hello")*sizeof(CHAR), 
		(strlen("Hello") + 1)*sizeof(CHAR), 
		"Hello" };
	ANSI_STRING as2 = RTL_CONSTANT_STRING("Hello");
	ANSI_STRING as3;
	RtlInitAnsiString(&as3, "Hello");
	KdPrint(("%Z\n", &as3));

	CHAR aEstr[1024] = { 0 };
	ANSI_STRING as4;
	RtlInitEmptyAnsiString(&as4, aEstr, sizeof(aEstr));
	RtlCopyString(&as4, &as2);

	RtlCompareString(&as1, &as2, TRUE);
	RtlCompareString(&as3, &as4, FALSE);

	RtlUpperString(&as4, &as1);





	UNICODE_STRING us1 = { 
		strlen("Hello")*sizeof(WCHAR),
		(strlen("Hello") + 1)*sizeof(WCHAR),
		L"Hello" };
	UNICODE_STRING us2 = RTL_CONSTANT_STRING(L"Hello");
	UNICODE_STRING us3;
	RtlInitUnicodeString(&us3, L"Hello");
	KdPrint(("%wZ\n", &us3));

	WCHAR wEstr[1024] = { 0 };
	UNICODE_STRING us4;
	RtlInitEmptyUnicodeString(&us4, wEstr, sizeof(wEstr));
	RtlCopyUnicodeString(&us4, &us2);

	RtlCompareUnicodeString(&us1, &us2, TRUE);
	RtlCompareUnicodeString(&us3, &us4, FALSE);

	RtlUpcaseUnicodeString(&us3, &us1, TRUE);
	RtlUpcaseUnicodeString(&us4, &us1, FALSE);
	*/

	/*
	ULONG d1;
	ULONG d2;
	ULONG d3;
	ULONG d4;
	UNICODE_STRING us5;
	RtlInitUnicodeString(&us5, L"123456");
	RtlUnicodeStringToInteger(&us5, 16, &d1);
	RtlUnicodeStringToInteger(&us5, 10, &d2);
	RtlUnicodeStringToInteger(&us5, 8, &d3);
	RtlUnicodeStringToInteger(&us5, 2, &d4);

	KdPrint(("d1:%d\n", d1));
	KdPrint(("d2:%d\n", d2));
	KdPrint(("d3:%d\n", d3));
	KdPrint(("d4:%d\n", d4));

	WCHAR buf6[10] = { 0 };
	UNICODE_STRING us6;
	RtlInitEmptyUnicodeString(&us6, buf6, sizeof(buf6));
	RtlIntegerToUnicodeString(123456, 16, &us6);
	KdPrint(("us6:%wZ\n", &us6));
	*/

	/*
	UNICODE_STRING us;
	ANSI_STRING as;
	RtlInitAnsiString(&as, "This is a ANSISTRING!\n");
	RtlAnsiStringToUnicodeString(&us, &as, TRUE);

	KdPrint(("UNICODE_STRING:%wZ\n", &us));
	*/

	UNICODE_STRING us;
	UNICODE_STRING us1 = RTL_CONSTANT_STRING(L"UNICODE");
	ANSI_STRING as1 = RTL_CONSTANT_STRING("ANSIC");
	WCHAR buf[128] = { 0 };
	RtlInitEmptyUnicodeString(&us, buf, sizeof(buf));
	RtlUnicodeStringPrintf(&us, L"%d ¸ö %wZ ºÍ %d ¸ö %Z\n", 10, &us1, 3, &as1);
	KdPrint(("%wZ", &us));

	return STATUS_SUCCESS;
}