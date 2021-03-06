#include"drvHead.h"

//进程枚举
//基于进程双链表的遍历，较快，但由于采用硬编码而有兼容性问题
VOID PsEnum1()
{
	PEPROCESS pEproc = NULL;		//获取进程地址
	PEPROCESS pFirstProc = NULL;	//保存EPROCESS进程双链表头节点
	PLIST_ENTRY pLe = NULL;
	PUCHAR pProcName = NULL;		//进程名字符串指针
	ULONG ulPid = 0;				//进程ID

	//获取当前进程EPROCESS
	pEproc = PsGetCurrentProcess();
	if (!pEproc)
	{
		KdPrint(("GetCurrentProcess Failed!\n"));
		return;
	}
	pFirstProc = pEproc;
	//KdBreakPoint();
	KdPrint(("PsEnum1!\n"));
	do
	{
		pProcName = (PUCHAR)pEproc + 0x16c;
		ulPid = *(ULONG *)((PCHAR)pEproc + 0xb4);
		if (ulPid > 300000)
		{
			break;
		}
		KdPrint(("PID:%d\tparentID:%d\t%s\n",
			ulPid,
			(ULONG)PsGetProcessInheritedFromUniqueProcessId(pEproc),
			pProcName));
		//PsEnumModule(pEproc);
		//ThEnum2(pEproc);
		pLe = (PLIST_ENTRY)((PCHAR)pEproc + 0xb8);
		pLe = pLe->Flink;
		pEproc = (PEPROCESS)((PCHAR)pLe - 0xb8);
	} while (pEproc != pFirstProc);
	KdPrint(("---------------------\n"));
}

//较通用，无兼容性问题，但为线性遍历
VOID PsEnum2()
{
	ULONG ulPid = 0;
	PEPROCESS pEproc = NULL;
	PUCHAR pProcName = NULL;
	NTSTATUS status;
	KdPrint(("PsEnum2!\n"));
	for (ulPid = 4; ulPid < 20000; ulPid += 4)
	{
		status = PsLookupProcessByProcessId((HANDLE)ulPid, &pEproc);
		if (NT_SUCCESS(status))
		{
			pProcName = PsGetProcessImageFileName(pEproc);
			KdPrint(("PID:%d\tparentID:%d\t%s\n",
				ulPid,
				(ULONG)PsGetProcessInheritedFromUniqueProcessId(pEproc),
				pProcName));
			//ThEnum3(pEproc);
		}
	}
	KdPrint(("---------------------\n"));
}

//进程暂停
//给定进程pid，暂停对应进程的执行
NTSTATUS PsSuspend(HANDLE hPid)
{
	NTSTATUS status;
	PEPROCESS pEproc = NULL;
	status = PsLookupProcessByProcessId(hPid, &pEproc);

	if (NT_SUCCESS(status))
	{
		//xp->Nt.5版本则以硬函数编码方式执行函数
		if (SharedUserData->NtMajorVersion == 5)
		{
			status = pXpPsSuspendProcess(pEproc);
		}
		else
		{
			status = PsSuspendProcess(pEproc);
		}
		if (NT_SUCCESS(status))
		{
			KdPrint(("pid:%d\t%s\t Suspend!\n", (ULONG)hPid, PsGetProcessImageFileName(pEproc)));
		}
	}
	return status;
}

//进程恢复
//给定进程pid，恢复对应进程的执行
NTSTATUS PsResume(HANDLE hPid)
{
	NTSTATUS status;
	PEPROCESS pEproc = NULL;
	status = PsLookupProcessByProcessId(hPid, &pEproc);

	if (NT_SUCCESS(status))
	{
		//xp->Nt.5版本则以硬函数编码方式执行函数
		if (SharedUserData->NtMajorVersion == 5)
		{
			status = pXpPsResumeProcess(pEproc);
		}
		else
		{
			status = PsResumeProcess(pEproc);
		}
		if (NT_SUCCESS(status))
		{
			KdPrint(("pid:%d\t%s\t Resume!\n", (ULONG)hPid, PsGetProcessImageFileName(pEproc)));
		}
	}

	return status;
}

//进程终止
//给定进程pid，终止对应进程的执行
NTSTATUS PsTerminate(HANDLE hPid)
{
	HANDLE hProc = NULL;
	CLIENT_ID clientId = { 0 };
	NTSTATUS status;
	OBJECT_ATTRIBUTES oa;
	clientId.UniqueProcess = hPid;
	InitializeObjectAttributes(
		&oa,
		NULL,
		OBJ_KERNEL_HANDLE,
		NULL,
		NULL
	);

	status = ZwOpenProcess(
		&hProc,
		PROCESS_ALL_ACCESS,
		&oa,
		&clientId
	);
	if (NT_SUCCESS(status))
	{
		status = ZwTerminateProcess(hProc, 0);
		if (NT_SUCCESS(status))
		{
			KdPrint(("PID:%d\tTerminate Success!\n", hPid));
		}
		else
		{
			KdPrint(("TerminateProcess Failed!0x%x\n", status));
		}
	}
	else
	{
		KdPrint(("OpenProcess Failed!0x%x\n", status));
	}
	ZwClose(hProc);
	return status;
}


//进程模块枚举
//给定进程的_EPROCESS结构指针，枚举其已装载的模块
VOID PsEnumModule(PEPROCESS process)
{
	if (!process)
	{
		return;
	}
	//KdBreakPoint();
	KAPC_STATE kApcState;
	NTSTATUS status;
	PPEB pPeb = PsGetProcessPeb(process);
	PLIST_ENTRY pLe;
	PLIST_ENTRY pFirLe;
	PLDR_DATA_TABLE_ENTRY pLdrDataEntry;
	if (!pPeb)
	{
		return;
	}

	//保证当前进程可以访问目标进程地址空间
	KeStackAttachProcess(process, &kApcState);
	pFirLe = pLe = pPeb->Ldr->InMemoryOrderModuleList.Flink;
	do
	{
		//根据成员变量InMemoryOrderLinks在LDR_DATA_TABLE_ENTRY结构体内的偏移，计算pLe对应的LDR_DATA_TABLE_ENTRY结构体内存首地址
		pLdrDataEntry = CONTAINING_RECORD(
			pLe,
			LDR_DATA_TABLE_ENTRY,
			InMemoryOrderLinks
		);
		if (pLdrDataEntry->DllBase)
		{
			KdPrint(("--%wZ\tBase:%p\n",
				&pLdrDataEntry->FullDllName,
				pLdrDataEntry->DllBase));
		}
		pLe = pLe->Flink;
	} while (pLe->Flink != pFirLe);
	KeUnstackDetachProcess(&kApcState);

	/*
	__try
	{
		//保证当前进程可以访问目标进程地址空间
		KeStackAttachProcess(process, &kApcState);
		pFirLe = pLe = pPeb->Ldr->InMemoryOrderModuleList.Flink;
		do
		{
			//根据成员变量InMemoryOrderLinks在LDR_DATA_TABLE_ENTRY结构体内的偏移，计算pLe对应的LDR_DATA_TABLE_ENTRY结构体内存首地址
			pLdrDataEntry = CONTAINING_RECORD(
				pLe,
				LDR_DATA_TABLE_ENTRY,
				InMemoryOrderLinks
			);
			KdPrint(("--%wZ\tBase:%p\n",
				&pLdrDataEntry->FullDllName,
				pLdrDataEntry->DllBase));
			pLe = pLdrDataEntry->InMemoryOrderLinks.Flink;
		} while (pLe != pFirLe);
		KeUnstackDetachProcess(&kApcState);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		KdPrint(("%s DLLinfo Read Failed!\n", PsGetProcessImageFileName(process)));
	}
	*/
}
