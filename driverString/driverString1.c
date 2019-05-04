#include<ntddk.h>

VOID DriverUnload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("Driver Unload!\n"));
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	DriverObject->DriverUnload = DriverUnload;
	KdPrint(("Driver Load!\n"));
	NTSTATUS status = STATUS_SUCCESS;
	__try
	{
	
		/*
		typedef struct _UNICODE_STRING 
		{
			USHORT Length;				//字节数
			USHORT MaximumLength;		//最大字节数

		//缓冲区定义
		#ifdef MIDL_PASS
			[size_is(MaximumLength / 2), length_is((Length) / 2) ] USHORT * Buffer;
		#else // MIDL_PASS
			_Field_size_bytes_part_opt_(MaximumLength, Length) PWCH   Buffer;
		#endif // MIDL_PASS
		} UNICODE_STRING;
		typedef UNICODE_STRING *PUNICODE_STRING;
		typedef const UNICODE_STRING *PCUNICODE_STRING;
		*/

		UNICODE_STRING uString = { wcslen(L"This is a unicode string!") * sizeof(WCHAR),
			(wcslen(L"This is a unicode string!") + 1) * sizeof(WCHAR),
			L"This is a unicode string!" };


		/*
		typedef struct _STRING 
		{
		    USHORT Length;			//字节数
		    USHORT MaximumLength;	//最大字节数

		//缓冲区定义
		#ifdef MIDL_PASS
		    [size_is(MaximumLength), length_is(Length) ]
		#endif // MIDL_PASS
		    _Field_size_bytes_part_opt_(MaximumLength, Length) PCHAR Buffer;
		} STRING;
		typedef STRING *PSTRING;
		typedef STRING ANSI_STRING;
		typedef PSTRING PANSI_STRING;
		*/

		ANSI_STRING aString = { strlen("This is a ansi string!") * sizeof(char),
			(strlen("This is a ansi string!") + 1) * sizeof(char),
			"This is a ansi string!" };

		//两类字符串的回显方式
		KdPrint(("%Z\n", &aString));
		KdPrint(("%wZ\n", &uString));
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		status = GetExceptionCode();
		KdPrint(("Error: 0x%x\n", status));
	}

	return STATUS_SUCCESS;
}